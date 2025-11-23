#include "name_data.h"
#include "csv_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "normalize.h"

// Alustaa kaikki nimilistat
int init_name_data(NameData *data, const Args *args, ConfigEntry *entries, int n) {
    const char *firstMPath = find_config(entries, n, "firstMDataPaths");
    const char *secondMPath = find_config(entries, n, "secondMDataPaths");
    const char *firstFPath  = find_config(entries, n, "firstFDataPaths");
    const char *secondFPath = find_config(entries, n, "secondFDataPaths");
    const char *lastPath    = find_config(entries, n, "lastDataPaths");

    if (!firstMPath || !secondMPath || !firstFPath || !secondFPath || !lastPath) {
        if (!firstMPath) fprintf(stderr, "Config missing key: firstMNames\n");
        if (!secondMPath) fprintf(stderr, "Config missing key: secondMNames\n");
        if (!firstFPath) fprintf(stderr, "Config missing key: firstFNames\n");
        if (!secondFPath) fprintf(stderr, "Config missing key: secondFNames\n");
        if (!lastPath) fprintf(stderr, "Config missing key: lastDataPaths\n");
        fprintf(stderr, "ERROR: Missing one or more config entries!\n");
        return 0;
    }

    if (!load_names(firstMPath, &data->firstMNames, &data->firstMCount, args)) return 0;
    if (!load_names(secondMPath, &data->secondMNames, &data->secondMCount, args)) return 0;
    if (!load_names(firstFPath, &data->firstFNames, &data->firstFCount, args)) return 0;
    if (!load_names(secondFPath, &data->secondFNames, &data->secondFCount, args)) return 0;
    if (!load_lastnames(lastPath, &data->lastNames, &data->lastCount, args)) return 0;

    return 1;
}

// Lataa sukunimet rivikohtaisesti, ohittaa otsikon
int load_lastnames(const char *filename, char ***names, int *count, const Args *args) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Could not open %s\n", filename);
        return 0;
    }

    char line[1024];
    int capacity = 100;
    *names = malloc(capacity * sizeof(char*));
    *count = 0;

    int first_line = 1;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        normalize_line(line);

        if (first_line) {
            if (args->verbose) {
            debug_print_hex(line);  // tulostaa hexdumpin jokaisesta rivistä
            printf("DEBUG: skipping header line '%s'\n", line);
            }
            first_line = 0;
            continue;
        }

        if (*count >= capacity) {
            capacity *= 2;
            *names = realloc(*names, capacity * sizeof(char*));
        }
        (*names)[*count] = strdup(line);
        if (args->verbose) {
            debug_print_hex(line);  // tulostaa hexdumpin jokaisesta rivistä
            printf("DEBUG: loaded last name '%s'\n\n", line);
        }
        (*count)++;
    }

    fclose(fp);
    if (args->verbose) {
        printf("DEBUG: total %d last names loaded from %s\n\n", *count, filename);
    }
    return 1;
}

void free_name_data(NameData *data) {
    for (int i = 0; i < data->firstMCount; i++) free(data->firstMNames[i]);
    free(data->firstMNames);

    for (int i = 0; i < data->secondMCount; i++) free(data->secondMNames[i]);
    free(data->secondMNames);

    for (int i = 0; i < data->firstFCount; i++) free(data->firstFNames[i]);
    free(data->firstFNames);

    for (int i = 0; i < data->secondFCount; i++) free(data->secondFNames[i]);
    free(data->secondFNames);

    for (int i = 0; i < data->lastCount; i++) free(data->lastNames[i]);
    free(data->lastNames);
}
