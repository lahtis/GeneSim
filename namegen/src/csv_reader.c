#include "args.h"
#include <string.h>   // strcspn, strdup
#include <stdlib.h>   // malloc, realloc
#include <stdio.h>
#include "csv_reader.h"
#include "debug.h"
#include "normalize.h"


int load_names_from_csv(const char *filename, char ***names, int *count, const Args *args) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    char line[1024];
    int capacity = 100;
    *names = malloc(capacity * sizeof(char*));
    *count = 0;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        normalize_line(line);

        char *token = strtok(line, ",");
        while (token) {
            if (*count >= capacity) {
                capacity *= 2;
                *names = realloc(*names, capacity * sizeof(char*));
            }
            (*names)[*count] = strdup(token);
            if (args->verbose) {
                debug_print_hex(line);  // tulostaa hexdumpin jokaisesta rivistä
                printf("DEBUG: loaded name '%s'\n\n", token);
            }
            (*count)++;
            token = strtok(NULL, ",");
        }
    }

    fclose(fp);
    if (args->verbose) {
        debug_print_hex(line);  // tulostaa hexdumpin jokaisesta rivistä
        printf("DEBUG: total %d names loaded from %s\n\n", *count, filename);
    }
    return 1;
}
