#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "split_names.h"

// Trimmaa whitespacea alusta ja lopusta
static char *trim(char *s) {
    char *end;

    // ohita alun whitespace
    while (isspace((unsigned char)*s)) s++;

    if (*s == 0) return s; // pelkkää whitespacea

    // etsi lopun whitespace
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;

    // katkaise merkkijono
    end[1] = '\0';
    return s;
}

int split_names(const char *line, char ***names) {
    int count = 0;
    int capacity = 4;
    *names = malloc(capacity * sizeof(char*));

    char *copy = strdup(line);
    char *token = strtok(copy, ",");

    while (token != NULL) {
        char *clean = trim(token);
        if (clean[0] != '\0') {
            if (count >= capacity) {
                capacity *= 2;
                *names = realloc(*names, capacity * sizeof(char*));
            }
            (*names)[count++] = strdup(clean);
            printf("DEBUG: loaded name '%s'\n", clean);
        } else {
            printf("DEBUG: skipped empty field\n");
        }
        token = strtok(NULL, ",");
    }

    free(copy);
    return count;
}
