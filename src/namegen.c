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

// Versio: MAJOR.MINOR.PATCH
#define VERSION_MAJOR 0
#define VERSION_MINOR 4
#define VERSION_PATCH 18 //

// Luodaan versionumerosta merkkijono tulostusta varten
#define VERSION (VERSION_MAJOR * 10000 + VERSION_MINOR * 100 + VERSION_PATCH)

// Yhdistäminen versionumero tulostusmerkkijonoksi
#define STR(x) #x
#define XSTR(x) STR(x)
#define VERSION_STRING XSTR(VERSION_MAJOR) "." XSTR(VERSION_MINOR) "." XSTR(VERSION_PATCH)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

// --- KONFIGURAATIO JA MAKROT ---
#define MAX_LINE_LENGTH 512
#define MAX_NAME_LENGTH 64
#define MAX_NAMES 500
#define MAX_PERIODS 7
#define MAX_FAMILIES 10

// --- TIETORAKENTEET ---

typedef struct {
    char name[MAX_NAME_LENGTH];
} NameEntry;

typedef struct {
    NameEntry names[MAX_NAMES];
    int count;
} NamePeriodList;

typedef struct {
    NamePeriodList periods[MAX_PERIODS]; // Indeksit 0-6 vastaavat periodeja 1-7
    int total_names;
} MultiPeriodNames;

typedef struct {
    NameEntry names[MAX_NAMES];
    int count;
} SimpleNameList;

typedef struct {
    char male_first_names[MAX_LINE_LENGTH];
    char male_middle_names[MAX_LINE_LENGTH];
    char female_first_names[MAX_LINE_LENGTH];
    char female_middle_names[MAX_LINE_LENGTH];
    char last_names[MAX_LINE_LENGTH];
} AppPaths;

// --- GLOBAALIT MUUTTUJAT ---
MultiPeriodNames first_names = {0};
MultiPeriodNames middle_names = {0};
MultiPeriodNames female_first_names = {0};
MultiPeriodNames female_middle_names = {0};
SimpleNameList last_names = {0};
AppPaths app_paths = {0};


// --- APUFUNKTIOT (STRING HANDLING) ---
// LISÄÄ TÄMÄ FUNKTIO SYÖTEPUSERIN SIIVOUKSEEN
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// LISÄÄ NÄMÄ MUISTIN VAPAUTUS FUNKTIOT (Jos et käytä mallocia, jätä ne tyhjiksi)
void free_decade_data(void *data) {
    // Tässä versiossa oletetaan, että dynaamista muistia ei ole varattu taulukoille (MAX_NAMES on vakio).
    // Jätetään toistaiseksi tyhjäksi, jotta linkkerivirhe korjaantuu.
}

void free_names(void *data) {
    // Jätetään toistaiseksi tyhjäksi, jotta linkkerivirhe korjaantuu.
}


char* trim_leading_spaces(char *str) {
    if (str == NULL) return NULL;
    while (isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

// UUSI FUNKTIO V0.4.17: Poistaa välilyönnit/tyhjät merkit merkkijonon lopusta
char* trim_trailing_spaces(char *str) {
    if (str == NULL) return NULL;
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
    return str;
}

// Tarkistaa, onko merkkijono kelvollinen nimi (ei numeroita)
int is_valid_name(const char *name) {
    if (name == NULL || *name == '\0') {
        return 0;
    }
    for (size_t i = 0; i < strlen(name); i++) {
        // Sallii vain kirjaimet (ASCII + erikoismerkit) ja välilyönnit
        if (!isalpha((unsigned char)name[i]) && !isspace((unsigned char)name[i]) && name[i] != '-') {
            return 0;
        }
    }
    return 1;
}

// --- KONFIGURAATION LUKU ---

// Lukee arvot config.ini-tiedostosta
int load_config(const char *filename, AppPaths *paths) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        // Jos tiedostoa ei löydy, käytetään oletusarvoja
        printf("WARNING: Could not open config file '%s'. Using default paths.\n", filename);
        // Oletusarvot tähän, jos configia ei ladata
        strcpy(paths->male_first_names, "data/FI-fi/Finnish-men-first-names.csv");
        strcpy(paths->male_middle_names, "data/FI-fi/Finnish-men-seconds-names.csv");
        strcpy(paths->female_first_names, "data/FI-fi/Finnish-women-first-names.csv");
        strcpy(paths->female_middle_names, "data/FI-fi/Finnish-women-middle-names.csv");
        strcpy(paths->last_names, "data/FI-fi/Finnish-last-names.csv");
        return 0; // Palautetaan 0, koska oletusarvot asetettiin
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    // Asetetaan alkuun tyhjät merkkijonot
    paths->male_first_names[0] = '\0';
    paths->male_middle_names[0] = '\0';
    paths->female_first_names[0] = '\0';
    paths->female_middle_names[0] = '\0';
    paths->last_names[0] = '\0';

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n\r")] = 0; // Poistaa rivinvaihdot

        if (line[0] == '[' || line[0] == ';' || line[0] == '#' || line[0] == '\0') {
            continue; // Ohitetaan osion otsikot, kommentit ja tyhjät rivit
        }

        char *equals_sign = strchr(line, '=');
        if (equals_sign) {
            *equals_sign = '\0'; // Jaetaan avain ja arvo

            char *key = trim_leading_spaces(line);
            key = trim_trailing_spaces(key);
            char *value = trim_leading_spaces(equals_sign + 1);
            value = trim_trailing_spaces(value);

            if (strcmp(key, "male_first_names") == 0) strcpy(paths->male_first_names, value);
            else if (strcmp(key, "male_middle_names") == 0) strcpy(paths->male_middle_names, value);
            else if (strcmp(key, "female_first_names") == 0) strcpy(paths->female_first_names, value);
            else if (strcmp(key, "female_middle_names") == 0) strcpy(paths->female_middle_names, value);
            else if (strcmp(key, "last_names") == 0) strcpy(paths->last_names, value);

            count++;
        }
    }

    fclose(file);
    return count;
}


// --- DATAN LATAUSFUNKTIOT ---

// UUSI FUNKTIO V0.4.16: Lataa nimet tiedostosta, jossa sarakkeet ovat periodeja.
int load_names_period_list(const char *filename, MultiPeriodNames *data, int verbose_flag) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        if (verbose_flag) printf("ERROR: Failed to open file: %s\n", filename);
        return 0;
    }

    char buffer[MAX_LINE_LENGTH];
    // Ohitetaan otsikkorivi
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        return 0;
    }

    // Asetetaan alkuun nollat
    data->total_names = 0;
    for (int i = 0; i < MAX_PERIODS; i++) {
        data->periods[i].count = 0;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // TÄRKEÄÄ! Poistaa molemmat \n ja \r
        buffer[strcspn(buffer, "\n\r")] = 0;

        char temp_buffer[MAX_LINE_LENGTH];
        strcpy(temp_buffer, buffer);

        char *token;
        char *rest = temp_buffer;
        int i = 0;

        while ((token = strtok_r(rest, ",", &rest))) {
            if (i >= MAX_PERIODS) break; // Varmistaa, ettei ylitetä taulukon kokoa

            int period_index = i;

            // Kaksi vaihetta trimmaus (alku ja loppu)
            char *name_candidate = trim_leading_spaces(token);
            name_candidate = trim_trailing_spaces(name_candidate); // V0.4.17 KORJAUS

            if (strlen(name_candidate) > 0 && is_valid_name(name_candidate) && data->periods[period_index].count < MAX_NAMES) {

                // Nimi on kelvollinen: tallennetaan listaan
                strcpy(data->periods[period_index].names[data->periods[period_index].count].name, name_candidate);
                data->periods[period_index].count++;
                data->total_names++;

            } else if (strlen(name_candidate) > 0 && !is_valid_name(name_candidate)) {
                // Tunnistetaan, jos yritetään parsia jotain, joka ei ole nimi
                if (verbose_flag) {
                    printf("WARNING: Skipping invalid name candidate: '%s' in %s\n", name_candidate, filename);
                }
            }
            i++;
        }
    }

    fclose(file);
    return data->total_names;
}

// Lataa sukunimet yksinkertaisesta yhden sarakkeen listasta
int load_last_names(const char *filename, SimpleNameList *data, int verbose_flag) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        if (verbose_flag) printf("ERROR: Failed to open last name file: %s\n", filename);
        return 0;
    }

    char buffer[MAX_LINE_LENGTH];
    data->count = 0;

    // Ohitetaan otsikkorivi
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        return 0;
    }

    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n\r")] = 0;

        char *name_candidate = trim_leading_spaces(buffer);
        name_candidate = trim_trailing_spaces(name_candidate);

        if (strlen(name_candidate) > 0 && is_valid_name(name_candidate) && data->count < MAX_NAMES) {
            strcpy(data->names[data->count].name, name_candidate);
            data->count++;
        } else if (strlen(name_candidate) > 0 && !is_valid_name(name_candidate)) {
            if (verbose_flag) {
                printf("WARNING: Skipping invalid name candidate: '%s' in %s\n", name_candidate, filename);
            }
        }
    }

    fclose(file);
    return data->count;
}


// --- GENERATION FUNKTIOT ---

// Valitsee satunnaisen nimen listasta
const char* get_random_name(const NamePeriodList *list) {
    if (list->count == 0) {
        return NULL;
    }
    return list->names[rand() % list->count].name;
}

// Generoi koko nimen ja tulostaa sen
void generate_and_print_name(int period_index, char gender, int max_middle_names, int middle_chance, const char *surname, int include_age, int family_mode, int parent_mode) {

    // Asetetaan nimilistat sukupuolen mukaan
    const MultiPeriodNames *first_list = (gender == 'M') ? &first_names : &female_first_names;
    const MultiPeriodNames *middle_list = (gender == 'M') ? &middle_names : &female_middle_names;

    const NamePeriodList *first_names_period = &first_list->periods[period_index];
    const NamePeriodList *middle_names_period = &middle_list->periods[period_index];

    // Nimen saatavuustarkistus (kriittinen perhegeneroinnissa)
    if (first_names_period->count == 0 || (max_middle_names > 0 && middle_names_period->count == 0)) {
        if (family_mode) {
            printf("ERROR: Cannot generate %s. Name data missing for period %d. Skipping generation.\n",
                   (parent_mode ? "parent" : "child"), period_index + 1);
        } else {
            printf("ERROR: Cannot generate name. Name data missing for period %d. Skipping generation.\n", period_index + 1);
        }
        return;
    }

    // 1. Etunimi
    const char *first = get_random_name(first_names_period);

    if (first == NULL) {
        // Tämä pitäisi olla mahdotonta aiemman count-tarkistuksen ansiosta, mutta turvallisuuden vuoksi.
        printf("ERROR: First name generation failed.\n");
        return;
    }

    printf("%s", first);

    // 2. Keskinimet
    int num_middle = 0;
    if (max_middle_names > 0) {
        // Jos keskinimiä pyydetään, valitaan niiden määrä (1..max_middle_names)
        for (int m = 0; m < max_middle_names; m++) {
            if ((rand() % 100) < middle_chance) {
                const char *middle = get_random_name(middle_names_period);
                if (middle) {
                    printf(" %s", middle);
                    num_middle++;
                }
            }
        }
    }

    // 3. Sukunimi
    if (surname && strlen(surname) > 0) {
        printf(" %s", surname);
    } else if (last_names.count > 0 && !family_mode) {
        // Valitaan sukunimi suoraan SimpleNameList-rakenteesta
        const char *last = last_names.names[rand() % last_names.count].name;
        if (last) {
            printf(" %s", last);
        }
    }

    // 4. Syntymävuosi
    if (include_age) {
        int base_year = 1860 + (period_index * 10);
        int birth_year = base_year + (rand() % 10);

        if (family_mode) {
            // Perhemoodissa vanhempien ikä sijoitetaan periodin sisään
            if (parent_mode) {
                printf(" (Born: %d)", birth_year);
            }
            // Lasten iät lasketaan myöhemmin generate_family -funktiossa
        } else {
            printf(" (Born: %d)", birth_year);
        }
    }

    printf("\n");
}


// --- MAIN LOOP JA HELPPERIT ---

void print_help() {
    // KORJATTU v.0.4.17: Poistettu tuntemattomat muuttujat (data, filename) ja goto
    printf("NameGen (v.%s)\n", VERSION_STRING);
    printf("\n");
    printf("Usage: namegen.exe [-h] [-v] [-p <Number>] [-g <M/F>] [-n <count>] ...\n");
    printf("\n");
    printf("Options:\n");
    printf("  -p <number>, --period <number>        Selects the time period (1-%d, 0=Random).\n", MAX_PERIODS);
    printf("  -g <gender>, --gender <M/F>           Selects gender (M/F, default M).\n");
    printf("  -n <count>, --count <count>           Number of names to generate (default 1).\n");
    printf("  -m <chance>, --middle-chance <chance> Middle name probability (0-100, default 50).\n");
    printf("  -M <max>, --max-middle-names <max>    Max number of middle names (1-3, default 1).\n");
    printf("  -A, --age                             Includes birth year/age.\n");
    printf("  -P, --couple                          Generates names for a couple (M+F).\n");
    printf("  -F, --family                          Generates a family (Parents + Kids).\n");
    printf("  -R, --random-kids                     Randomize number of children (1-5).\n");
    printf("  -S <surname>, --set-last-name <name>  Sets the surname for all generated names.\n");
    printf("  -L, --no-last-name                    Does not generate a last name.\n");
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
    int interactive_mode = 0;

    // --- ESIVALMISTELU & PARAMETRIEN LUKU ---
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose_flag = 1;
        }
    }
    load_config("config.ini", &app_paths);

    // Jos mitään muita parametreja ei annettu kuin mahdollinen '-v', aseta interaktiivinen tila
    if (argc == 1) {
        interactive_mode = 1;
    }

    for (int i = 1; i < argc; i++) {
        interactive_mode = 0;
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) { print_help(); goto cleanup_and_exit; }
        if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0) { printf("Namegen v.%s\n", VERSION_STRING); goto cleanup_and_exit; }
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

        printf("Select period (1=1860–69 2=1870–79 3=1880–89 4=1890–99 5=1900–09 6=1910–19 7=1920–29, 0=Random): ");
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

    if (verbose_flag) {
        printf("--- Reading files (using paths from config.ini) ---\n");
    }



    // Ladataan nimet globaaleihin muuttujiin (MultiPeriodNames & SimpleNameList)
    int loaded_count = 0;

    loaded_count += load_names_period_list(app_paths.male_first_names, &first_names, 1); // <-- Aseta verbose 1
    loaded_count += load_names_period_list(app_paths.male_middle_names, &middle_names, 1); // <-- Aseta verbose 1
    loaded_count += load_names_period_list(app_paths.female_first_names, &female_first_names, 1); // <-- Aseta verbose 1
    loaded_count += load_names_period_list(app_paths.female_middle_names, &female_middle_names, 1); // <-- Aseta verbose 1
    loaded_count += load_last_names(app_paths.last_names, &last_names, 1); // <-- Aseta verbose 1

    if (loaded_count == 0) {
    fprintf(stderr, "\n*** FATAL ERROR: No name data loaded. Check 'config.ini' and data file paths. ***\n");
    };

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

    int random_period = (period_num == 0);
    char output_buffer[MAX_LINE_LENGTH];

// --- GENERATION LOOP ---
for (int i = 0; i < count; i++) {
    int current_period_index;
    if (random_period) {
        current_period_index = rand() % MAX_PERIODS;
    } else {
        current_period_index = period_num - 1;
    }

    // Tarkista, onko datalistat tyhjiä ennen generointia (KANNATTAA JÄTTÄÄ, VAIKKA LATAUS ONNISTUIKIN)
    if (first_names.periods[current_period_index].count == 0 ||
        female_first_names.periods[current_period_index].count == 0)
    {
        fprintf(stderr, "ERROR: Name data missing for period %d. Skipping generation.\n", current_period_index + 1);
        continue;
    }

    int birth_year = 0;
    if (age_mode) {
        int decade_start = 1860 + (current_period_index * 10);
        birth_year = decade_start + (rand() % 10);
    }

    // HUOM: Sukunimi valitaan ennen generointia
    const char *family_surname = NULL;
    if (surname_override != NULL) {
        family_surname = surname_override;
    } else if (last_names.count > 0 && !no_last_name) {
        // KORJATTU SUKUNIMEN VALINTA SimpleNameListille
        family_surname = last_names.names[rand() % last_names.count].name;
    }

    // --- KORJATTU GENERATION LOGIIKKA: Käsittelee kaikki kolme tapausta (F, P, Single) ---
    if (family_mode) {
        // **PERHETILA (Family Mode)** - Toimii jo

        int current_num_kids = num_kids;
        if (random_kids_mode) {
            current_num_kids = 1 + (rand() % 5);
        }

        printf("--- Family %d (Period %d) ---\n", i + 1, current_period_index + 1);

        // 1. Isä (Male Parent)
        printf("Father: ");
        generate_and_print_name(current_period_index, 'M', max_middle_names, middle_name_chance, family_surname, age_mode, 1 /*family_mode*/, 1 /*parent_mode*/);

        // 2. Äiti (Female Parent)
        printf("Mother: ");
        generate_and_print_name(current_period_index, 'F', max_middle_names, middle_name_chance, family_surname, age_mode, 1 /*family_mode*/, 1 /*parent_mode*/);

        // 3. Lapset (Kids)
        for (int k = 0; k < current_num_kids; k++) {
            char kid_gender = (rand() % 2 == 0) ? 'M' : 'F';

            // HUOM: Lapsen syntymävuosi vaatii ikälipun
            int kid_age_mode = (age_mode || (argc > 1)); // Jos joku parametri annettu, oletetaan ikä tarpeelliseksi
            int kid_birth_year = birth_year + 5 + (rand() % 11);

            printf("Child %d (%c): ", k + 1, kid_gender);
            // Vain parent_mode asettaa iäksi birth_yearin. Lapset käyttävät omaa laskettua ikäänsä.
            generate_and_print_name(current_period_index, kid_gender, max_middle_names, middle_name_chance, family_surname, age_mode, 1 /*family_mode*/, 0 /*kid_mode*/);
        }
        printf("-----------------\n");

    } else if (couple_mode) {
        // **PARISKUNTATILA (Couple Mode)** - TÄMÄ PUUTTUI

        printf("\n--- Couple %d (Period %d) ---\n", i + 1, current_period_index + 1);

        // 1. Mies
        printf("Male: ");
        generate_and_print_name(current_period_index, 'M', max_middle_names, middle_name_chance, family_surname, age_mode, 0, 0);

        // 2. Nainen
        printf("Female: ");
        generate_and_print_name(current_period_index, 'F', max_middle_names, middle_name_chance, family_surname, age_mode, 0, 0);
        printf("\n");

    } else {
        // **YKSITTÄISNIMI (Single Name)** - TÄMÄ PUUTTUI

        generate_and_print_name(current_period_index, gender, max_middle_names, middle_name_chance, family_surname, age_mode, 0, 0);
    }

} // --- GENERATION LOOP LOPPUU ---

// --- PUHDISTUS ---
cleanup_and_exit:
    // HUOM: Vapauta globaalit muuttujat, joita käytettiin ladattaessa!
    // Varmista, että nämä vastaavat TIETORAKENNETYYPEJÄSI.
    // Tässä oletetaan, että 'free_decade_data' vapauttaa 'MultiPeriodNames'
    // ja 'free_names' vapauttaa 'SimpleNameList'.

    free_decade_data(&first_names);
    free_decade_data(&middle_names);
    free_decade_data(&female_first_names);
    free_decade_data(&female_middle_names);
    free_names(&last_names); // Käytä globaalia last_names
    // free_config_paths();
    return 0;
}
