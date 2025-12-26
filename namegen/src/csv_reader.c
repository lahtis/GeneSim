#include "args.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "csv_reader.h"
#include "debug.h"
#include "normalize.h"

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

int load_names_from_csv(const char *filename, char ***names, int *count, const Args *args) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    char line[1024];
    int capacity = 100;
    *names = malloc(capacity * sizeof(char*));
    *count = 0;

    // Periodi 1-11 vastaa sarakkeita 1-11 (sarake 0 on itse nimi)
    int target_col = (args->period > 0) ? args->period : 1;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        normalize_line(line);

        // K‰ytet‰‰n puolipistett‰ erottimena master_config.json mukaisesti
        char *current = line;
        char *name = strsep(&current, ";");
        char *weight_str = NULL;

        // Hyp‰t‰‰n halutun periodin sarakkeeseen
        for (int i = 1; i <= target_col; i++) {
            weight_str = strsep(&current, ";");
        }

        // Tarkistetaan onko nimi k‰ytˆss‰ t‰ll‰ periodilla (ei ole "-" tai "0")
        if (name && weight_str && strcmp(weight_str, "-") != 0 && strcmp(weight_str, "0") != 0) {
            if (*count >= capacity) {
                capacity *= 2;
                *names = realloc(*names, capacity * sizeof(char*));
            }
            (*names)[*count] = strdup(name);

            if (args->verbose) {
                printf("DEBUG: Loaded '%s' for period %d (weight: %s)\n", name, target_col, weight_str);
            }
            (*count)++;
        }
    }

    fclose(fp);
    return 1;
}
