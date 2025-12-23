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
#include <locale.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "config.h"
#include "args.h"
#include "loader.h"
#include "generator.h"

void print_ohjesaanto(const void *cfg_ptr, const Args *args) {
    const Config *cfg = (const Config *)cfg_ptr;
    if (!cfg || !cfg->firstMDataPaths) return;

    char path[512];
    strncpy(path, cfg->firstMDataPaths, sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0'; // Varmistetaan nollapääte

    // Määritellään last_slash tässä!
    char *last_slash = strrchr(path, '/');
    if (!last_slash) last_slash = strrchr(path, '\\');

    if (!last_slash) {
        printf("[!] Polkuvirhe ohjesaantoa etsiessa.\n");
        return;
    }

    FILE *f = NULL;
    const char *target_file = (args->lang_en) ? "GUIDELINES.txt" : "OHJESAANTO.txt";

    // Asetetaan valittu tiedosto polkuun
    strcpy(last_slash + 1, target_file);
    f = fopen(path, "r");

    // Fallback: Jos valittua kieltä ei ole, kokeillaan toista
    if (f == NULL) {
        const char *alt_file = (args->lang_en) ? "OHJESAANTO.txt" : "GUIDELINES.txt";
        strcpy(last_slash + 1, alt_file);
        f = fopen(path, "r");
        if (f) target_file = alt_file;
    }

    if (f == NULL) {
        printf("\n[!] Error: Documentation not found (%s).\n", target_file);
        return;
    }

    printf("\n==================================================\n");
    printf("           DOCUMENTATION / OHJESAANTO           \n");
    printf("           Language: %-26s \n", (strstr(target_file, "GUIDE") ? "English" : "Finnish"));
    printf("==================================================\n");

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        while (*p && (unsigned char)*p <= 32) p++;
        if (*p == '\0' || *p == '#') continue;
        printf(" %s", p);
    }
    printf("\n==================================================\n\n");

    fclose(f);
}

int main(int argc, char *argv[]) {
    // 1. WINDOWS & UTF-8 TUKI
    // Tämä varmistaa, että skandit (ä, ö) näkyvät oikein komentorivillä
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    #endif

    setlocale(LC_ALL, "fi_FI.UTF-8");

    Args args;
    // Alustetaan args oletusarvoilla (estää satunnaiset luvut muistissa)
    memset(&args, 0, sizeof(Args));
    args.count = 1; // Oletuksena yksi nimi

    // 2. PARSE ARGS
    parse_args(argc, argv, &args);

    // Help ja Version tarkistukset
    if (args.help) {
        print_help();
        return 0;
    }
    if (args.version) {
        print_version();
        return 0;
    }

    // 3. SEED (SATUNNAISUUS)
    if (args.seed != 0) {
        srand((unsigned int)args.seed);
        if (args.verbose) {
            fprintf(stderr, "[INFO] Seed set to: %u\n", (unsigned int)args.seed);
        }
    } else {
        srand((unsigned int)time(NULL));
    }

    // 4. KONFIGURAATION LATAUS
    // Varmista että tiedosto on olemassa (config.txt tai config.json)
    Config *cfg = load_config("config.txt");
    if (!cfg) {
        fprintf(stderr, "ERROR: Configuration file 'config.txt' not found.\n");
        return 1;
    }

    if (args.list_periods) {
        print_ohjesaanto(cfg, &args);
        free_config(cfg);
        return 0; // Lopetetaan tähän, kuten "ohjesääntö" vaatii
    }

    // 5. PERIODI-LOGIIKKA JA VALIDIOINTI
    if (args.period <= 0) {
    args.period = (rand() % 7) + 1; // Arvotaan sarake 1-7 väliltä
    }

    // Tarkistetaan vain, että tiedosto on olemassa (ei kansioita)
    if (cfg->firstMDataPaths) {
        FILE *test_f = fopen(cfg->firstMDataPaths, "r");
        if (test_f == NULL) {
            printf("\n[HUOMIO] Nimitiedostoa ei loytynyt: %s\n", cfg->firstMDataPaths);
            return 1; // Kriittinen virhe, jos tiedostoa ei ole
        } else {
            fclose(test_f);
            if (args.verbose) printf("[INFO] Kaytetaan saraketta (period): %d\n", args.period);
        }
    }

    int period_idx = args.period; // Käytetään suoraan numerona, koska koodisi vertaa current_col == target_period

    // 6. DATAN LATAUS
    NameData *data = load_all_data_with_config(cfg, period_idx, args.verbose);
    if (!data) {
        fprintf(stderr, "FATAL: Loading name data failed.\n");
        free_config(cfg);
        return 1;
    }

    if (args.verbose) {
        fprintf(stderr, "[INFO] Data loaded for the era %d\n", args.period);
        fprintf(stderr, "[INFO] First names (M/F): %d/%d, Last names: %d\n\n",
                data->m1_count, data->f1_count, data->l_count);
    }

    // 7. GENEROINTI
    // Jos output on JSON, aloitetaan taulukko
    if (args.output_mode == OUTPUT_JSON) {
        printf("[\n");
    }

    for (int i = 0; i < args.count; i++) {
        if (args.family_mode) {
            generate_family(&args, cfg, data, stdout);
        } else if (args.couple_mode) {
            generate_couple(&args, cfg, data, stdout);
        } else {
            generate_single(&args, cfg, data, stdout);
        }

        // Jos on useampi generointi ja JSON, lisätään pilkku väliin
        // Huom: Tämä vaatii, että generaattorifunktiot osaavat käsitellä sisäiset pilkut
        if (args.output_mode == OUTPUT_JSON && i < args.count - 1) {
            printf(",\n");
        }
    }

    // Jos output on JSON, suljetaan taulukko
    if (args.output_mode == OUTPUT_JSON) {
        printf("\n]\n");
    }

    // 8. PUHDISTUS
    free_all_data(data);
    free_config(cfg);

    return 0;
}
