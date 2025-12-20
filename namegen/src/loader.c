#include "loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

    // Check: at least a last name path must be present
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
    if (!filename) return NULL;
    FILE *fp = fopen(filename, "r");
    if (!fp) { perror("fopen"); return NULL; }

    int capacity = 16;
    *count = 0;
    Name *names = malloc(capacity * sizeof(Name));
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        // 1. Clean up the newline and spaces at the end
        line[strcspn(line, "\r\n")] = 0;

        // 2. Skip empty lines or lines that start with a number (such as 1870-29)
        if (strlen(line) == 0 || isdigit(line[0])) {
            continue;
        }

        // 3. Take only the first word (if there's other junk on the line after the comma)
        char *token = strtok(line, ",");
        if (!token) continue;

        if (*count >= capacity) {
            capacity *= 2;
            names = realloc(names, capacity * sizeof(Name));
        }

        // 4. Save the name in all fields to be on the safe side
        names[*count].first  = strdup(token);
        names[*count].second = strdup(token);
        names[*count].last   = strdup(token);

        (*count)++;
    }
    fclose(fp);
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
