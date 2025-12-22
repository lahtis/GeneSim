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

void list_file_periods(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("The file %s could not be opened.\n", filename);
        return;
    }

    char line[1024];
    if (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0; // Siivotaan rivinvaihto

        char *token;
        char *rest = line;
        int idx = 0;

        printf("\nFound periods in file [%s]:\n", filename);
        printf("--------------------------------------------------\n");

        // Go through the header row
        while ((token = strtok_r(rest, ";,", &rest))) {
            printf("Index %d: %s\n", idx, token);
            idx++;
        }
        printf("--------------------------------------------------\n");
    }
    fclose(fp);
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

Name *load_names(const char *filename, int target_period, int *count, int verbose) {
    if (!filename) return NULL;
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        if (verbose) printf("ERROR: Failed to open file: %s\n", filename);
        return NULL;
    }

    int capacity = 100;
    *count = 0;
    Name *names = malloc(capacity * sizeof(Name));
    char line[2048];

    // Ohitetaan otsikkorivi
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        char *ptr = line;
        char *token = NULL;
        char *found_token = NULL;
        int current_col = 0;

        // Käydään sarakkeet läpi
        while ((token = my_strsep(&ptr, ",")) != NULL) {
            // Trimataan välilyönnit heti
            while (isspace((unsigned char)*token)) token++;

            // Jos sarake ei ole tyhjä, otetaan se talteen "varalle"
            if (strlen(token) > 0) {
                found_token = token;
            }

            // Jos päästiin tavoitesarakkeeseen ja se ei ole tyhjä, valitaan se
            if (current_col == target_period && strlen(token) > 0) {
                found_token = token;
                break;
            }
            current_col++;
        }

        // Jos löydettiin jokin nimi (joko tavoitesarakkeesta tai varalta jostain muualta)
        if (found_token && strlen(found_token) > 0 && is_valid_name(found_token)) {
            if (*count >= capacity) {
                capacity *= 2;
                Name *temp = realloc(names, capacity * sizeof(Name));
                if(!temp) break;
                names = temp;
            }
            names[*count].first = strdup(found_token);
            names[*count].second = NULL;
            names[*count].last = NULL;
            (*count)++;
        }
    }

    fclose(fp);
    // DEBUG: Tulostetaan montako nimeä oikeasti saatiin talteen
    if (verbose &&*count > 0) {
        printf("INFO: Loaded %d names from %s\n", *count, filename);
    }
    return names;
}

NameData* load_all_data_with_config(Config *cfg, int target_period, int verbose) {
    if (!cfg) return NULL;

    NameData *nd = calloc(1, sizeof(NameData));
    if (!nd) return NULL;

    // Kaytetaan lataamiseen sinun load_names-funktiota
    nd->m1 = load_names(cfg->firstMDataPaths, target_period, &nd->m1_count, verbose);
    nd->m2 = load_names(cfg->secondMDataPaths, target_period, &nd->m2_count, verbose);
    nd->f1 = load_names(cfg->firstFDataPaths, target_period, &nd->f1_count, verbose);
    nd->f2 = load_names(cfg->secondFDataPaths, target_period, &nd->f2_count, verbose);
    nd->l  = load_names(cfg->lastDataPaths, target_period, &nd->l_count, verbose);

    return nd;
}

void free_all_data(NameData *nd) {
    if (!nd) return;
    free_names(nd->m1, nd->m1_count);
    free_names(nd->m2, nd->m2_count);
    free_names(nd->f1, nd->f1_count);
    free_names(nd->f2, nd->f2_count);
    free_names(nd->l, nd->l_count);
    free(nd);
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
