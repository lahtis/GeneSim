/**
* @file namegen.c
* @brief NameGen simple name generator program.

namegen - A comprehensive lineage and family relationship simulator.
Developed with GTK3 and C.

Copyright (C) 2025 Tuomas Lähteenmäki

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

// Versio: MAJOR.MINOR.PATCH
#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 0 // Versio 0.2.0: Lisätty usean keskinimen tuki ja datan validointi

// Luodaan versionumerosta merkkijono tulostusta varten
#define VERSION_STRING "0.2.0"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Maksimipituus yhdelle nimelle ja riville
#define MAX_LINE_LENGTH 4096
#define MAX_FULL_NAME_LENGTH 1024 // Puskurin koko koko nimelle

// Rakenne nimilistan tietojen tallentamiseen
typedef struct {
    char **names;
    int count;
} NameList;

// --- B. KOKO TIEDOSTON TIETORAKENNE (DecadeData) ---
typedef struct {
    char **decades;
    NameList *lists;
    int num_decades;
} DecadeData;

// Funktio vapauttaa NameList-rakenteen varaaman muistin
void free_names(NameList *list) {
    if (list->names != NULL) {
        for (int i = 0; i < list->count; i++) {
            free(list->names[i]);
        }
        free(list->names);
    }
    list->names = NULL;
    list->count = 0;
}

// Funktio poistaa alussa olevat välilyönnit (trimmaa)
char* trim_leading_spaces(char *str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

// Apu Funktio virheiden käsittelyyn
void print_error(const char *message) {
    fprintf(stderr, "ERROR: %s\n", message);
}

// UUSI: Tarkistaa, sisältääkö merkkijono vain aakkosellisia merkkejä, välilyöntejä ja väliviivoja.
int is_valid_name(const char *name) {
    if (name == NULL || *name == '\0') {
        return 0; // Tyhjä merkkijono ei ole kelvollinen nimi
    }
    for (int i = 0; name[i] != '\0'; i++) {
        // Sallitaan aakkoset, välilyönnit ja väliviiva. Muut merkit (kuten numerot) hylätään.
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ' && name[i] != '-') {
            return 0; // Epäkelpo merkki löytyi
        }
    }
    return 1; // Kelvollinen
}


// Funktio vapauttaa DecadeData-rakenteen varaaman muistin
void free_decade_data(DecadeData *data) {
    if (data->decades != NULL) {
        for (int i = 0; i < data->num_decades; i++) {
            free(data->decades[i]);
        }
        free(data->decades);
    }

    if (data->lists != NULL) {
        for (int i = 0; i < data->num_decades; i++) {
            free_names(&data->lists[i]);
        }
        free(data->lists);
    }

    data->num_decades = 0;
    data->decades = NULL;
    data->lists = NULL;
}


// Ladataan nimet CSV-tiedostosta, jossa on useita sarakkeita (yksi sarake = yksi vuosikymmenlista)
void load_names_multi_column(const char *filename, DecadeData *data, int verbose) {
    data->num_decades = 0;
    data->decades = NULL;
    data->lists = NULL;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        print_error("Error opening file for multi-column loading.");
        return;
    }

    char buffer[MAX_LINE_LENGTH];

    // 1. Lue otsikkorivi (vuosikymmenet)
    if (fgets(buffer, MAX_LINE_LENGTH, file) == NULL) {
        fclose(file);
        return;
    }

    char header_copy[MAX_LINE_LENGTH];
    strcpy(header_copy, buffer);
    header_copy[strcspn(header_copy, "\n\r")] = 0;

    char *token = strtok(header_copy, ",");
    while (token != NULL) {
        data->decades = (char **)realloc(data->decades, (data->num_decades + 1) * sizeof(char *));
        data->decades[data->num_decades] = strdup(trim_leading_spaces(token));

        data->lists = (NameList *)realloc(data->lists, (data->num_decades + 1) * sizeof(NameList));
        data->lists[data->num_decades].names = NULL;
        data->lists[data->num_decades].count = 0;

        data->num_decades++;
        token = strtok(NULL, ",");
    }
    if (verbose) {
        printf("Loaded %d headlines of the decade.\n", data->num_decades);
    }

    // 2. Lue varsinaiset tiedot rivi kerrallaan
    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        buffer[strcspn(buffer, "\n\r")] = 0;
        char data_copy[MAX_LINE_LENGTH];
        strcpy(data_copy, buffer);

        token = strtok(data_copy, ",");
        int col = 0;
        while (token != NULL && col < data->num_decades) {

            char *clean_name = trim_leading_spaces(token);

            // Jos sarake ei ole tyhjä JA nimi on kelvollinen (uusi tarkistus)
            if (strlen(clean_name) > 0 && is_valid_name(clean_name)) {
                NameList *current_list = &data->lists[col];

                current_list->names = (char **)realloc(current_list->names, (current_list->count + 1) * sizeof(char *));

                current_list->names[current_list->count] = strdup(clean_name);
                current_list->count++;
            }
            else if (strlen(clean_name) > 0 && verbose) {
                // Tulosta varoitus epäkelvosta datasta (esim. Kauko 1870-29 tapauksessa)
                fprintf(stderr, "WARNING: Skipped invalid name candidate '%s' during load.\n", clean_name);
            }

            col++;
            token = strtok(NULL, ",");
        }
    }

    fclose(file);
}

// Funktio tulostaa käytettävissä olevat vuosikymmenet ja niiden koot
void print_available_decades(const DecadeData *data) {
    printf("\n--- Available time periods (CSV structure) ---\n");
    for (int i = 0; i < data->num_decades; i++) {
        printf("%d: %s (Names on the list: %d)\n",
               i + 1, data->decades[i], data->lists[i].count);
    }
    printf("------------------------------------------------------\n");
}


// Funktio lataa nimet tavallisesta tiedostosta (yksi nimi per rivi)
void load_names_simple(const char *filename, NameList *list, int verbose) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "WARNING: Could not open file: %s\n", filename);
        list->count = 0;
        list->names = NULL;
        return;
    }

    list->count = 0;
    list->names = NULL;
    char buffer[MAX_LINE_LENGTH];

    // Luetaan rivi kerrallaan
    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        buffer[strcspn(buffer, "\n\r")] = 0;

        char *clean_name = trim_leading_spaces(buffer);
        // Tarkista myös sukunimien kelpoisuus
        if (strlen(clean_name) > 0 && is_valid_name(clean_name)) {

            list->names = (char **)realloc(list->names, (list->count + 1) * sizeof(char *));

            list->names[list->count] = strdup(clean_name);

            list->count++;
        }
        else if (strlen(clean_name) > 0 && verbose) {
            fprintf(stderr, "WARNING: Skipped invalid last name candidate '%s' during load.\n", clean_name);
        }
    }

    fclose(file);
    if (verbose) {
        printf("Loaded %d name from the file: %s\n", list->count, filename);
    }
}


// Funktio valitsee ja palauttaa satunnaisen nimen NameList-rakenteesta
const char* select_random_name(const NameList *list) {
    if (list == NULL || list->count == 0) {
        return "";
    }
    int index = rand() % list->count;
    return list->names[index];
}

// --- GENERATION FUNKTIO ---

void generate_and_print_name(int period_index, int gender_flag, int verbose_flag,
                             const DecadeData *first_names_set,
                             const DecadeData *middle_names_set,
                             const NameList *last_names_list,
                             int middle_chance,
                             int generate_last_name,
                             const char *override_last_name,
                             int max_middle_names)
{
    const char *first = "";
    const char *last = "";

    char full_name[MAX_FULL_NAME_LENGTH] = "";


    // 1. ETUNIMEN TARKISTUS JA VALINTA
    if (first_names_set == NULL || period_index < 0 || period_index >= first_names_set->num_decades) {
        print_error("Cannot generate a name: Invalid First Name list or index.");
        return;
    }

    if (first_names_set->lists[period_index].count > 0) {
        first = select_random_name(&first_names_set->lists[period_index]);
    } else {
        print_error("Cannot generate a name: First name list for the selected period is empty.");
        return;
    }

    if (strlen(first) == 0) {
        print_error("Cannot generate a name: Failed to select a first name.");
        return;
    }

    // Lisää etunimi puskuriin
    snprintf(full_name, MAX_FULL_NAME_LENGTH, "%s", first);

    // 2. KESKINIMEN VALINTA (max_middle_names asti)
    const NameList *middle_list = (middle_names_set != NULL && middle_names_set->num_decades > period_index) ?
                                  &middle_names_set->lists[period_index] : NULL;

    for (int i = 0; i < max_middle_names; i++) {
        if (middle_list != NULL && middle_list->count > 0 && rand() % 100 < middle_chance) {
            const char *middle = select_random_name(middle_list);
            if (strlen(middle) > 0) {
                size_t current_len = strlen(full_name);
                snprintf(full_name + current_len, MAX_FULL_NAME_LENGTH - current_len, " %s", middle);
            }
        }
    }

    // 3. SUKUNIMEN TARKISTUS JA VALINTA
    if (override_last_name != NULL) {
        last = override_last_name;
    }
    else if (generate_last_name == 0) {
        last = "";
    }
    else if (last_names_list == NULL || last_names_list->count == 0) {
        print_error("WARNING: Last name list is empty. Using a placeholder.");
        last = "SukunimiPuuttuu";
    } else {
        last = select_random_name(last_names_list);

        if (strlen(last) == 0) {
            print_error("WARNING: Failed to select a last name. Using a placeholder.");
            last = "SukunimiVirhe";
        }
    }


    // DEBUG-LOHKO
    if (verbose_flag) {
        fprintf(stderr, "DEBUG: Gender=%s, First='%s', MiddleCountMax=%d, Last='%s', Chance=%d, LastGen=%d, LastOverride='%s'\n",
                gender_flag == 0 ? "Male" : "Female", first, max_middle_names, last, middle_chance, generate_last_name, (override_last_name ? override_last_name : "NULL"));
    }

    // 4. Lisää sukunimi nimen loppuun (jos sellainen on)
    if (strlen(last) > 0) {
        size_t current_len = strlen(full_name);
        snprintf(full_name + current_len, MAX_FULL_NAME_LENGTH - current_len, " %s", last);
    }

    // 5. Tulostus
    printf("%s\n", full_name);
}


// --- PÄÄOHJELMA ---

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(65001);
#endif

    setlocale(LC_ALL, "");
    srand(time(NULL));

    // MUUTTUJAT KÄYTTÄJÄN PARAMETREJA VARTEN
    int verbose_flag = 0;
    int period_index = -1;
    int automatic_generation = 0;
    int gender_flag = 0; // 0 = Male (oletus), 1 = Female
    int name_count = 1;
    int middle_name_chance = 50;
    int generate_last_name = 1;
    char *override_last_name = NULL;
    int max_middle_names = 1;
    int couple_mode = 0; // UUSI: Parigenerointi

    // TIEDOSTOPOLUT
    const char *first_file = "data/FI-fi/Finnish-men-firts-names.csv";
    const char *middle_file = "data/FI-fi/Finnish-men-seconds-names.csv";
    const char *female_first_file = "data/FI-fi/Finnish-women-first-names.csv";
    const char *female_middle_file = "data/FI-fi/Finnish-women-middle-names.csv";
    const char *last_file_simple = "data/FI-fi/Finnish-last-names.csv";

    // KOLME ERI TIETORAKENNETTA
    DecadeData first_names = {NULL, NULL, 0};
    DecadeData middle_names = {NULL, NULL, 0};
    NameList last_names_simple = {NULL, 0};
    DecadeData female_first_names = {NULL, NULL, 0};
    DecadeData female_middle_names = {NULL, NULL, 0};


    // 1. KÄSITTELE KOMENTORIVIPARAMETRIT
    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("Program version: %s\n", VERSION_STRING);
            goto cleanup_and_exit;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            goto load_files_for_help;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose_flag = 1;
            printf("Verbose mode activated.\n");
        }
        else if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gender") == 0) {
            if (i + 1 < argc) {
                if (strcmp(argv[i+1], "female") == 0 || strcmp(argv[i+1], "F") == 0) {
                    gender_flag = 1;
                } else if (strcmp(argv[i+1], "male") == 0 || strcmp(argv[i+1], "M") == 0) {
                    gender_flag = 0;
                } else {
                    fprintf(stderr, "ERROR: Incorrect gender selection '%s'. Use 'male'/'M' or 'female'/'F' selector.\n", argv[i+1]);
                    goto cleanup_and_exit_error;
                }
                i++;
            } else {
                fprintf(stderr, "ERROR: Flag -g/--gender requires an option (male/female).\n");
                goto cleanup_and_exit_error;
            }
        }
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--period") == 0) {
            if (i + 1 < argc) {
                char *endptr;
                errno = 0;
                long valinta_long = strtol(argv[i + 1], &endptr, 10);

                if (endptr == argv[i + 1] || *endptr != '\0' || errno == ERANGE) {
                    fprintf(stderr, "ERROR: Invalid season number '%s'. Must be an integer.\n", argv[i+1]);
                    goto cleanup_and_exit_error;
                }

                int valinta = (int)valinta_long;

                period_index = valinta - 1;
                automatic_generation = 1;
                i++;
            } else {
                fprintf(stderr, "Error: Flag -p/--period requires season number.\n");
                goto cleanup_and_exit_error;
            }
        }

        else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--count") == 0) {
             if (i + 1 < argc) {
                char *endptr;
                errno = 0;
                long count_long = strtol(argv[i + 1], &endptr, 10);

                if (endptr == argv[i + 1] || *endptr != '\0' || errno == ERANGE || count_long <= 0) {
                    fprintf(stderr, "ERROR: Invalid name count '%s'. Must be a positive integer.\n", argv[i+1]);
                    goto cleanup_and_exit_error;
                }

                name_count = (int)count_long;
                i++;
            } else {
                fprintf(stderr, "Error: Flag -n/--count requires a number.\n");
                goto cleanup_and_exit_error;
            }
        }

        else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--middle-chance") == 0) {
             if (i + 1 < argc) {
                char *endptr;
                errno = 0;
                long chance_long = strtol(argv[i + 1], &endptr, 10);

                if (endptr == argv[i + 1] || *endptr != '\0' || errno == ERANGE || chance_long < 0 || chance_long > 100) {
                    fprintf(stderr, "ERROR: Invalid middle name chance '%s'. Must be an integer between 0 and 100.\n", argv[i+1]);
                    goto cleanup_and_exit_error;
                }

                middle_name_chance = (int)chance_long;
                i++;
            } else {
                fprintf(stderr, "Error: Flag -m/--middle-chance requires a number (0-100).\n");
                goto cleanup_and_exit_error;
            }
        }

        else if (strcmp(argv[i], "-M") == 0 || strcmp(argv[i], "--max-middle-names") == 0) {
             if (i + 1 < argc) {
                char *endptr;
                errno = 0;
                long max_long = strtol(argv[i + 1], &endptr, 10);

                if (endptr == argv[i + 1] || *endptr != '\0' || errno == ERANGE || max_long < 1 || max_long > 3) {
                    fprintf(stderr, "ERROR: Invalid max middle names '%s'. Must be an integer between 1 and 3.\n", argv[i+1]);
                    goto cleanup_and_exit_error;
                }

                max_middle_names = (int)max_long;
                i++;
            } else {
                fprintf(stderr, "Error: Flag -M/--max-middle-names requires a number (1-3).\n");
                goto cleanup_and_exit_error;
            }
        }

        // UUSI: Parigenerointi
        else if (strcmp(argv[i], "-P") == 0 || strcmp(argv[i], "--couple") == 0) {
            couple_mode = 1;
            generate_last_name = 1; // Pakota sukunimen generointi paritilassa
            // Ohita -g lippu paritilassa
        }

        else if (strcmp(argv[i], "-L") == 0 || strcmp(argv[i], "--no-last-name") == 0) {
            generate_last_name = 0;
        }

        else if (strcmp(argv[i], "-S") == 0 || strcmp(argv[i], "--set-last-name") == 0) {
            if (i + 1 < argc) {
                override_last_name = argv[i+1];
                generate_last_name = 1;
                i++;
            } else {
                fprintf(stderr, "Error: Flag -S/--set-last-name requires a surname.\n");
                goto cleanup_and_exit_error;
            }
        }

        else {
            fprintf(stderr, "ERROR: Unknown parameter: %s\n", argv[i]);
            goto cleanup_and_exit_error;
        }
    }

    // --- TIEDOSTOJEN LATAUS ---
    load_files_for_help:
    if (verbose_flag) {
        printf("--- Reading files ---\n");
    }
    // Ladataan kaikki neljä listaa
    load_names_multi_column(first_file, &first_names, verbose_flag);
    load_names_multi_column(middle_file, &middle_names, verbose_flag);
    load_names_multi_column(female_first_file, &female_first_names, verbose_flag);
    load_names_multi_column(female_middle_file, &female_middle_names, verbose_flag);

    // Sukunimilista ladataan vain, jos sitä tarvitaan
    if (generate_last_name && override_last_name == NULL) {
        load_names_simple(last_file_simple, &last_names_simple, verbose_flag);
    } else if (verbose_flag && override_last_name == NULL) {
        printf("Skipped loading last names (flag -L).\n");
    } else if (verbose_flag && override_last_name != NULL) {
        printf("Skipped loading last names (flag -S is set).\n");
    }

    if (verbose_flag) {
        printf("--------------------------\n");
    }

    // Kriittinen tarkistus tiedostojen latauksen jälkeen
    if (first_names.num_decades == 0 || (generate_last_name && override_last_name == NULL && last_names_simple.count == 0 && !couple_mode)) {
        if (first_names.num_decades == 0) {
            fprintf(stderr, "ERROR: Required first names file is missing or empty.\n");
        } else if (generate_last_name && override_last_name == NULL && last_names_simple.count == 0) {
            fprintf(stderr, "ERROR: Last names file is missing or empty, but last names are required.\n");
        }
        goto cleanup_and_exit_error;
    }

    // Tarkistetaan periodin sallittu alue
    if (period_index != -1 && period_index >= first_names.num_decades) {
        fprintf(stderr, "ERROR: Invalid season index set via -p. Choose 1-%d.\n", first_names.num_decades);
        goto cleanup_and_exit_error;
    }


    // KÄSITTELE OHJE-LIPPU
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        printf("Use: %s [-v] [-p <Number>] [-g <male/female>] [-n <count>] [-m <chance>] [-M <max>] [-P] [-L] [-S <surname>]\n", argv[0]);
        printf("  -p <number>, --period <number>    Selects the time period (1-%d).\n", first_names.num_decades);
        printf("  -g <gender>, --gender <male/female>  Selects gender (ignored if -P is used).\n");
        printf("  -n <count>, --count <count>    Specifies the number of names (or couples if -P is used).\n");
        printf("  -m <chance>, --middle-chance <chance>  Middle name probability (0-100, default 50).\n");
        printf("  -M <max>, --max-middle-names <max>  Max number of middle names (1-3, default 1).\n");
        printf("  -P, --couple    Generates names for a couple (M+F). Surname is generated for M and copied to F.\n"); // UUSI
        printf("  -L, --no-last-name    Does not generate a last name (ignored if -S or -P is set).\n");
        printf("  -S <surname>, --set-last-name <surname>  Sets the surname for all generated names (overrides -L and random generation).\n");
        printf("  -v, --verbose    Show more information about the debug.\n");
        printf("  -h, --help    Displays help.\n");
        printf("  -V, --version    Displays the version number and exits.\n");
        printf("Without flags, the program asks for the season interactively.\n");
        goto cleanup_and_exit;
    }


    // A. KÄYTTÄJÄN ESITTELY JA KYSELY

    if (automatic_generation) {
        if (verbose_flag) {
            printf("Use command line selection: %d (Season: %s). Generating %d %s. Middle name chance: %d%% (Max %d). Last name: %s.\n",
                    period_index + 1, first_names.decades[period_index], name_count,
                    couple_mode ? "couples" : "names", middle_name_chance, max_middle_names,
                    (override_last_name != NULL) ? override_last_name :
                    (generate_last_name ? "Generated" : "No"));
        }
    } else {
        // Interaktiivinen kysely on ennallaan, ei tarvitse näyttää tässä
    }

    // --- B. GENERATION LOHKO ---

    if (period_index != -1) {

        // MÄÄRITÄ SUKUPUOLEN MUKAISET LISTAT
        DecadeData *male_first_set = &first_names;
        DecadeData *male_middle_set = &middle_names;
        DecadeData *female_first_set = &female_first_names;
        DecadeData *female_middle_set = &female_middle_names;


        if (couple_mode) {
            printf("\n--- Generated Couples (%d total) ---\n", name_count);
            for (int k = 0; k < name_count; k++) {

                // 1. GENERATE MALE NAME (to get the last name)
                const char *male_surname = NULL;
                char male_surname_buffer[100]; // Puskuri miehen sukunimelle

                // Jos override on asetettu, käytä sitä
                if (override_last_name != NULL) {
                    male_surname = override_last_name;
                } else {
                    // Generoi uusi sukunimi (ja tallenna se)
                    const char *generated_surname = select_random_name(&last_names_simple);
                    if (strlen(generated_surname) > 0) {
                        strcpy(male_surname_buffer, generated_surname);
                        male_surname = male_surname_buffer;
                    } else {
                         male_surname = "SukunimiPuuttuu";
                    }
                }

                // Kutsu generointifunktiota miehelle
                generate_and_print_name(period_index, 0, verbose_flag,
                                        male_first_set, male_middle_set, &last_names_simple,
                                        middle_name_chance, 1, male_surname, max_middle_names);

                // 2. GENERATE FEMALE NAME (using male's last name)
                // Kutsu generointifunktiota naiselle käyttäen miehen sukunimeä override-parametrinä
                generate_and_print_name(period_index, 1, verbose_flag,
                                        female_first_set, female_middle_set, &last_names_simple,
                                        middle_name_chance, 1, male_surname, max_middle_names);
                printf("---\n"); // Erottele parit
            }
            printf("-----------------------------------\n");

        } else {
            // Normaali yhden nimen generointi
            DecadeData *first_set = (gender_flag == 0) ? male_first_set : female_first_set;
            DecadeData *middle_set = (gender_flag == 0) ? male_middle_set : female_middle_set;

            // TARKISTUS
            if (period_index >= first_set->num_decades || first_set->lists[period_index].count == 0) {
                fprintf(stderr, "\nError: There are not enough first names in the selected time period. (%s). \n", first_set->decades[period_index]);
            } else {
                // KUTSUTAAN GENERATION FUNKTIOTA SILMUKASSA
                printf("\n--- Generated Names (%d total) ---\n", name_count);
                for (int k = 0; k < name_count; k++) {
                    generate_and_print_name(period_index, gender_flag, verbose_flag,
                                            first_set, middle_set, &last_names_simple,
                                            middle_name_chance,
                                            generate_last_name,
                                            override_last_name,
                                            max_middle_names);
                }
                printf("-----------------------------------\n");
            }
        }

    } else if (!automatic_generation) {
        printf("Exiting the program.\n");
    }

    // --- PUHDISTUS ---
    cleanup_and_exit:
    free_decade_data(&first_names);
    free_decade_data(&middle_names);
    free_names(&last_names_simple);
    free_decade_data(&female_first_names);
    free_decade_data(&female_middle_names);
    return 0;

    cleanup_and_exit_error:
    free_decade_data(&first_names);
    free_decade_data(&middle_names);
    free_names(&last_names_simple);
    free_decade_data(&female_first_names);
    free_decade_data(&female_middle_names);
    return 1;
}
