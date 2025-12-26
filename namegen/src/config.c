#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "config.h"

// Apufunktio: etsi polku avaimen perusteella
const char *find_config(ConfigEntry *entries, int n, const char *key) {
    for (int i = 0; i < n; i++) {
        if (strcmp(entries[i].key, key) == 0) return entries[i].value;
    }
    return NULL;
}

int read_config(const char *filename, ConfigEntry entries[], int max_entries) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Failed to open config file");
        return 0;
    }

    int count = 0;
    char line[512];
    while (fgets(line, sizeof(line), f) && count < max_entries) {
        // Ohitetaan kommentit ja tyhjät rivit
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        // Etsitään yhtäsuuruusmerkki
        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0'; // Katkaistaan merkkijono yhtäsuuruusmerkin kohdalta
        char *key_ptr = line;
        char *val_ptr = eq + 1;

        // Trimmataan välilyönnit avaimen alusta ja lopusta
        while(isspace((unsigned char)*key_ptr)) key_ptr++;
        char *key_end = key_ptr + strlen(key_ptr) - 1;
        while(key_end > key_ptr && isspace((unsigned char)*key_end)) {
            *key_end = '\0';
            key_end--;
        }

        // Trimmataan välilyönnit ja rivinvaihdot arvon alusta ja lopusta
        while(isspace((unsigned char)*val_ptr)) val_ptr++;
        char *val_end = val_ptr + strlen(val_ptr) - 1;
        while(val_end > val_ptr && isspace((unsigned char)*val_end)) {
            *val_end = '\0';
            val_end--;
        }

        // Kopioidaan puhdistetut arvot entry-taulukkoon
        strncpy(entries[count].key, key_ptr, sizeof(entries[count].key) - 1);
        strncpy(entries[count].value, val_ptr, sizeof(entries[count].value) - 1);

        count++;
    }

    fclose(f);
    return count;
}
