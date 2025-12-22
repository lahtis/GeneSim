#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "args.h"
#include "loader.h"
#include "generator.h"

int main(int argc, char *argv[]) {
    #ifdef _WIN32
    SetConsoleOutputCP(65001); // UTF-8
    SetConsoleCP(65001);
    #endif

    setlocale(LC_ALL, "fi_FI.UTF-8");
    Args args;

    // 1. Parsitaan komentoriviparametrit
    parse_args(argc, argv, &args);

    // 2. Asetetaan seed
    if (args.seed != 0) {
        srand((unsigned int)args.seed); // Käytetään käyttäjän antamaa lukua
        if (args.verbose) {
            fprintf(stderr, "INFO: Seed set to %u\n", (unsigned int)args.seed);
        }
    } else {
        srand((unsigned int)time(NULL)); // Jos seediä ei annettu (0), käytetään kelloa
    }

    // TÄMÄ PUUTTUU: Tarkistetaan pitääkö näyttää help tai versio
    if (args.help) {
        print_help();
        return 0; // Lopetetaan ohjelma tähän
    }

    if (args.version) {
        print_version();
        return 0; // Lopetetaan ohjelma tähän
    }
    // 2. Ladataan konfiguraatio (HUOM: palauttaa Config-pointterin)
    Config *cfg = load_config("config.txt");
    if (!cfg) {
        fprintf(stderr, "Error: The configuration file could not be loaded.\n");
        return 1;
    }

    // 3. Periodi-logiikka (tarkistetaan ja arvotaan tarvittaessa)
    if (args.period <= 0 || args.period > 7) {
        args.period = (rand() % 7) + 1;
    }
    // Indeksi on 0-6 tiedostoissasi
    int period_idx = args.period - 1;

    // 4. Ladataan kaikki nimitiedostot (Käytetään load_all_data_with_config)
    NameData *data = load_all_data_with_config(cfg, period_idx, args.verbose);
    if (!data) {
        fprintf(stderr, "FATAL: Could not load name data.\n");
        free_config(cfg);
        return 1;
    }

    if (args.verbose) {
    printf("INFO: Data loaded successfully.\n");
    printf("INFO: Men names: %d, Women names: %d, Surnames: %d\n\n",
            data->m1_count, data->f1_count, data->l_count);
}

    // 5. Generointisilmukka
    for (int i = 0; i < args.count; i++) {
        if (args.family_mode) {
            generate_family(&args, cfg, data, stdout);
        } else if (args.couple_mode) {
            generate_couple(&args, cfg, data, stdout);
        } else {
            generate_single(&args, cfg, data, stdout);
        }
    }

    // 6. Puhdistus (Käytetään loader.c:n funktioita)
    free_all_data(data);
    free_config(cfg);

    return 0;
}
