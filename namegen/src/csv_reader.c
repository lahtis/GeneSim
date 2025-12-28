#include "args.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "csv_reader.h"
#include "debug.h"
#include "normalize.h"
#include "loader.h"

// Windows-yhteensopiva strsep-toteutus
char *strsep(char **stringp, const char *delim) {
    char *start = *stringp;
    char *p;
    if (start == NULL) return NULL;
    p = strpbrk(start, delim);
    if (p) {
        *p = '\0';
        *stringp = p + 1;
    } else {
        *stringp = NULL;
    }
    return start;
}

int load_names_from_csv(const char *filename, NameEntry **entries, int *count, const Args *args) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    char line[1024];
    int capacity = 100;
    *entries = malloc(capacity * sizeof(NameEntry));
    *count = 0;

    // Periodi 1-11 vastaa sarakkeita 1-11 (sarake 0 on nimi)
    int target_col = (args->period > 0) ? args->period : 1;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        // Jos sinulla on normalize_line(line); k‰yt‰ sit‰ t‰ss‰

        char *current = line;
        char *name = strsep(&current, ";");
        char *weight_str = NULL;

        if (!name || strlen(name) == 0) continue;

        // Hyp‰t‰‰n halutun periodin sarakkeeseen
        for (int i = 1; i <= target_col; i++) {
            weight_str = strsep(&current, ";");
        }

        // Tarkistetaan onko nimi k‰ytˆss‰ (ei ole "-" tai "0")
        if (weight_str && strcmp(weight_str, "-") != 0 && strcmp(weight_str, "0") != 0) {
            if (*count >= capacity) {
                capacity *= 2;
                *entries = realloc(*entries, capacity * sizeof(NameEntry));
            }

            // Tallennetaan nimi
            strncpy((*entries)[*count].name, name, 63);
            (*entries)[*count].name[63] = '\0';

            // Muutetaan paino numeroksi (atof k‰sittelee pisteen, varmista ettei ole pilkkuja)
            double w = atof(weight_str);
            (*entries)[*count].weight = (w > 0) ? w : 1.0;

            (*count)++;
        }
    }

    if (args->verbose) {
        printf("[DEBUG] Tiedostosta %s ladattu %d nime‰ sarakkeesta %d\n", filename, *count, target_col);
    }

    fclose(fp);
    return 1;
}
