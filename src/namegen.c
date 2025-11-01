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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <ctype.h> // Käytetään isspace:n kanssa trimmaamiseen
#include <windows.h> // LISÄÄ TÄMÄ

// Maksimipituus yhdelle nimelle ja riville
#define MAX_LINE_LENGTH 4096
#define DEBUG_MODE 0

// Rakenne nimilistan tietojen tallentamiseen
typedef struct {
    char **names;
    int count;
} NameList;

// --- B. KOKO TIEDOSTON TIETORAKENNE (DecadeData) ---
// Koko säilö CSV-tiedoston vuosikymmentiedoille
typedef struct {
    char **decades;      // Taulukko vuosikymmenten otsikoille ("1870–79")
    NameList *lists;     // Dynaaminen taulukko NameList-rakenteille
    int num_decades;     // Vuosikymmenten lukumäärä (sarakkeiden lkm)
} DecadeData;

// Funktio poistaa alussa olevat välilyönnit (trimmaa)
char* trim_leading_spaces(char *str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

// Funktio vapauttaa DecadeData-rakenteen varaaman muistin
void free_decade_data(DecadeData *data) {
    if (data->decades != NULL) {
        // 1. Vapauta vuosikymmenten otsikot
        for (int i = 0; i < data->num_decades; i++) {
            free(data->decades[i]);
        }
        free(data->decades);
    }

    if (data->lists != NULL) {
        // 2. Vapauta jokainen NameList
        for (int i = 0; i < data->num_decades; i++) {
            free_names(&data->lists[i]);
        }
        free(data->lists);
    }

// Nollaa laskurit ja osoittimet
    data->num_decades = 0;
    data->decades = NULL;
    data->lists = NULL;
}

// Ladataan nimet CSV-tiedostosta, jossa on useita sarakkeita (yksi sarake = yksi vuosikymmenlista)
void load_names_multi_column(const char *filename, DecadeData *data) {
    // Alustus
    data->num_decades = 0;
    data->decades = NULL;
    data->lists = NULL;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
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
    header_copy[strcspn(header_copy, "\n\r")] = 0; // Poista rivinvaihto

    // Lasketaan sarakkeiden/vuosikymmenten määrä ja varataan muisti otsikoille ja listoille
    char *token = strtok(header_copy, ",");
    while (token != NULL) {
        // Otsikon varaus ja tallennus
        data->decades = (char **)realloc(data->decades, (data->num_decades + 1) * sizeof(char *));
        data->decades[data->num_decades] = strdup(trim_leading_spaces(token));

        // NameList-rakenteen varaus ja alustus
        data->lists = (NameList *)realloc(data->lists, (data->num_decades + 1) * sizeof(NameList));
        data->lists[data->num_decades].names = NULL;
        data->lists[data->num_decades].count = 0;

        data->num_decades++;
        token = strtok(NULL, ",");
    }
    printf("Loaded %d headlines of the decade.\n", data->num_decades);

    // 2. Lue varsinaiset tiedot rivi kerrallaan
    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        buffer[strcspn(buffer, "\n\r")] = 0;
        char data_copy[MAX_LINE_LENGTH];
        strcpy(data_copy, buffer);

        token = strtok(data_copy, ",");
        int col = 0;
        while (token != NULL && col < data->num_decades) {

            char *clean_name = trim_leading_spaces(token); // Puhdista välilyönnit

            // Jos sarake ei ole tyhjä, tallenna nimi
            if (strlen(clean_name) > 0) {
                NameList *current_list = &data->lists[col];

                // Varaa muistia osoittimelle
                current_list->names = (char **)realloc(current_list->names, (current_list->count + 1) * sizeof(char *));

                // Tallenna nimi
                current_list->names[current_list->count] = strdup(clean_name);
                current_list->count++;
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

// Funktio vapauttaa NameList-rakenteen varaaman muistin
void free_names(NameList *list) {
    if (list->names != NULL) {
        for (int i = 0; i < list->count; i++) {
            free(list->names[i]);
        }
        free(list->names);
    }
}

// --- 1. TIEDOSTON LATAUSFUNKTIOT ---

// Funktio lataa nimet tavallisesta tiedostosta (yksi nimi per rivi)
void load_names_simple(const char *filename, NameList *list) {
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
        buffer[strcspn(buffer, "\n\r")] = 0; // Poista rivinvaihto

        char *clean_name = trim_leading_spaces(buffer);
        if (strlen(clean_name) > 0) {

            list->names = (char **)realloc(list->names, (list->count + 1) * sizeof(char *));
            if (list->names == NULL) {
                perror("Memory allocation failed (simple realloc)");
                exit(EXIT_FAILURE);
            }

            // KORJAUS: Vain yksi strdup(), käytetään puhdistettua nimeä
            list->names[list->count] = strdup(clean_name);

            if (list->names[list->count] == NULL) {
                perror("Memory allocation failed (simple strdup)");
                exit(EXIT_FAILURE);
            }

            list->count++;
        }
    }

    fclose(file);
    printf("Loaded %d name from the file: %s\n", list->count, filename);
}


// Funktio lataa nimet CSV-tiedostosta (ottaa vain ensimmäisen sarakkeen huomioon)
void load_names_from_csv(const char *filename, NameList *list) {
    FILE *file = fopen(filename, "r");
    // Siirrä osoitinta eteenpäin ohittaen alussa olevat välilyönnit
    if (file == NULL) {
        fprintf(stderr, "WARNING: Unable to open CSV-file: %s\n", filename);
        list->count = 0;
        list->names = NULL;
        return;
    }

    list->count = 0;
    list->names = NULL;
    char buffer[MAX_LINE_LENGTH];

    if (list->count == 0) {
    return ""; // Palauta tyhjä merkkijono, jos lista on tyhjä.
}

    // OHITA ENSIMMÄINEN OTSIKKORIVI
    if (fgets(buffer, MAX_LINE_LENGTH, file) == NULL) {
        fclose(file);
        return;
    }

    // Luetaan rivi kerrallaan
    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        char temp_buffer[MAX_LINE_LENGTH];
        strcpy(temp_buffer, buffer); // Kopioidaan rivi, koska strtok muuttaa alkuperäistä

        // Poista rivinvaihto kopioidusta
        temp_buffer[strcspn(temp_buffer, "\n\r")] = 0;

        // Käytetään strtok-funktiota erottamaan merkkijono pilkun (',') kohdalta
        char *name_token = strtok(temp_buffer, ",");

       if (name_token != NULL) {
            // Poista mahdollinen ylimääräinen välilyönti nimen alusta
            char *clean_name = name_token;
            while (*clean_name == ' ') {
                clean_name++;
            }

    if (strlen(clean_name) > 0) { // Varmistus: Tyhjien nimien ohitus
                list->names = (char **)realloc(list->names, (list->count + 1) * sizeof(char *));
                if (list->names == NULL) {
                    perror("Memory allocation failed (csv realloc)");
                    fclose(file); // Sulje tiedosto ennen poistumista
                    free_names(list); // Vapauta jo varattu muisti
                    exit(EXIT_FAILURE);
                }

                list->names[list->count] = strdup(clean_name); // Tallenna puhdistettu nimi
                if (list->names[list->count] == NULL) {
                    perror("Memory allocation failed (csv strdup)");
                    fclose(file);
                    free_names(list);
                    exit(EXIT_FAILURE);
                }

                list->count++;
            }
        }
    }

    fclose(file);
    printf("Loaded %d names from CSV-file: %s\n", list->count, filename);
}

// Funktio valitsee ja palauttaa satunnaisen nimen NameList-rakenteesta
const char* select_random_name(const NameList *list) {
    if (list->count == 0) {
        return "";
    }
    // Satunnainen indeksi: 0 ... (list->count - 1)
    int index = rand() % list->count;
    return list->names[index];
}


// --- 3. PÄÄOHJELMA ---

int main() {
    SetConsoleOutputCP(CP_UTF8);  // merkistövirheen korjaus WIN11, poista tämä kun käännät Linuxille.
    // Asettaa ohjelman lokalisoinnin käyttämään UTF-8-merkistöä
    // Tämä yrittää korjata "1890ÔÇô99" -tyyppiset merkkivääristymät
    setlocale(LC_ALL, "fi_FI.UTF-8");

    // Asetetaan satunnaislukugeneraattorin siemen
    srand(time(NULL));

    // KOLME ERI TIETORAKENNETTA
    DecadeData first_names = {NULL, NULL, 0};
    DecadeData middle_names = {NULL, NULL, 0};
    NameList last_names_simple = {NULL, 0}; // Yksinkertainen lista sukunimille

    const char *first_file = "data/FI-fi/Finnish-men-firts-names.csv";
    const char *middle_file = "data/FI-fi/Finnish-men-seconds-names.csv";
    const char *last_file_simple = "data/FI-fi/Finnish-men-last-names.csv";

    // 1. LADATAAN KAIKKI KOLME TIEDOSTOA HETI ALUSSA!
    printf("--- Reading files ---\n");
    load_names_multi_column(first_file, &first_names);
    load_names_multi_column(middle_file, &middle_names);
    // KORJAUS 1 & 2: Lataa sukunimet oikealla muuttujalla ja oikeassa kohdassa!
    load_names_simple(last_file_simple, &last_names_simple);
    printf("--------------------------\n");

    // 2. KRIITTINEN TARKISTUS: Poistu, jos pakolliset tiedostot puuttuvat
    if (first_names.num_decades == 0 || last_names_simple.count == 0) {

        fprintf(stderr, "\nERROR: Required files are missing or empty.\n");
        fprintf(stderr, "Number of first name columns: %d, Number of last names: %d.\n",
                        first_names.num_decades, last_names_simple.count);
        free_decade_data(&first_names);
        free_decade_data(&middle_names);
        free_names(&last_names_simple);
        return 1;
    }

    // ANNA VAROITUS, JOS KESKINIMET PUUTTUVAT, MUTTA JATKA
    if (middle_names.num_decades == 0) {
        fprintf(stderr, "\nWARNING: Middle names file not loaded. The generator does not use middle names.\n");
    }

    // A. KÄYTTÄJÄN ESITTELY JA KYSELY
    print_available_decades(&first_names); // Kutsutaan vain kerran!

    int valinta = 0;
    int index = -1; // Käytettävä indeksi

    printf("Enter the period number for name generation (1-%d) or 0 to exit: ", first_names.num_decades);
    if (scanf("%d", &valinta) != 1 || valinta < 0 || valinta > first_names.num_decades) {
        printf("Incorrect choice.\n");
        valinta = 0; // Jos virhe, poistu
    }

    if (valinta > 0) {
        index = valinta - 1; // Valittu lista, esim. 1 -> indeksi 0

        // TARKISTUS: Varmistetaan, että valitulla indeksillä on nimiä etunimilistassa
        if (index < first_names.num_decades && first_names.lists[index].count > 0) {

            // 1. Nimien valinta
            const char *first = first_names.lists[index].names[rand() % first_names.lists[index].count];
            const char *middle = "";
            const char *last = last_names_simple.names[rand() % last_names_simple.count];

            // 2. KESKINIMEN VALINTA (50% todennäköisyys)
            if (index < middle_names.num_decades &&
                middle_names.lists[index].count > 0 &&
                rand() % 100 < 50) {
                middle = middle_names.lists[index].names[rand() % middle_names.lists[index].count];
            }

            // DEBUG-LOHKO: Tulostaa muuttujien arvot vain, jos DEBUG_MODE on 1
#if DEBUG_MODE
            fprintf(stderr, "DEBUG: First='%s', Middle='%s', Last='%s'\n", first, middle, last);
#endif

            // 3. Tulostus
            printf("\nGenerated name from the period '%s':\n", first_names.decades[index]);
            if (strlen(middle) > 0) {
                printf(">>> %s %s %s <<<\n\n", first, middle, last);
            } else {
                printf(">>> %s %s <<<\n\n", first, last);
            }

        } else {
            printf("\nVIRHE: There are not enough first names in the selected time period. (%s). \n", first_names.decades[index]);
        }
    } else {
        printf("Exiting the program.\n");
    }

    // LOPUSSA: Vapautetaan muisti
    free_decade_data(&first_names);
    free_decade_data(&middle_names);
    free_names(&last_names_simple);

    return 0;
}
