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
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "loader.h"
#include "config.h"
// #include "args.h"
#include "generator.h"

// --- APUFUNKTIO: Ohjesääntö ---
void print_historical_list(cJSON *root, const char *lang) {
    cJSON *seasons = cJSON_GetObjectItem(root, "seasons");
    if (!cJSON_IsArray(seasons)) {
        printf("VIRHE: 'seasons'-listaa ei loytynyt JSON-datasta.\n");
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
    int list_periods = 0;
    char *locale_pref = "fi"; // Oletuskieli

    // 1. Yksinkertainen parseri suoraan mainissa (ei tarvitse args.c:tä)
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--lp") == 0) {
            list_periods = 1;
            if (i + 1 < argc && argv[i+1][0] != '-') {
                locale_pref = argv[i+1];
                i++;
            }
        }
    }

    // 2. Ladataan config.conf
    Config *cfg = load_config("config.conf");
    if (!cfg) {
        fprintf(stderr, "ERROR: Could not load config.conf\n");
        return 1;
    }

    // 3. Toiminta, jos --lp annettiin
    if (list_periods == 1) {
        char full_master_path[512];
        snprintf(full_master_path, sizeof(full_master_path), "data/%s/%s",
                 cfg->locale, cfg->master_config_path);

        cJSON *root = load_master_config(full_master_path);
        if (root) {
            print_historical_list(root, locale_pref);
            cJSON_Delete(root);
        } else {
            printf("ERROR: JSON not found at %s\n", full_master_path);
        }

        free_config(cfg);
        return 0; // Lopeta tähän
    }

    // Normaali generointi alkaa tasta, jos ei lp-lippua
    printf("Starting name generation for year %d...\n", cfg->year);

    free_config(cfg);
    return 0;
}
