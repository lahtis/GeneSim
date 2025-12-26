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

    if (n == 0) return NULL; // Tiedostoa ei löytynyt tai se oli tyhjä

    Config *cfg = calloc(1, sizeof(Config));
    if (!cfg) return NULL;

    // Käytetään find_config-funktiota arvojen poimimiseen
    const char *val;

    if ((val = find_config(entries, n, "locale"))) cfg->locale = strdup(val);
    if ((val = find_config(entries, n, "country"))) cfg->country = strdup(val);
    if ((val = find_config(entries, n, "master_config"))) cfg->master_config_path = strdup(val);
    if ((val = find_config(entries, n, "default_region"))) cfg->default_region = strdup(val);

    if ((val = find_config(entries, n, "year"))) cfg->year = atoi(val);
    if ((val = find_config(entries, n, "count"))) cfg->count = atoi(val);
    if ((val = find_config(entries, n, "output_file"))) cfg->output_file = strdup(val);
    if ((val = find_config(entries, n, "output_format"))) cfg->output_format = strdup(val);

    if ((val = find_config(entries, n, "verbose"))) cfg->verbose = atoi(val);
    if ((val = find_config(entries, n, "use_historical_logic"))) cfg->use_historical_logic = atoi(val);
    if ((val = find_config(entries, n, "deterministic_seed"))) cfg->deterministic_seed = atoi(val);

    // Validointi
    if (!cfg->master_config_path) {
        fprintf(stderr, "Error: 'namegen_master_config' path not found in %s\n", filename);
        free_config(cfg);
        return NULL;
    }

    return cfg;
}

void free_config(Config *cfg) {
    if (!cfg) return;
    free(cfg->locale);
    free(cfg->country);
    free(cfg->master_config_path);
    free(cfg->default_region);
    free(cfg->output_file);
    free(cfg->output_format);
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

NameEntry *load_names(const char *filename, int target_period, int *count, int verbose, int max_cols, NameEntry *old_entries) {
    if (!filename) return old_entries;
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        if (verbose) printf("ERROR: Failed to open file: %s\n", filename);
        return old_entries;
    }

    int names_from_this_file = 0; // Lisätään tämä laskuri debug-tulostetta varten
    int capacity = (*count > 0) ? *count + 200 : 200;
    NameEntry *entries;

    if (old_entries == NULL) {
        entries = malloc(capacity * sizeof(NameEntry));
        *count = 0;
    } else {
        entries = realloc(old_entries, capacity * sizeof(NameEntry));
    }

    if (!entries) { fclose(fp); return old_entries; }

    char line[2048];
    fgets(line, sizeof(line), fp); // Otsikko

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        char *ptr = line;
        char *token = NULL;
        int current_col = 0;

        while ((token = my_strsep(&ptr, ";")) != NULL) {
            if (current_col == target_period) {
                if (strcmp(token, "-,0") == 0 || strlen(token) < 3) break;

                char *comma = strchr(token, ',');
                if (comma) {
                    *comma = '\0';
                    char *name_part = token;
                    double weight_part = atof(comma + 1);

                    if (weight_part > 0.0 && is_valid_name(name_part)) {
                        if (*count >= capacity) {
                            capacity += 100;
                            NameEntry *temp = realloc(entries, capacity * sizeof(NameEntry));
                            if (!temp) { fclose(fp); return entries; }
                            entries = temp;
                        }
                        entries[*count].name = strdup(name_part);
                        entries[*count].weight = weight_part;
                        (*count)++;
                        names_from_this_file++; // Kasvatetaan tiedostokohtaista laskuria
                    }
                }
                break;
            }
            current_col++;
        }
    }

    if (verbose) {
        printf("[DEBUG] Added %d names from %s (Total: %d)\n",
               names_from_this_file, filename, *count);
    }

    fclose(fp);
    return entries;
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
            int safe_col = (target_period < max_year) ? target_period : (max_year - 1);

            if (verbose) printf("[DEBUG] Loading: %s (Type: %s, Gender: %s)\n", filename, type, gender);

            // 3. KORJAUS: Tunnistuslogiikka JSON-kenttien perusteella
            if (strcmp(type, "first_name") == 0) {
                if (strcmp(gender, "male") == 0)
                    nd->m1 = load_names(full_csv_path, safe_col, &nd->m1_count, verbose, max_year, NULL);
                else
                    nd->f1 = load_names(full_csv_path, safe_col, &nd->f1_count, verbose, max_year, NULL);
            }
            else if (strcmp(type, "middle_name") == 0) {
                if (strcmp(gender, "male") == 0)
                    nd->m2 = load_names(full_csv_path, safe_col, &nd->m2_count, verbose, max_year, NULL);
                else
                    nd->f2 = load_names(full_csv_path, safe_col, &nd->f2_count, verbose, max_year, NULL);
            }
            else if (strstr(type, "surname") != NULL || strcmp(type, "elite") == 0) {
                    nd->l = load_names(full_csv_path, safe_col, &nd->l_count, verbose, max_year, nd->l);
            }
            else if (strcmp(type, "occupation") == 0) {
                if (strcmp(category, "child") == 0) {
                    if (strcmp(gender, "male") == 0)
                        nd->occupations_child_m = load_names(full_csv_path, safe_col, &nd->occ_cm_count, verbose, max_year, NULL);
                    else
                        nd->occupations_child_f = load_names(full_csv_path, safe_col, &nd->occ_cf_count, verbose, max_year, NULL);
                } else {
                    if (strcmp(gender, "male") == 0)
                        nd->occupations_m = load_names(full_csv_path, safe_col, &nd->occ_m_count, verbose, max_year, NULL);
                    else
                        nd->occupations_f = load_names(full_csv_path, safe_col, &nd->occ_f_count, verbose, max_year, NULL);
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

    // Nyt listoja on yhteensä 9 (4 nimeä, 1 sukunimi, 4 ammattia)
    NameEntry *lists[] = {
        nd->m1, nd->m2, nd->f1, nd->f2, nd->l,
        nd->occupations_m, nd->occupations_f,
        nd->occupations_child_m, nd->occupations_child_f
    };
    int counts[] = {
        nd->m1_count, nd->m2_count, nd->f1_count, nd->f2_count, nd->l_count,
        nd->occ_m_count, nd->occ_f_count,
        nd->occ_cm_count, nd->occ_cf_count
    };

    for (int j = 0; j < 9; j++) {
        if (lists[j]) {
            for (int i = 0; i < counts[j]; i++) {
                free(lists[j][i].name);
            }
            free(lists[j]);
        }
    }
    free(nd);
}
