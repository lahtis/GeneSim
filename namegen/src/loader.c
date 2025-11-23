#include "loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config *load_config(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return NULL;
    }

    Config *cfg = calloc(1, sizeof(Config));
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        char *key = strtok(line, "=");
        char *val = strtok(NULL, "\n");
        if (!key || !val) continue;

        if (strcmp(key, "firstMDataPaths") == 0) cfg->firstMDataPaths = strdup(val);
        else if (strcmp(key, "secondMDataPaths") == 0) cfg->secondMDataPaths = strdup(val);
        else if (strcmp(key, "firstFDataPaths") == 0) cfg->firstFDataPaths = strdup(val);
        else if (strcmp(key, "secondFDataPaths") == 0) cfg->secondFDataPaths = strdup(val);
        else if (strcmp(key, "lastDataPaths") == 0) cfg->lastDataPaths = strdup(val);
        else if (strcmp(key, "output-file") == 0) cfg->output_file = strdup(val);
        else if (strcmp(key, "format") == 0) cfg->format = strdup(val);
        else if (strcmp(key, "count") == 0) cfg->count = atoi(val);
        else if (strcmp(key, "verbose") == 0) cfg->verbose = atoi(val);
    }
    fclose(fp);

    // Tarkistus: vähintään sukunimipolku oltava
    if (!cfg->lastDataPaths) {
        fprintf(stderr, "Config missing lastDataPaths!\n");
        free_config(cfg);
        return NULL;
    }

    return cfg;
}

void free_config(Config *cfg) {
    if (!cfg) return;
    free(cfg->firstMDataPaths);
    free(cfg->secondMDataPaths);
    free(cfg->firstFDataPaths);
    free(cfg->secondFDataPaths);
    free(cfg->lastDataPaths);
    free(cfg->output_file);
    free(cfg->format);
    free(cfg);
}

Name *load_names(const char *filename, int *count) {
    if (!filename) {
        fprintf(stderr, "No input file specified!\n");
        return NULL;
    }
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return NULL;
    }

    int capacity = 16;
    *count = 0;
    int skipped = 0;
    Name *names = malloc(capacity * sizeof(Name));
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        // Pilkotaan rivin osat
        char *first  = strtok(line, ",");
        char *second = strtok(NULL, ",");
        char *last   = strtok(NULL, ",\n");

        // Jos rivillä ei ole edes yhtä nimeä → ohitetaan
        if (!first || strlen(first) == 0) {
            skipped++;
            continue;
        }

        if (*count >= capacity) {
            capacity *= 2;
            Name *tmp = realloc(names, capacity * sizeof(Name));
            if (!tmp) {
                perror("realloc");
                free(names);
                fclose(fp);
                return NULL;
            }
            names = tmp;
        }

        // Täytetään puuttuvat kentät tyhjällä merkkijonolla
        names[*count].first  = strdup(first);
        names[*count].second = (second && strlen(second) > 0) ? strdup(second) : strdup("");
        names[*count].last   = (last   && strlen(last)   > 0) ? strdup(last)   : strdup("");
        (*count)++;
    }
    fclose(fp);

    if (skipped > 0) {
        fprintf(stderr, "Skipped %d invalid rows in %s\n", skipped, filename);
    }

    return names;
}

void free_names(Name *names, int count) {
    for (int i = 0; i < count; i++) {
        free(names[i].first);
        free(names[i].second);
        free(names[i].last);
    }
    free(names);
}
