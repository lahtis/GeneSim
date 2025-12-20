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


    // Windows-yhteensopiva versio strsep-funktiosta
    char *my_strsep(char **stringp, const char *delim) {
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

    // Apufunktio nimen validointiin (kuten demokoodissasi)
    int is_valid_name(const char *name) {
        if (!name || *name == '\0') return 0;
        for (int i = 0; name[i]; i++) {
            if (!isalpha((unsigned char)name[i]) && !isspace((unsigned char)name[i]) && name[i] != '-') {
                return 0;
            }
        }
        return 1;
    }

Name *load_names(const char *filename, int target_period, int *count) {
    if (!filename) return NULL;
    FILE *fp = fopen(filename, "r");
    if (!fp) { perror("fopen"); return NULL; }

    int capacity = 100;
    *count = 0;
    Name *names = malloc(capacity * sizeof(Name));
    char line[2048];

    // Ohita otsikkorivi
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        char *ptr = line;
        char *token = NULL;
        int current_col = 0;

        while (current_col <= target_period) {
            token = my_strsep(&ptr, ",");
            if (current_col == target_period) break;
            current_col++;
            if (!ptr) { token = NULL; break; }
        }

        if (token) {
            while (isspace((unsigned char)*token)) token++;
            if (strlen(token) > 0 && is_valid_name(token)) {
                if (*count >= capacity) {
                    capacity *= 2;
                    names = realloc(names, capacity * sizeof(Name));
                }
                names[*count].first = strdup(token);
                names[*count].second = strdup(token);
                names[*count].last = strdup(token);
                (*count)++;
            }
        }
    }
    fclose(fp);
    return names;
}

void free_names(Name *names, int count) {
    if (!names) return;
    for (int i = 0; i < count; i++) {
        free(names[i].first);
        free(names[i].second);
        free(names[i].last);
    }
    free(names);
}
