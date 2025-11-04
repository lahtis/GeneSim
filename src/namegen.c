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
#define VERSION_MINOR 3
#define VERSION_PATCH 0 // Versio 0.2.1: Interaktiivinen tuki parigeneroinnille ja Help-tekstin siistiminen

// Luodaan versionumerosta merkkijono tulostusta varten
#define VERSION_STRING "0.3.0"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <locale.h>
#include <errno.h>
#include <math.h>


#ifdef _WIN32
#include <windows.h>
#endif

// Makrot
#define MAX_NAME_LENGTH 4096
#define MAX_LINE_LENGTH 1024
#define MAX_NAMES 50000
#define MAX_PERIODS 7

// --- 1. TIETORAKENTEET ---

typedef struct {
    char name[MAX_NAME_LENGTH];
} Name;

typedef struct {
    Name *names;
    int count;
} NameList;

typedef struct {
    NameList periods[MAX_PERIODS];
    int total_names;
} DecadeData;

typedef struct {
    char *male_first_file;
    char *male_middle_file;
    char *female_first_file;
    char *female_middle_file;
    char *last_file_simple;
} ConfigPaths;

ConfigPaths app_paths = { NULL, NULL, NULL, NULL, NULL };

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


// --- 2. APUFUNKTIOT (Muistinhallinta, String-käsittely & Lataus) ---

char* trim_leading_spaces(char *str) {
    if (str == NULL) return NULL;
    while (isspace((unsigned char)*str)) {
        str++;
    }
    size_t len = strlen(str);
    if (len == 0) return str;
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
    return str;
}

char* trim_and_lower(char *str) {
    if (str == NULL) return NULL;
    char *trimmed = trim_leading_spaces(str);
    for (char *p = trimmed; *p; p++) {
        *p = tolower((unsigned char)*p);
    }
    return trimmed;
}

void free_config_paths() {
    if (app_paths.male_first_file) free(app_paths.male_first_file);
    if (app_paths.male_middle_file) free(app_paths.male_middle_file);
    if (app_paths.female_first_file) free(app_paths.female_first_file);
    if (app_paths.female_middle_file) free(app_paths.female_middle_file);
    if (app_paths.last_file_simple) free(app_paths.last_file_simple);
}

void load_config(const char *filename, int verbose) {
    app_paths.male_first_file = strdup("data/FI-fi/Finnish-men-first-names.csv");
    app_paths.male_middle_file = strdup("data/FI-fi/Finnish-men-seconds-names.csv");
    app_paths.female_first_file = strdup("data/FI-fi/Finnish-women-first-names.csv");
    app_paths.female_middle_file = strdup("data/FI-fi/Finnish-women-middle-names.csv");
    app_paths.last_file_simple = strdup("data/FI-fi/Finnish-last-names.csv");

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        if (verbose) {
            fprintf(stderr, "WARNING: Config file '%s' not found. Using default paths.\n", filename);
        }
        return;
    }

    char buffer[MAX_LINE_LENGTH];
    int paths_updated = 0;

    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        char *line = buffer;
        line = trim_leading_spaces(line);
        if (strlen(line) == 0 || line[0] == ';' || line[0] == '#' || line[0] == '[') {
            continue;
        }

        char *equal_sign = strchr(line, '=');
        if (equal_sign == NULL) {
            continue;
        }

        *equal_sign = '\0';
        char *key = trim_and_lower(line);
        char *value = trim_leading_spaces(equal_sign + 1);

        if (strlen(key) == 0 || strlen(value) == 0) {
            continue;
        }

        if (strcmp(key, "male_first_names") == 0) { free(app_paths.male_first_file); app_paths.male_first_file = strdup(value); paths_updated++; }
        else if (strcmp(key, "male_middle_names") == 0) { free(app_paths.male_middle_file); app_paths.male_middle_file = strdup(value); paths_updated++; }
        else if (strcmp(key, "female_first_names") == 0) { free(app_paths.female_first_file); app_paths.female_first_file = strdup(value); paths_updated++; }
        else if (strcmp(key, "female_middle_names") == 0) { free(app_paths.female_middle_file); app_paths.female_middle_file = strdup(value); paths_updated++; }
        else if (strcmp(key, "last_names") == 0) { free(app_paths.last_file_simple); app_paths.last_file_simple = strdup(value); paths_updated++; }
    }

    fclose(file);
    if (verbose) {
        printf("Configuration loaded from '%s'. Updated %d file paths.\n", filename, paths_updated);
    }
}

void free_names(NameList *list) {
    if (list->names) {
        free(list->names);
        list->names = NULL;
        list->count = 0;
    }
}

void free_decade_data(DecadeData *data) {
    for (int i = 0; i < MAX_PERIODS; i++) {
        free_names(&data->periods[i]);
    }
    data->total_names = 0;
}

int is_valid_name(const char *name) {
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]) &&
            name[i] != ' ' && name[i] != '-') {
            return 0;
        }
    }
    return 1;
}

void load_names_simple(const char *filename, NameList *list, int verbose) {
    if (filename == NULL) return;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Cannot open name file: %s (Skipping load)\n", filename);
        return;
    }

    list->count = 0;
    list->names = (Name *)malloc(MAX_NAMES * sizeof(Name));
    if (list->names == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed for simple name list.\n");
        fclose(file);
        return;
    }

    char buffer[MAX_NAME_LENGTH];
    while (fgets(buffer, MAX_NAME_LENGTH, file) != NULL && list->count < MAX_NAMES) {
        buffer[strcspn(buffer, "\n\r")] = 0;
        char *name = trim_leading_spaces(buffer);

        char *comma_pos = strchr(name, ',');
        if (comma_pos != NULL) {
            *comma_pos = '\0';
        }

        char *final_name = trim_leading_spaces(name);


        if (strlen(final_name) > 0 && is_valid_name(final_name)) {
            strcpy(list->names[list->count].name, final_name);
            list->count++;
        } else if (strlen(final_name) > 0 && verbose) {
            fprintf(stderr, "WARNING: Skipping invalid name candidate: '%s' in %s\n", final_name, filename);
        }
    }

    fclose(file);
    if (verbose) {
        printf("Loaded %d names from %s\n", list->count, filename);
    }
}

// VANHA load_names_multi_column poistettu / korvattu alla olevalla logiikalla

// UUSI FUNKTIO: Lataa nimet, joissa jokainen sarake on nimi (kuten käyttäjän datassa)
void load_names_period_list(const char *filename, DecadeData *data, int verbose) {
    if (filename == NULL) return;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Cannot open name list file: %s (Skipping load)\n", filename);
        return;
    }

    data->total_names = 0;

    // Alusta DecadeData
    for (int i = 0; i < MAX_PERIODS; i++) {
        data->periods[i].count = 0;
        if (data->periods[i].names == NULL) {
            data->periods[i].names = (Name *)malloc(MAX_NAMES * sizeof(Name));
        }
        if (data->periods[i].names == NULL) {
             fprintf(stderr, "ERROR: Memory allocation failed for DecadeData period %d.\n", i);
             fclose(file);
             return;
        }
    }

    char buffer[MAX_LINE_LENGTH];

    // OHITA OTSIKKORIVI
    if (fgets(buffer, MAX_LINE_LENGTH, file) == NULL) {
        fclose(file);
        return;
    }

    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        char *token;
        char *saveptr;
        int period_index = 0;

        buffer[strcspn(buffer, "\n\r")] = 0;

        char temp_buffer[MAX_LINE_LENGTH];
        strcpy(temp_buffer, buffer);

        // Loopataan MAX_PERIODS + 1 (jos ensimmäinen sarake on juurinimi)
        // Kuitenkin, data vaikuttaa olevan vain 7 saraketta + nimeä alussa, joten luotetaan:

        // Ensimmäinen token (sarake 1) on usein rivin nimi. Meillä on MAX_PERIODS = 7
        // Luetaan 8 tokenia (sarake 0 + sarake 1..7)
        for(int i = 0; i <= MAX_PERIODS; i++) {
            token = strtok_r((i == 0) ? temp_buffer : NULL, ",", &saveptr);

            if (token == NULL) break;

            // Ohita ensimmäinen sarake (i == 0)
            if (i == 0) continue;

            // period_index on nyt 0..6 (MAX_PERIODS)
            period_index = i - 1;

            char *name_candidate = trim_leading_spaces(token);

            if (strlen(name_candidate) > 0 && is_valid_name(name_candidate) && data->periods[period_index].count < MAX_NAMES) {

                strcpy(data->periods[period_index].names[data->periods[period_index].count].name, name_candidate);
                data->periods[period_index].count++;
                data->total_names++;
            }
        }
    }

    fclose(file);
    if (verbose) {
        printf("Loaded %d total name entries across %d periods from %s\n", data->total_names, MAX_PERIODS, filename);
    }
}


char* get_random_name(NameList *list) {
    if (list->count == 0 || list->names == NULL) {
        return NULL;
    }
    int index = rand() % list->count;
    return list->names[index].name;
}

// --- 3. GENERATION LOGIC (Pysyy samana) ---

void generate_and_print_name(
    NameList *first_names,
    NameList *middle_names,
    NameList *last_names,
    const char *surname_override,
    int middle_name_chance,
    int max_middle_names,
    int no_last_name,
    char *output_buffer
) {
    output_buffer[0] = '\0';

    char *name;

    if (first_names->count == 0) {
        snprintf(output_buffer, MAX_LINE_LENGTH, "ERROR: Could not generate first name (List empty).");
        return;
    }

    // 1. Etunimi
    name = get_random_name(first_names);
    if (name != NULL) {
        strncat(output_buffer, name, MAX_LINE_LENGTH - strlen(output_buffer) - 1);
    } else {
        snprintf(output_buffer, MAX_LINE_LENGTH, "ERROR: Could not generate first name (List count: %d).", first_names->count);
        return;
    }

    // 2. Keskinimet
    int middle_count = 0;
    while (middle_count < max_middle_names && strlen(output_buffer) < MAX_LINE_LENGTH - MAX_NAME_LENGTH) {
        if (rand() % 100 < middle_name_chance) {
            if (middle_names->count == 0) {
                break;
            }
            name = get_random_name(middle_names);
            if (name != NULL) {
                strncat(output_buffer, " ", MAX_LINE_LENGTH - strlen(output_buffer) - 1);
                strncat(output_buffer, name, MAX_LINE_LENGTH - strlen(output_buffer) - 1);
                middle_count++;
            } else {
                break;
            }
        } else {
            break;
        }
    }

    // 3. Sukunimi
    if (!no_last_name) {
        const char *last_name_to_use = NULL;

        if (surname_override != NULL) {
            last_name_to_use = surname_override;
        } else if (last_names != NULL && last_names->count > 0) {
            last_name_to_use = get_random_name(last_names);
        }

        if (last_name_to_use != NULL) {
            strncat(output_buffer, " ", MAX_LINE_LENGTH - strlen(output_buffer) - 1);
            strncat(output_buffer, last_name_to_use, MAX_LINE_LENGTH - strlen(output_buffer) - 1);
        } else if (surname_override == NULL) {
            if (strlen(output_buffer) + 20 < MAX_LINE_LENGTH) {
                strncat(output_buffer, " (No Random Last Name)", MAX_LINE_LENGTH - strlen(output_buffer) - 1);
            }
        }
    }
}


// --- 4. MAIN PROGRAM ---

void print_help() {
    // Käytä vain suoraan tiedossa olevia makroja (kuten MAX_PERIODS)
    // Tässä käytämme suoraan numeroa 7, koska se on oletus.

    printf("namegen\n");
    printf("\n");
    printf("Usage: namegen.exe [-h] [-v] [-p <Number>] [-g <M/F>] [-n <count>] ...\n");
    printf("\n");
    printf("Options:\n");
    printf("  -p <number>, --period <number>        Selects the time period (1-7).\n");
    printf("  -g <gender>, --gender <M/F>           Selects gender (ignored if -P is used).\n");
    printf("  -n <count>, --count <count>           Specifies the number of names (or couples).\n");
    printf("  -P, --couple                          Generates names for a couple (M+F).\n");
    printf("  -F, --family                          Generates a family (Parents + Kids).\n");
    printf("  -A, --age                             Includes birth year/age.\n");
    printf("  -S <surname>, --set-last-name <name>  Sets the surname for all generated names.\n");
    printf("  -L, --no-last-name                    Does not generate a last name.\n");
    printf("  -m <chance>, --middle-chance <chance> Middle name probability (0-100, default 50).\n");
    printf("  -M <max>, --max-middle-names <max>    Max number of middle names (1-3, default 1).\n");
    printf("  -v, --verbose                         Show verbose output (file loading).\n");
    printf("  -h, --help                            Displays this help message.\n");
    printf("  -V, --version                         Displays the version number.\n");
    printf("\n");
}

int main(int argc, char *argv[]) {

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(65001);
    #endif

    setlocale(LC_ALL, "");
    srand(time(NULL));

    // ... (Parametrien esivalmistelu)
    int period_num = 0;
    int count = 1;
    char gender = 'M';
    int verbose_flag = 0;
    int couple_mode = 0;
    int family_mode = 0;
    int num_kids = 2;
    int random_kids_mode = 0;
    int age_mode = 0;
    const char *surname_override = NULL;
    int middle_name_chance = 50;
    int max_middle_names = 1;
    int no_last_name = 0;
    int interactive_mode = 1;

    // --- ESIVALMISTELU & PARAMETRIEN LUKU ---
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose_flag = 1;
            break;
        }
    }
    load_config("config.ini", verbose_flag);

    for (int i = 1; i < argc; i++) {
        interactive_mode = 0;
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) { print_help(); goto cleanup_and_exit; }
        if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0) { printf("Name Source Generator v0.3.0 Name List Optimized\n"); goto cleanup_and_exit; }

        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--period") == 0) { if (i + 1 < argc) period_num = atoi(argv[++i]); }
        else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--count") == 0) { if (i + 1 < argc) count = atoi(argv[++i]); }
        else if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gender") == 0) { if (i + 1 < argc) gender = toupper(argv[++i][0]); }
        else if (strcmp(argv[i], "-P") == 0 || strcmp(argv[i], "--couple") == 0) { couple_mode = 1; }
        else if (strcmp(argv[i], "-F") == 0 || strcmp(argv[i], "--family") == 0) { family_mode = 1; couple_mode = 1; }
        else if (strcmp(argv[i], "-k") == 0 || strcmp(argv[i], "--kids") == 0) {
            if (i + 1 < argc) num_kids = atoi(argv[++i]);
            if (num_kids < 1) num_kids = 1;
            if (num_kids > 10) num_kids = 10;
        }
        else if (strcmp(argv[i], "-R") == 0 || strcmp(argv[i], "--random-kids") == 0) { random_kids_mode = 1; }
        else if (strcmp(argv[i], "-A") == 0 || strcmp(argv[i], "--age") == 0) { age_mode = 1; }
        else if (strcmp(argv[i], "-S") == 0 || strcmp(argv[i], "--set-last-name") == 0) { if (i + 1 < argc) surname_override = argv[++i]; }
        else if (strcmp(argv[i], "-L") == 0 || strcmp(argv[i], "--no-last-name") == 0) { no_last_name = 1; }
        else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--middle-chance") == 0) { if (i + 1 < argc) middle_name_chance = atoi(argv[++i]); }
        else if (strcmp(argv[i], "-M") == 0 || strcmp(argv[i], "--max-middle-names") == 0) {
            if (i + 1 < argc) max_middle_names = atoi(argv[++i]);
            if (max_middle_names > 3) max_middle_names = 3;
            if (max_middle_names < 1) max_middle_names = 1;
        }
    }

    if (couple_mode || family_mode || surname_override != NULL) {
        no_last_name = 0;
    }

    // --- INTERAKTIIVINEN TILA ---
    if (interactive_mode) {
        int choice;
        printf("--- Interactive Name Generation ---\n");

        printf("Select period (1=1860s, 7=1920s, 0=Random): ");
        if (scanf("%d", &period_num) != 1) { period_num = 0; }
        clear_input_buffer();
        // ... (Interaktiivinen logiikka jatkuu)
        printf("Generate names for a couple (M+F)? (1=Yes, 0=No, default 0): ");
        if (scanf("%d", &choice) == 1) couple_mode = (choice == 1);
        clear_input_buffer();

        if (couple_mode) {
            printf("Generate a full family (Parents + Kids)? (1=Yes, 0=No, default 0): ");
            if (scanf("%d", &choice) == 1) family_mode = (choice == 1);
            clear_input_buffer();
            if (family_mode) couple_mode = 1;
        }

        if (family_mode) {
            printf("Randomize number of children (1-5)? (1=Yes, 0=No, default 0): ");
            if (scanf("%d", &choice) == 1) random_kids_mode = (choice == 1);
            clear_input_buffer();

            if (!random_kids_mode) {
                printf("How many children to generate (1-10, default 2): ");
                if (scanf("%d", &num_kids) != 1) { num_kids = 2; }
                if (num_kids < 1) num_kids = 1;
                if (num_kids > 10) num_kids = 10;
            }
            clear_input_buffer();

            printf("How many families to generate (default 1): ");
            if (scanf("%d", &count) != 1) count = 1;
            no_last_name = 0;
            clear_input_buffer();
        }

        if (family_mode || !couple_mode) {
            printf("Max middle names (1-3, default 1): ");
            if (scanf("%d", &max_middle_names) != 1) { max_middle_names = 1; }
            if (max_middle_names > 3) max_middle_names = 3;
            if (max_middle_names < 1) max_middle_names = 1;
            clear_input_buffer();

            printf("Middle name chance (0-100, default 50): ");
            if (scanf("%d", &middle_name_chance) != 1) { middle_name_chance = 50; }
            clear_input_buffer();

            printf("Include birth year/age? (1=Yes, 0=No, default 0): ");
            if (scanf("%d", &choice) == 1) age_mode = (choice == 1);
            clear_input_buffer();
        }

        if (!family_mode && !couple_mode) {
            printf("Gender (M/F, default M): ");
            if (scanf(" %c", &gender) == 1) gender = toupper(gender);
            clear_input_buffer();

            printf("Exclude surname? (1=Yes, 0=No, default 0): ");
            if (scanf("%d", &choice) == 1) no_last_name = (choice == 1);
            clear_input_buffer();

            printf("How many names to generate (default 1): ");
            if (scanf("%d", &count) != 1) count = 1;
            clear_input_buffer();
        } else if (couple_mode && !family_mode) {
            printf("How many couples to generate (default 1): ");
            if (scanf("%d", &count) != 1) count = 1;
            no_last_name = 0;
            clear_input_buffer();
        }
        printf("\n");
    }

    if (period_num < 0 || period_num > MAX_PERIODS) period_num = 0;

    // --- TIEDOSTOJEN LATAUS: NIMIDATALLE OPTIMOITU ---
    DecadeData first_names = {0};
    DecadeData middle_names = {0};
    DecadeData female_first_names = {0};
    DecadeData female_middle_names = {0};
    NameList last_names_simple = {0};

    if (verbose_flag) {
        printf("--- Reading files (using paths from config.ini) ---\n");
    }

    // Lisää tämä koodi main-funktioon tiedostojen latauksen jälkeen
    if (verbose_flag) {
    int index = 6; // Periodi 7 = indeksi 6
    printf("\n--- Period 7 (1920-29) Name Counts ---\n");
    printf("Male First Names: %d\n", first_names.periods[index].count);
    printf("Male Middle Names: %d\n", middle_names.periods[index].count);
    printf("Female First Names: %d\n", female_first_names.periods[index].count);
    printf("Female Middle Names: %d\n", female_middle_names.periods[index].count);
    printf("--------------------------------------\n");
}

    // 1. Monisarakkeiset tiedostot: KÄYTÄ UUTTA load_names_period_list -FUNKTIOTA
    load_names_period_list(app_paths.male_first_file, &first_names, verbose_flag);
    load_names_period_list(app_paths.male_middle_file, &middle_names, verbose_flag);
    load_names_period_list(app_paths.female_first_file, &female_first_names, verbose_flag);
    load_names_period_list(app_paths.female_middle_file, &female_middle_names, verbose_flag);

    // 2. Sukunimet
    int generate_last_name = !no_last_name;
    if (generate_last_name && surname_override == NULL) {
        load_names_simple(app_paths.last_file_simple, &last_names_simple, verbose_flag);
    }


    int random_period = (period_num == 0);
    char output_buffer[MAX_LINE_LENGTH];

    // --- GENERATION LOOP (Pysyy samana) ---
    for (int i = 0; i < count; i++) {
        int current_period_index;
        if (random_period) {
            current_period_index = rand() % MAX_PERIODS;
        } else {
            current_period_index = period_num - 1;
        }

        int birth_year = 0;
        if (age_mode) {
            int decade_start = 1860 + (current_period_index * 10);
            birth_year = decade_start + (rand() % 10);
        }

        NameList *male_first = &first_names.periods[current_period_index];
        NameList *male_middle = &middle_names.periods[current_period_index];
        NameList *female_first = &female_first_names.periods[current_period_index];
        NameList *female_middle = &female_middle_names.periods[current_period_index];

        if (family_mode) {
            // **PERHETILA**
            if (male_first->count == 0 || female_first->count == 0) {
                fprintf(stderr, "ERROR: Cannot generate family. Name data missing for period %d. Skipping generation.\n", current_period_index + 1);
                continue;
            }
            // ... (Generointilogiikka pysyy samana)
            const char *family_surname = NULL;
            if (surname_override != NULL) {
                family_surname = surname_override;
            } else if (last_names_simple.count > 0) {
                family_surname = get_random_name(&last_names_simple);
            }

            int current_num_kids = num_kids;
            if (random_kids_mode) {
                current_num_kids = 1 + (rand() % 5);
            }

            printf("--- Family %d (Period %d) ---\n", i + 1, current_period_index + 1);

            // 1. Isä (Male Parent)
            generate_and_print_name(male_first, male_middle, NULL, family_surname, middle_name_chance, max_middle_names, 0, output_buffer);
            printf("Father: %s", output_buffer);
            if (age_mode) { printf(" (Born: %d)", birth_year); }
            printf("\n");

            // 2. Äiti (Female Parent)
            generate_and_print_name(female_first, female_middle, NULL, family_surname, middle_name_chance, max_middle_names, 0, output_buffer);
            printf("Mother: %s", output_buffer);
            if (age_mode) { printf(" (Born: %d)", birth_year); }
            printf("\n");

            // 3. Lapset (Kids)
            for (int k = 0; k < current_num_kids; k++) {
                char kid_gender = (rand() % 2 == 0) ? 'M' : 'F';

                NameList *kid_first = (kid_gender == 'M') ? male_first : female_first;
                NameList *kid_middle = (kid_gender == 'M') ? male_middle : female_middle;

                int kid_birth_year = birth_year + 5 + (rand() % 11);

                generate_and_print_name(kid_first, kid_middle, NULL, family_surname, middle_name_chance, max_middle_names, 0, output_buffer);
                printf("Child %d (%c): %s", k + 1, kid_gender, output_buffer);
                if (age_mode) { printf(" (Born: %d)", kid_birth_year); }
                printf("\n");
            }
            printf("-----------------\n");


        } else if (couple_mode) {
            // **PARITILA**
            if (male_first->count == 0 || female_first->count == 0) {
                fprintf(stderr, "ERROR: Cannot generate couple. Name data missing for period %d. Skipping generation.\n", current_period_index + 1);
                continue;
            }

            const char *couple_surname = NULL;
            if (surname_override != NULL) {
                couple_surname = surname_override;
            } else if (last_names_simple.count > 0) {
                couple_surname = get_random_name(&last_names_simple);
            }

            generate_and_print_name(male_first, male_middle, NULL, couple_surname, middle_name_chance, max_middle_names, 0, output_buffer);
            printf("Male: %s", output_buffer);
            if (age_mode) { printf(" (Born: %d)", birth_year); }
            printf("\n");

            generate_and_print_name(female_first, female_middle, NULL, couple_surname, middle_name_chance, max_middle_names, 0, output_buffer);
            printf("Female: %s", output_buffer);
            if (age_mode) { printf(" (Born: %d)", birth_year); }
            printf("\n");

        } else {
            // **YKSITTÄINEN NIMI**
            NameList *current_first, *current_middle;
            if (gender == 'F') {
                current_first = female_first;
                current_middle = female_middle;
            } else {
                current_first = male_first;
                current_middle = male_middle;
            }

            if (current_first->count == 0) {
                fprintf(stderr, "ERROR: Cannot generate name. Data missing for %c in period %d. Skipping.\n", gender, current_period_index + 1);
                continue;
            }

            generate_and_print_name(current_first, current_middle, &last_names_simple, surname_override, middle_name_chance, max_middle_names, no_last_name, output_buffer);
            printf("%s", output_buffer);
            if (age_mode) { printf(" (Born: %d)", birth_year); }
            printf("\n");
        }
    }


    // --- PUHDISTUS ---
    cleanup_and_exit:
    free_decade_data(&first_names);
    free_decade_data(&middle_names);
    free_decade_data(&female_first_names);
    free_decade_data(&female_middle_names);
    free_names(&last_names_simple);
    free_config_paths();
    return 0;
}
