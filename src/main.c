/**
* @file genesim.c
* @brief GeneSim main program.

genesim - A comprehensive lineage and family relationship simulator.
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

// Maksimipituus yhdelle nimelle ja riville
#define MAX_LINE_LENGTH 256
#define MAX_NAME_LENGTH 100

// Rakenne nimilistan tietojen tallentamiseen
typedef struct {
    char **names;
    int count;
} NameList;

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

    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0; // Poista rivinvaihto

        list->names = (char **)realloc(list->names, (list->count + 1) * sizeof(char *));
        if (list->names == NULL) {
            perror("Memory allocation failed (simple realloc)");
            exit(EXIT_FAILURE);
        }

        list->names[list->count] = strdup(buffer);
        if (list->names[list->count] == NULL) {
            perror("Memory allocation failed (simple strdup)");
            exit(EXIT_FAILURE);
        }

        list->count++;
    }

    fclose(file);
    printf("Loaded %d name of the file: %s\n", list->count, filename);
}


// Funktio lataa nimet CSV-tiedostosta (ottaa vain ensimmäisen sarakkeen)
void load_names_from_csv(const char *filename, NameList *list) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "WARNING: Unable to open CSV-file: %s\n", filename);
        list->count = 0;
        list->names = NULL;
        return;
    }

    list->count = 0;
    list->names = NULL;
    char buffer[MAX_LINE_LENGTH];

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
        temp_buffer[strcspn(temp_buffer, "\n")] = 0;

        // Käytetään strtok-funktiota erottamaan merkkijono pilkun (',') kohdalta
        char *name_token = strtok(temp_buffer, ",");

        if (name_token != NULL) {
            // Varmistus: Tyhjien nimien ohitus (tapahtuu usein virheellisissä CSV-riveissä)
            if (strlen(name_token) > 0) {

                // Kasvatetaan muistia
                list->names = (char **)realloc(list->names, (list->count + 1) * sizeof(char *));
                if (list->names == NULL) {
                    perror("Memory allocation failed (csv realloc)");
                    exit(EXIT_FAILURE);
                }

                // Tallennetaan nimi
                list->names[list->count] = strdup(name_token);
                if (list->names[list->count] == NULL) {
                    perror("Memory allocation failed (csv strdup)");
                    exit(EXIT_FAILURE);
                }

                list->count++;
            }
        }
    }

    fclose(file);
    printf("Loaded %d name from CSV-file: %s\n", list->count, filename);
}

// --- 2. APUFUNKTIOT ---

// Funktio vapauttaa NameList-rakenteen varaaman muistin
void free_names(NameList *list) {
    if (list->names != NULL) {
        for (int i = 0; i < list->count; i++) {
            free(list->names[i]);
        }
        free(list->names);
    }
}

// Funktio valitsee ja palauttaa satunnaisen nimen NameList-rakenteesta
const char* select_random_name(const NameList *list) {
    if (list->count == 0) {
        return "";
    }
    int index = rand() % list->count;
    return list->names[index];
}

// --- 3. PÄÄOHJELMA ---

int main() {
    // Asetetaan satunnaislukugeneraattorin siemen
    srand(time(NULL));

    NameList first_names, middle_names, last_names;

    // 1. Ladataan nimet listoihin (Muista muuttaa tiedostonimet vastaamaan omia tiedostojasi!)

    // Esimerkki CSV-latauksesta (ensimmäiset nimet):
    load_names_from_csv("data/FI-fi/Finnish-men-firts-names.csv", &first_names);
    load_names_from_csv("data/FI-fi/Finnish-men-seconds-names.csv", &middle_names);
    load_names_from_csv("data/FI-fi/Finnish-men-last-names.csv", &last_names);

    // Esimerkki simple-latauksesta (keskinimet ja sukunimet, olettaen että ne ovat omissa tiedostoissaan)
    // load_names_simple("data/FI-fi/firstnames.txt", &first_names);
    // load_names_simple("data/FI-fi/middlenames.txt", &middle_names);
    // load_names_simple("data/FI-fi/surnames.txt", &last_names);

    printf("\n--- Randomly generated names (Middle name with 50%% probability) ---\n");

    // 2. Generoidaan ja tulostetaan 1 satunnaista nimeä
    for (int i = 0; i < 1; i++) {
        const char *first = select_random_name(&first_names);
        const char *last = select_random_name(&last_names);
        const char *middle = "";

        // Ehto keskinimen valinnalle (50 % todennäköisyys ja lista ei saa olla tyhjä)
        if (rand() % 2 == 1 && middle_names.count > 0) {
            middle = select_random_name(&middle_names);
        }

        // Tulostus: Jos middle on tyhjä, tulostetaan vain kaksi nimeä.
        if (strlen(middle) > 0) {
            printf("%d: %s %s %s\n", i + 1, first, middle, last);
        } else {
            printf("%d: %s %s\n", i + 1, first, last);
        }
    }

    // 3. Vapautetaan kaikki dynaamisesti varattu muisti
    free_names(&first_names);
    free_names(&middle_names);
    free_names(&last_names);

    return 0;
}
