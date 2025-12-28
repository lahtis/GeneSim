#include "cJSON.h"
#include "loader.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

// Apufunktio: Lukee koko tiedoston muistiin (cJSON tarvitsee tämän)
char* read_file_to_string(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = malloc(size + 1);
    if (buffer) {
        fread(buffer, 1, size, fp);
        buffer[size] = '\0';
    }
    fclose(fp);
    return buffer;
}

cJSON* load_master_config(const char *filename) {
    char *json_str = read_file_to_string(filename);
    if (!json_str) return NULL;

    cJSON *json = cJSON_Parse(json_str);
    free(json_str);
    return json;
}

Config *load_config(const char *filename) {
    ConfigEntry entries[MAX_ENTRIES];
    int n = read_config(filename, entries, MAX_ENTRIES);

    if (n == 0) return NULL;

    Config *cfg = calloc(1, sizeof(Config));
    if (!cfg) return NULL;

    const char *val;

    // Tekstiasetukset
    if ((val = find_config(entries, n, "locale"))) cfg->locale = strdup(val);
    if ((val = find_config(entries, n, "country"))) cfg->country = strdup(val);
    if ((val = find_config(entries, n, "master_config"))) cfg->master_config_path = strdup(val);
    if ((val = find_config(entries, n, "default_region"))) cfg->default_region = strdup(val);

    // Numeriset asetukset
    if ((val = find_config(entries, n, "year"))) cfg->year = atoi(val);
    if ((val = find_config(entries, n, "count"))) cfg->count = atoi(val);
    if ((val = find_config(entries, n, "output_file"))) cfg->output_file = strdup(val);
    if ((val = find_config(entries, n, "output_format"))) cfg->output_format = strdup(val);

    // Linkkuveitsen toiminnallisuus ja logiikka
    if ((val = find_config(entries, n, "verbose"))) cfg->verbose = atoi(val);
    if ((val = find_config(entries, n, "use_historical_logic"))) cfg->use_historical_logic = atoi(val);
    if ((val = find_config(entries, n, "deterministic_seed"))) cfg->deterministic_seed = atoi(val);

    // UUSI: Painotusten kytkin (0 = tasajakauma, 1 = käytä painoja)
    if ((val = find_config(entries, n, "use_weights"))) {
        cfg->use_weights = atoi(val);
    } else {
        cfg->use_weights = 1; // Turvallinen oletus
    }

    // Validointi
    if (!cfg->master_config_path) {
        fprintf(stderr, "Error: 'master_config' path not found in %s\n", filename);
        free_config(cfg);
        return NULL;
    }

    return cfg;
}

void free_config(Config *cfg) {
    if (!cfg) return;

    // 1. Vapautetaan yksittäiset merkkijonot
    free(cfg->locale);
    free(cfg->country);
    free(cfg->master_config_path);
    free(cfg->default_region);
    free(cfg->output_file);
    free(cfg->output_format);

    // 2. Vapautetaan dynaaminen säätyavainsanalista (v1.3.2)
    if (cfg->elite_keywords) {
        for (int i = 0; i < cfg->elite_key_count; i++) {
            if (cfg->elite_keywords[i]) {
                free(cfg->elite_keywords[i]);
            }
        }
        free(cfg->elite_keywords);
    }

    // 3. Vapautetaan lopuksi itse rakenne
    free(cfg);
}

void list_file_periods(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("The file %s could not be opened.\n", filename);
        return;
    }

    char line[1024];
    if (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0; // Siivotaan rivinvaihto

        char *token;
        char *rest = line;
        int idx = 0;

        printf("\nFound periods in file [%s]:\n", filename);
        printf("--------------------------------------------------\n");

        // Go through the header row
        while ((token = strtok_r(rest, ";,", &rest))) {
            printf("Index %d: %s\n", idx, token);
            idx++;
        }
        printf("--------------------------------------------------\n");
    }
    fclose(fp);
}

    // Windows-yhteensopiva versio strsep-funktiosta
    char *my_strsep(char **stringp, const char *delim) {
        char *start = *stringp;
        char *p;
        if (start == NULL) return NULL;
        p = strpbrk(start, delim);
        if (p) {
            *p = '\0';
            *stringp = p + 1;
        } else {
            *stringp = NULL;
        }
        return start;
    }

    // Apufunktio nimen validointiin (kuten demokoodissasi)
    int is_valid_name(const char *name) {
        if (!name || *name == '\0') return 0;
        for (int i = 0; name[i]; i++) {
            if (!isalpha((unsigned char)name[i]) && !isspace((unsigned char)name[i]) && name[i] != '-') {
                return 0;
            }
        }
        return 1;
    }

NameEntry *load_names(const char *filename, int target_period, int *count, int verbose, int max_cols, NameEntry *old_entries, int use_weights) {
    if (!filename) return old_entries;
    FILE *fp = fopen(filename, "r");
    if (!fp) return old_entries;

    int capacity = (*count > 0) ? *count + 500 : 500;
    NameEntry *entries = (old_entries == NULL) ? malloc(capacity * sizeof(NameEntry)) : realloc(old_entries, capacity * sizeof(NameEntry));

    char line[4096];
    fgets(line, sizeof(line), fp); // Otsikko

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        char *ptr = line;
        char *cols[64];
        int col_count = 0;

        // 1. Pilkotaan rivi sarakkeisiin
        char *token;
        while ((token = my_strsep(&ptr, ";")) != NULL && col_count < 64) {
            cols[col_count++] = token;
        }

        char name_buf[64] = {0};
        double weight_val = 1.0;
        int found = 0;


        // 2. LOGIIKKA: Onko nimi ja paino samassa solussa (Sukunimet) vai eri sarakkeissa (Etunimet)?
        int col_idx = target_period - 1;
        if (col_idx < 0) col_idx = 0;
        if (col_idx >= col_count) col_idx = col_count - 1;

if (verbose && col_idx < col_count) {
        //    printf("[DEBUG-CSV] File: %s | Col: %d | Raw: '%s'\n", filename, col_idx, cols[col_idx]);
        }

char *target_cell = cols[col_idx];
if (!target_cell) continue;

char *comma = strchr(target_cell, ',');

if (comma) {
    // SUKUNIMET (Aalto,75)
    *comma = '\0'; // Katkaistaan merkkijono pilkun kohdalta
    char *name_part = target_cell;
    double weight_part = atof(comma + 1);

    if (verbose && weight_part > 0) {
        printf("[DEBUG-LOAD] SUCCESS: '%s' (Weight: %.1f)\n", name_part, weight_part);
    }
    if (weight_part > 0.0) {
        if (*count >= capacity) {
            capacity += 200;
            entries = realloc(entries, capacity * sizeof(NameEntry));
        }
        entries[*count].name = strdup(name_part);
        entries[*count].weight = use_weights ? weight_part : 1.0;
        (*count)++;
    }

}
else if (col_count > 1 && cols[0] != NULL) {
    // ETUNIMET (Nimi sarakkeessa 0, Paino sarakkeessa col_idx)
    double w = atof(target_cell);
    if (w > 0 || target_cell[0] != '-') {
        if (*count >= capacity) {
            capacity += 200;
            entries = realloc(entries, capacity * sizeof(NameEntry));
        }
        entries[*count].name = strdup(cols[0]);
        entries[*count].weight = use_weights ? w : 1.0;
        (*count)++;
    }
}

        // 3. TALLENNUS
        if (found && strlen(name_buf) > 0) {
            if (*count >= capacity) {
                capacity += 200;
                entries = realloc(entries, capacity * sizeof(NameEntry));
            }
            entries[*count].name = strdup(name_buf);
            entries[*count].weight = use_weights ? weight_val : 1.0;
            (*count)++;
        }
    }

    fclose(fp);
    return entries;
}

 // Säädyn tunnistus (Alustava logiikka)
int get_social_status(const char *occ) {
    if (!occ) return 0;
    // Säätyläis- tai ylemmän keskiluokan avainsanat
    if (strstr(occ, "Farmaseutti") || strstr(occ, "Kauppias") ||
        strstr(occ, "Lääkäri") || strstr(occ, "Maisteri")) {
        return 1; // Elite / Middle class
    }
    return 0; // Labor / Common
}

void load_social_logic_dynamic(Config *cfg) {
    // 1. Lue tiedosto merkkijonoksi (käytetään aiempaa apufunktiota)
    char *json_data = read_file_to_string("data/social_logic.json");
    if (!json_data) {
        printf("[ERROR] social_logic.json ei löytynyt!\n");
        return;
    }

    cJSON *root = cJSON_Parse(json_data);
    if (!root) {
        free(json_data);
        return;
    }

    // 2. Navigoidaan rakenteessa: locales -> FI -> periods
    cJSON *locales = cJSON_GetObjectItem(root, "locales");
    cJSON *fi = cJSON_GetObjectItem(locales, "FI");
    cJSON *periods = cJSON_GetObjectItem(fi, "periods");

    // 3. Etsitään oikea aikakausi vuoden perusteella
    cJSON *period = NULL;
    cJSON_ArrayForEach(period, periods) {
        cJSON *years_node = cJSON_GetObjectItem(period, "years");
        if (!years_node) continue;

        int start_yr, end_yr;
        if (sscanf(years_node->valuestring, "%d-%d", &start_yr, &end_yr) == 2) {
            if (cfg->year >= start_yr && cfg->year <= end_yr) {

                // 4. Löydettiin oikea vuosi! Ladataan avainsanat.
                cJSON *sc = cJSON_GetObjectItem(period, "social_classes");
                cJSON *ek = cJSON_GetObjectItem(sc, "elite_keywords");

                if (ek && cJSON_IsArray(ek)) {
                    cfg->elite_key_count = cJSON_GetArraySize(ek);
                    cfg->elite_keywords = malloc(cfg->elite_key_count * sizeof(char*));

                    for (int i = 0; i < cfg->elite_key_count; i++) {
                        cJSON *item = cJSON_GetArrayItem(ek, i);
                        cfg->elite_keywords[i] = strdup(item->valuestring);

                        if (cfg->verbose) {
                            printf("  [DEBUG] Social Logic: Added elite keyword '%s'\n", cfg->elite_keywords[i]);
                        }
                    }
                }
                break; // Lopetetaan etsintä, kun oikea kausi löytyi
            }
        }
    }

    cJSON_Delete(root);
    free(json_data);
}

// Uusi load_all_data_with_config, joka käyttää JSONia
NameData* load_all_data_with_config(Config *cfg, int target_period, int verbose) {
    if (!cfg || !cfg->locale) return NULL;

    // 1. Muodostetaan polku master-konfiguraatioon
    char master_path[512];
    snprintf(master_path, sizeof(master_path), "data/%s/%s",
             cfg->locale,
             cfg->master_config_path ? cfg->master_config_path : "namegen_master_config.json");

    if (verbose) printf("[INFO] Opening master config: %s\n", master_path);

    // 2. Luetaan JSON muistiin
    char *json_data = read_file_to_string(master_path);
    if (!json_data) {
        fprintf(stderr, "[ERROR] Could not read file: %s\n", master_path);
        return NULL;
    }

    cJSON *root = cJSON_Parse(json_data);
    if (!root) {
        fprintf(stderr, "[ERROR] JSON parse error in %s\n", master_path);
        free(json_data);
        return NULL;
    }

    // 3. Alustetaan NameData-rakenne
    NameData *nd = calloc(1, sizeof(NameData));
    if (!nd) { cJSON_Delete(root); free(json_data); return NULL; }

    // 4. Käydään läpi "resources" -taulukko
    cJSON *resources = cJSON_GetObjectItem(root, "data_sources");
if (cJSON_IsArray(resources)) {
        cJSON *res = NULL;
        cJSON_ArrayForEach(res, resources) {
            // 2. KORJAUS: Haetaan tyyppi, sukupuoli ja kategoria (id:tä ei ole)
            cJSON *type_item = cJSON_GetObjectItem(res, "type");
            cJSON *gender_item = cJSON_GetObjectItem(res, "gender");
            cJSON *cat_item = cJSON_GetObjectItem(res, "category");
            cJSON *file_item = cJSON_GetObjectItem(res, "file");
            cJSON *my_item = cJSON_GetObjectItem(res, "max_game_year");

            if (!type_item || !file_item || !my_item) continue;

            const char *type = type_item->valuestring;
            const char *filename = file_item->valuestring;
            const char *gender = gender_item ? gender_item->valuestring : "";
            const char *category = cat_item ? cat_item->valuestring : "";
            int max_year = my_item->valueint;

            char full_csv_path[512];
            snprintf(full_csv_path, sizeof(full_csv_path), "data/%s/%s", cfg->locale, filename);

            // Valitaan sarake (p-parametri vs tiedoston maksimi)
            int safe_col = target_period;

            // Jos pyydetään kautta 11, mutta tiedoston max_year on 7, safe_col on 7.
            if (safe_col > max_year) {
                safe_col = max_year;
            }
            // Varmistetaan, ettei mennä alle ykkösen
            if (safe_col < 1) safe_col = 1;

            if (verbose) {
                printf("[DEBUG] Processing %s | Period: %d | Max: %d | Final Col: %d\n",
                filename, target_period, max_year, safe_col - 1);
            }

            // 3. KORJAUS: Tunnistuslogiikka JSON-kenttien perusteella + use_weights tuki
            if (strcmp(type, "first_name") == 0) {
                if (strcmp(gender, "male") == 0)
                    nd->m1 = load_names(full_csv_path, safe_col, &nd->m1_count, verbose, max_year, nd->m1, cfg->use_weights);
                else
                    nd->f1 = load_names(full_csv_path, safe_col, &nd->f1_count, verbose, max_year, nd->f1, cfg->use_weights);
            }
            else if (strcmp(type, "middle_name") == 0) {
                if (strcmp(gender, "male") == 0)
                    nd->m2 = load_names(full_csv_path, safe_col, &nd->m2_count, verbose, max_year, nd->m2, cfg->use_weights);
                else
                    nd->f2 = load_names(full_csv_path, safe_col, &nd->f2_count, verbose, max_year, nd->f2, cfg->use_weights);
            }
            // 1. Laajennettu tunnistus: katsotaan tyyppiä TAI tiedostonimeä
            else if (strcmp(type, "occupation") != 0 && strcmp(type, "first_name") != 0 && strcmp(type, "middle_name") != 0) {
                    if (strstr(filename, "saatylaiset") || strstr(filename, "ruotsalaiset") || strcmp(type, "elite") == 0) {
        if (verbose) printf(" +[DEBUG] -> Routing to ELITE pool\n");
        int old_count_e = nd->l_elite_count;
        nd->l_elite = load_names(full_csv_path, safe_col, &nd->l_elite_count, verbose, max_year, nd->l_elite, cfg->use_weights);
    } else {
        if (verbose) printf(" +[DEBUG] -> Routing to COMMON pool\n");
        int old_count = nd->l_count;
        nd->l = load_names(full_csv_path, safe_col, &nd->l_count, verbose, max_year, nd->l, cfg->use_weights);
        if (verbose) printf(" +[DEBUG] -> Loaded %d new names Common pool. (Total: %d)\n", (nd->l_count - old_count), nd->l_count);
    }
}
            else if (strcmp(type, "occupation") == 0) {
                if (strcmp(category, "child") == 0) {
                    if (strcmp(gender, "male") == 0)
                        nd->occupations_child_m = load_names(full_csv_path, safe_col, &nd->occ_cm_count, verbose, max_year, NULL, cfg->use_weights);
                    else
                        nd->occupations_child_f = load_names(full_csv_path, safe_col, &nd->occ_cf_count, verbose, max_year, NULL, cfg->use_weights);
                } else {
                    if (strcmp(gender, "male") == 0)
                        nd->occupations_m = load_names(full_csv_path, safe_col, &nd->occ_m_count, verbose, max_year, NULL, cfg->use_weights);
                    else
                        // KORJATTU: nd->occupations_f (oli nd->occupations_m)
                        nd->occupations_f = load_names(full_csv_path, safe_col, &nd->occ_f_count, verbose, max_year, NULL, cfg->use_weights);
                }
            }
        }
    }

    // 5. Siivous
    cJSON_Delete(root);
    free(json_data);
    return nd;
}

void free_all_data(NameData *nd) {
    if (!nd) return;

    // Päivitetty: Nyt listoja on yhteensä 10 (lisätty l_elite)
    NameEntry *lists[] = {
        nd->m1, nd->m2, nd->f1, nd->f2,
        nd->l, nd->l_elite, // Sukunimet ja säätyläiset
        nd->occupations_m, nd->occupations_f,
        nd->occupations_child_m, nd->occupations_child_f
    };

    int counts[] = {
        nd->m1_count, nd->m2_count, nd->f1_count, nd->f2_count,
        nd->l_count, nd->l_elite_count,
        nd->occ_m_count, nd->occ_f_count,
        nd->occ_cm_count, nd->occ_cf_count
    };

    // Muutettu silmukan koko 9 -> 10
    for (int j = 0; j < 10; j++) {
        if (lists[j]) {
            for (int i = 0; i < counts[j]; i++) {
                if (lists[j][i].name) {
                    free(lists[j][i].name);
                }
            }
            free(lists[j]);
        }
    }

    free(nd);
}
