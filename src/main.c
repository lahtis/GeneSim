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

// Maksimipituus yhdelle nimelle (voi kasvattaa tarvittaessa)
#define MAX_NAME_LENGTH 100

// Rakenne nimilistan tietojen tallentamiseen
typedef struct {
    char **names; // Osoitin taulukkoon, jossa merkkijonot (nimet) ovat
    int count;    // Nimien lukumäärä listassa
} NameList;

// Funktio lukee nimet tiedostosta dynaamisesti NameList-rakenteeseen
void load_names(const char *filename, NameList *list) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("File opening failed!");
        list->count = 0; // Tärkeää, jos tiedostoa ei löydy
        list->names = NULL;
        return; // Palataan virheen sattuessa
    }

    list->count = 0;
    list->names = NULL;
    char buffer[MAX_NAME_LENGTH];

    // Luetaan nimi kerrallaan tiedostosta
    while (fgets(buffer, MAX_NAME_LENGTH, file) != NULL) {
        // Poista rivinvaihto, jos sellainen on (viimeinen merkki)
        buffer[strcspn(buffer, "\n")] = 0;

        // 1. Kasvata taulukon kokoa yhdellä (realloc)
        list->names = (char **)realloc(list->names, (list->count + 1) * sizeof(char *));
        if (list->names == NULL) {
            perror("Memory allocation failed (realloc)");
            exit(EXIT_FAILURE);
        }

        // 2. Varaa muisti luetulle nimelle (strdup luo kopion)
        list->names[list->count] = strdup(buffer);
        if (list->names[list->count] == NULL) {
            perror("Memory allocation failed (strdup)");
            exit(EXIT_FAILURE);
        }

        list->count++;
    }

    fclose(file);
    printf("Loaded %d name from the file: %s\n", list->count, filename);
}

// Funktio vapauttaa NameList-rakenteen varaaman muistin
void free_names(NameList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->names[i]); // Vapautetaan kunkin nimen muisti
    }
    free(list->names); // Vapautetaan osoitintaulukon muisti
}

// Funktio valitsee ja palauttaa satunnaisen nimen NameList-rakenteesta
const char* select_random_name(const NameList *list) {
    if (list->count == 0) {
        return "Name list is empty."; // Palautetaan tyhjä merkkijono, jos lista on tyhjä
    }
    // Satunnainen indeksi välillä 0 - count-1
    int index = rand() % list->count;
    return list->names[index];
}

int main() {
    // Asetetaan satunnaislukugeneraattorin siemen (tärkeää!)
    srand(time(NULL));

    NameList first_names, last_names, middle_names;

    // 1. Lataa nimet tiedostoista
    load_names("data/FI-fi/firstnames.txt", &first_names);   // ETUNIMILISTA
    load_names("data/FI-fi/middlenames.txt", &middle_names); // KESKINIMILISTA
    load_names("data/FI-fi/surnames.txt", &last_names);      // SUKUNIMILISTA

    // Voit lisätä uusia listoja (esim. middle_names) helposti tässä:
    // NameList middle_names;
    // load_names("middlenames.txt", &middle_names);

    printf("\n--- Randomly generated names (Middle name with 50%% probability) ---\n");

    // 2. Generoidaan ja tulostetaan 5 satunnaista nimeä
    for (int i = 0; i < 5; i++) {
        const char *first = select_random_name(&first_names);
        const char *last = select_random_name(&last_names);

        // **KESKINIMEN EHDOTON VALINTA**
        const char *middle = ""; // Oletus: ei keskinimeä

        // rand() % 2 antaa arvon 0 tai 1. Jos arvo on 1, otetaan keskinimi.
        if (rand() % 2 == 1 && middle_names.count > 0) {
            middle = select_random_name(&middle_names);
        }

        // Tulostus: Tässä käytetään ehdollista muotoilua.
        // Jos 'middle' on tyhjä (""), tulostuu vain etunimi ja sukunimi.
        if (strlen(middle) > 0) {
            printf("%d: %s %s %s\n", i + 1, first, middle, last);
        } else {
            printf("%d: %s %s\n", i + 1, first, last);
        }
    }

    // 3. Vapautetaan kaikki dynaamisesti varattu muisti
    free_names(&first_names);
    free_names(&middle_names); // Vapautetaan myös uusi lista
    free_names(&last_names);

    return 0;
}
