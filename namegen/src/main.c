/**
* @file namegen.c
* @brief NameGen a comprehensive lineage and family relationship simulator.

namegen - A comprehensive lineage and family relationship simulator.
Developed pure C.

Copyright (C) 2025 Tuomas Lähteenmäki lahtis@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "cJSON.h"
#include "config.h"
#include "loader.h"
#include <locale.h>
#include "args.h"
#include "generator.h"
#include "output.h"

#ifdef _WIN32
#include <windows.h>
#endif

Args args;                  // Tämä kertoo, että args on määritelty main.c:ssä
unsigned int actual_seed;   // Sama siemenluvulle
Config *cfg = NULL;

// --- APUFUNKTIO: Ohjesääntö ---
void print_historical_list(cJSON *root, const char *lang) {
    cJSON *seasons = cJSON_GetObjectItem(root, "seasons");
    if (!cJSON_IsArray(seasons)) {
        printf("ERROR: 'seasons' list not found in JSON data.\n");
        return;
    }

    int use_en = (lang && strcmp(lang, "en") == 0);

    printf("\n================================================================================\n");
    printf("   GENESIM - HISTORICAL PERIODS (%s)\n", use_en ? "EN" : "FI");
    printf("--------------------------------------------------------------------------------\n");
    printf("%-3s | %-12s | %-45s\n", "ID", use_en ? "YEARS" : "VUODET", use_en ? "DESCRIPTION" : "KUVAUS");
    printf("--------------------------------------------------------------------------------\n");

    cJSON *s = NULL;
    cJSON_ArrayForEach(s, seasons) {
        int id = cJSON_GetObjectItem(s, "id")->valueint;
        const char *y = cJSON_GetObjectItem(s, "years")->valuestring;
        const char *n = cJSON_GetObjectItem(s, use_en ? "note_en" : "note_fi")->valuestring;
        printf("%-3d | %-12s | %-45s\n", id, y, n);
    }
    printf("================================================================================\n\n");
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, ".UTF8");
    // 1. Asetetaan konsolin koodisivu UTF-8:ksi (65001)
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    #endif

    // 1. ASETETAAN SEED
    actual_seed = args.seed ? args.seed : (unsigned int)time(NULL);
    srand(actual_seed);

    // 2. ASETETAAN KOVAT KAULAAN
    Config *cfg = calloc(1, sizeof(Config));
    // Asetetaan "kovat" oletukset ensin
    cfg->year = 1860;
    cfg->count = 1;
    cfg->logging_enabled = 1;
    cfg->verbose = 1;

    // Ladataan tiedostosta (ylikirjoittaa oletukset)
    load_technical_settings(cfg, "config.conf");

    // Luetaan argumentit (ylikirjoittaa tiedostoasetukset)
    parse_args(argc, argv, &args);

    // 6. TARKISTETAAN APUKOMENNOT
    // 2. DEBUG: Tarkistetaan mitä parse_args oikeasti teki

    if (args.help) { print_help(); free_config(cfg); return 0; }
    if (args.version) { print_version(); free_config(cfg); return 0; }


    if (cfg->verbose) {
    printf("[DEBUG] Args structure (CLI): verbose = %d\n", args.verbose);
    }
    sync_args_to_config(&args, cfg); // Päivittää cfg-rakenteen args-arvoilla
    if (cfg->verbose) {
    printf("[DEBUG] Config structure after synchronization: verbose = %d\n", cfg->verbose);
    }

    // --- SIIRRETTY SEEDIN ASETUS TÄHÄN ---
    // Jos args.seed on annettu, käytetään sitä, muuten kelloa
    actual_seed = args.seed ? (unsigned int)args.seed : (unsigned int)time(NULL);
    srand(actual_seed);

    if (cfg->verbose) {
        printf("[DEBUG] Seed initialized to: %u\n", actual_seed);
    }

    // Tallennetaan uusi tila
    save_config("config.conf", cfg);

    // 5. NYT synkronoidaan vuosi lokitusta varten
    if (args.year == 0 && cfg != NULL) {
    args.year = cfg->year;
    }

    // 7. PERIODILISTAUS
    if (args.list_periods) {
    char *json_raw = read_file_to_string("data/fi-FI/namegen_master_config.json");
    if (json_raw) {
        cJSON *temp_root = cJSON_Parse(json_raw);
        if (temp_root) {
            // MUUTOS TÄSSÄ: Muunnetaan int (0/1) merkkijonoksi ("fi"/"en")
            print_historical_list(temp_root, args.lang_en ? "en" : "fi");
            cJSON_Delete(temp_root);
        }
        free(json_raw);
    }
    free_config(cfg);
    return 0;
    }

    if (cfg->verbose) printf("[INFO] The program is loading data into memory.\n");

    // 9. Lasketaan kausi-ID (1-11) lataajalle
    int target_period = ((cfg->year - 1860) / 10) + 1;
    if (target_period < 1) target_period = 1;
    if (target_period > 11) target_period = 11;

    // Jos polkua ei ole asetettu, asetetaan oletus ennen latausta
    if (!cfg->master_config_path) {
        cfg->master_config_path = strdup("namegen_master_config.json");
    }

    if (cfg->verbose) {
        printf("\n[DEBUG-PRE-LOAD] --- INITIALIZING DATA LOAD ---\n");
        printf("[DEBUG-PRE-LOAD] Data Locale: %s\n", cfg->locale ? cfg->locale : "NULL");
        printf("[DEBUG-PRE-LOAD] Target Seed: \n");
        printf("[DEBUG-PRE-LOAD] Target Year: %d\n", cfg->year);
        printf("[DEBUG-PRE-LOAD] Target Period ID: %d\n", target_period);
        // Huom: Jos populate_config rakentaa polun, tulostetaan se:
        printf("[DEBUG-PRE-LOAD] Master Config Path: %s\n", cfg->master_config_path ? cfg->master_config_path : "NOT SET");
        printf("----------------------------------------------\n\n");
    }

    // 10. LADATAAN DATA (Dynaaminen linkkuveitsi-lataaja)
    NameData *nd = load_all_data_with_config(cfg, target_period, cfg->verbose);

    if (!nd) {
        fprintf(stderr, "[FATAL] Failed to load data. Check the data folder.\n");
        // Jos haluat vielä tarkemman syyn fataliin:
        if (!cfg->locale) fprintf(stderr, "[HINT] Config locale is NULL!\n");

        free_config(cfg);
        return 1;
    }

    if (cfg->verbose) {
        printf("\n[DEBUG-FLOW] --- STARTING GENERATION ---\n");
        printf("[DEBUG-FLOW] Requested Year: %d\n", cfg->year);
        printf("[DEBUG-FLOW] Calculated Period ID: %d\n", target_period);
    }

    // 11. GENEROINTI
    if (cfg->verbose) printf("[INFO] Generating %d items...\n\n", cfg->count);

    FILE *output_fp = stdout;

    // Tarkistetaan onko Config-rakenteessa määritelty tiedostopolku
    if (cfg->output_file && strlen(cfg->output_file) > 0) {
        output_fp = fopen(cfg->output_file, "w");
        if (!output_fp) {
            fprintf(stderr, "[ERROR] Could not open file %s. Using command window.\n", cfg->output_file);
            output_fp = stdout;
        }
    }

    // 12. JSON-aloitus
    if (args.output_mode == OUTPUT_JSON) {
        fprintf(output_fp, "[\n");
    }

    for (int i = 0; i < cfg->count; i++) {
        if (i > 0 && args.output_mode == OUTPUT_JSON) {
            fprintf(output_fp, ",\n");
        }

        if (cfg->verbose) printf("[DEBUG] Generating target %d...\n\n", i+1);

        if (args.family_mode) {
            generate_family(&args, cfg, nd, output_fp);
        } else if (args.couple_mode) {
            generate_couple(&args, cfg, nd, output_fp);
        } else {
            generate_single(&args, cfg, nd, output_fp);
        }

        // Pakotetaan tuloste ulos jokaisen nimen jälkeen
        fflush(output_fp);
    }

    // 13. JSON-lopetus
    if (args.output_mode == OUTPUT_JSON) {
        fprintf(output_fp, "\n]\n");
    }

    // 14. Suljetaan tiedosto jos se avattiin
    if (output_fp != stdout) fclose(output_fp);

    // 15. SIIVOUS
       free_all_data(nd); // Käytä tätä jos sinulla on NameData-vapauttaja
       free_config(cfg);

    // printf("[INFO] The program executed successfully.\n");
    return 0;
}
