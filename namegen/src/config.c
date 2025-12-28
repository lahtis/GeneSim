#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "config.h"
#include "args.h"

// Apufunktio: etsi polku avaimen perusteella
const char *find_config(ConfigEntry *entries, int n, const char *key) {
    for (int i = 0; i < n; i++) {
        if (strcmp(entries[i].key, key) == 0) return entries[i].value;
    }
    return NULL;
}

void sync_args_to_config(Args *args, Config *cfg) {
    if (!args || !cfg) return;

    // 1. Perusasetukset (vain jos ne on annettu komentorivill‰ eli != 0)
    if (args->count > 0) cfg->count = args->count;

    // Vuosi ja periodi synkronointi
    if (args->period > 0) {
        cfg->year = 1860 + (args->period - 1) * 10;
    } else if (args->year > 0) {
        cfg->year = args->year;
    }

    // 2. Tekniset kytkimet (P‰ivitet‰‰n vain jos poikkeaa oletuksesta)

    // P‰ivitet‰‰n vain jos k‰ytt‰j‰ on antanut --log on tai --log off
    // (Olettaen ett‰ args.log on 1 jos "on" ja 0 jos ei mainittu)
    if (args->log == 1) cfg->logging_enabled = 1;

    // TƒMƒ KORJAA VERBOSE-ONGELMAN:
    if (args->verbose == 1) {
        cfg->verbose = 1;
    }

    // Painotukset (p‰ivitet‰‰n vain jos k‰ytt‰j‰ on asettanut jotain)
    // T‰ss‰ pit‰isi tiet‰‰ onko --no-weights annettu.
    if (args->use_weights == 0) cfg->use_weights = 0;
    else if (args->use_weights == 1) cfg->use_weights = 1;

    // 3. Merkkijonot (data_locale ja output_file)
    // Jos k‰ytt‰j‰ antoi komentorivill‰ uuden tiedostonimen
    if (strlen(args->output_filename) > 0) {
        if (cfg->output_file) free(cfg->output_file);
        cfg->output_file = strdup(args->output_filename);
    }

    // 4. Syvemm‰t generaattorin asetukset (jos lis‰tty configiin)
    if (args->middle_chance >= 0) {
        cfg->middle_chance = args->middle_chance;
    }

    if (args->seed != 0) {
        cfg->deterministic_seed = args->seed;
    }
}

void load_technical_settings(Config *cfg, const char *filename) {
    ConfigEntry tech_entries[50]; // Nostettu m‰‰r‰‰ hieman varmuuden vuoksi
    int count = read_config(filename, tech_entries, 50);

    if (count <= 0) return; // Jos tiedostoa ei ole, k‰ytet‰‰n jo asetettuja oletuksia

    const char *val;

    // 1. Master Configuration -asetukset
    val = find_config(tech_entries, count, "data_locale");
    if (val) {
        if (cfg->locale) free(cfg->locale); // Vapautetaan vanha jos on
        cfg->locale = strdup(val);
    }

    // 2. Simulation initial values
    val = find_config(tech_entries, count, "year");
    if (val) cfg->year = atoi(val);

    // 3. Amount and form of generation
    val = find_config(tech_entries, count, "count");
    if (val) cfg->count = atoi(val);

    val = find_config(tech_entries, count, "output_file");
    if (val && strlen(val) > 0) {
        if (cfg->output_file) free(cfg->output_file);
        cfg->output_file = strdup(val);
    }

    // 4. Technical switches
    val = find_config(tech_entries, count, "log");
    if (val) cfg->logging_enabled = atoi(val);

    val = find_config(tech_entries, count, "use_weights");
    if (val) cfg->use_weights = atoi(val);

    val = find_config(tech_entries, count, "verbose");
    if (val) cfg->verbose = atoi(val);
}

void populate_config(Config *cfg, ConfigEntry *entries, int count) {
    const char *val;
    char path_buf[512]; // <--- TƒMƒ PUUTTUI: Varaa tilaa polun rakentamiseen

    // 1. Maa ja kieli - Standardoidaan fi-FI
    val = find_config(entries, count, "data_locale");
    cfg->locale = strdup(val ? val : "fi-FI");

    // 2. Vuosi ja aikakausi
    val = find_config(entries, count, "YEAR");
    cfg->year = val ? atoi(val) : 1860;

    // Lasketaan dynaamisesti periodi-ID (1-11) CSV-lataajaa varten
    cfg->current_period = ((cfg->year - 1860) / 10) + 1;
    if (cfg->current_period < 1) cfg->current_period = 1;
    if (cfg->current_period > 11) cfg->current_period = 11;

    // 3. Linkkuveitsi-asetukset (Oletusarvot s‰‰tylogiikalle)
    val = find_config(entries, count, "MAX_MIDDLE_NAMES");
    cfg->max_middle_names = val ? atoi(val) : 3;

    val = find_config(entries, count, "MIDDLE_CHANCE");
    cfg->middle_chance = val ? atoi(val) : -1;

    // 4. Rakenna polku Master Configiin locale-kansion perusteella
    // Nyt path_buf on m‰‰ritelty, joten sprintf toimii
    snprintf(path_buf, sizeof(path_buf), "data/%s/namegen_master_config.json", cfg->locale);
    cfg->master_config_path = strdup(path_buf);

    if (cfg->verbose) {
        printf("[DEBUG] Locale: %s\n", cfg->locale);
        printf("[DEBUG] Master Config polku: %s\n", cfg->master_config_path);
    }

    // Alustetaan JSON-juuret nolliksi
    cfg->social_logic_root = NULL;
    cfg->regions_root = NULL;
    cfg->current_period_idx = 0;

    // 5. Elite-avainsanojen pilkkominen (v1.3.2)
    val = find_config(entries, count, "ELITE_KEYWORDS");
    if (val && strlen(val) > 0) {
        char *tmp = strdup(val);
        char *token = strtok(tmp, ",");
        int k_count = 0;

        // Lasketaan m‰‰r‰
        char *count_ptr = strdup(val);
        char *c_token = strtok(count_ptr, ",");
        while(c_token) { k_count++; c_token = strtok(NULL, ","); }
        free(count_ptr);

        cfg->elite_keywords = malloc(k_count * sizeof(char *));
        cfg->elite_key_count = k_count;

        token = strtok(tmp, ",");
        for(int i = 0; i < k_count; i++) {
            if (token) {
                while(isspace((unsigned char)*token)) token++;
                cfg->elite_keywords[i] = strdup(token);
                token = strtok(NULL, ",");
            }
        }
        free(tmp);
    } else {
        cfg->elite_keywords = NULL;
        cfg->elite_key_count = 0;
    }
}

int read_config(const char *filename, ConfigEntry entries[], int max_entries) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Failed to open config file");
        return 0;
    }

    int count = 0;
    char line[512];
    while (fgets(line, sizeof(line), f) && count < max_entries) {
        // Ohitetaan kommentit ja tyhj‰t rivit
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        // Etsit‰‰n yht‰suuruusmerkki
        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0'; // Katkaistaan merkkijono yht‰suuruusmerkin kohdalta
        char *key_ptr = line;
        char *val_ptr = eq + 1;

        // Trimmataan v‰lilyˆnnit avaimen alusta ja lopusta
        while(isspace((unsigned char)*key_ptr)) key_ptr++;
        char *key_end = key_ptr + strlen(key_ptr) - 1;
        while(key_end > key_ptr && isspace((unsigned char)*key_end)) {
            *key_end = '\0';
            key_end--;
        }

        // Trimmataan v‰lilyˆnnit ja rivinvaihdot arvon alusta ja lopusta
        while(isspace((unsigned char)*val_ptr)) val_ptr++;
        char *val_end = val_ptr + strlen(val_ptr) - 1;
        while(val_end > val_ptr && isspace((unsigned char)*val_end)) {
            *val_end = '\0';
            val_end--;
        }

        // Kopioidaan puhdistetut arvot entry-taulukkoon
        strncpy(entries[count].key, key_ptr, sizeof(entries[count].key) - 1);
        strncpy(entries[count].value, val_ptr, sizeof(entries[count].value) - 1);

        count++;
    }

    fclose(f);
    return count;
}

void save_config(const char *filename, Config *cfg) {
    if (cfg->verbose) {
    printf("[DEBUG] Saving config: verbose is %d\n", cfg->verbose);
    }
    if (!cfg) return;

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "[ERROR] Could not open %s for writing.\n", filename);
        return;
    }

    fprintf(fp, "# --- GeneSim NameGen Configuration ---\n\n");
    fprintf(fp, "# Master Configuration\n");
    fprintf(fp, "data_locale=%s\n", cfg->locale ? cfg->locale : "fi-FI");
    fprintf(fp, "master_config=namegen_master_config.json\n\n");
    fprintf(fp, "# Simulation initial values\n");
    fprintf(fp, "default_region=yleinen_suomi\n");
    fprintf(fp, "year=1860\n\n");
    fprintf(fp, "# Amount and form of generation\n");
    fprintf(fp, "count=%d\n", cfg->count);
    fprintf(fp, "output_format=plain\n");
    fprintf(fp, "output_file=\n\n");
    fprintf(fp, "# Technical switches\n");
    fprintf(fp, "# Use CSV weights: 1 = historical popularity, 0 = Uniform distribution (all names equally common)\n");
    fprintf(fp, "use_weights=%d\n", cfg->use_weights);
    fprintf(fp, "log=%d\n", cfg->logging_enabled);
    fprintf(fp, "verbose=%d\n", cfg->verbose);


    if (cfg->output_file) {
        fprintf(fp, "output_file=%s\n", cfg->output_file);
    }

    fclose(fp);

    if (cfg->verbose) {
        printf("[INFO] Settings saved to %s\n", filename);
    }
}
