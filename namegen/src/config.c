#include <stdio.h>
#include <string.h>
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
        // ohitetaan kommentit ja tyhjät rivit
        if (line[0] == '#' || line[0] == '\n') continue;

        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';
        strncpy(entries[count].key, line, sizeof(entries[count].key)-1);
        strncpy(entries[count].value, eq+1, sizeof(entries[count].value)-1);

        // poista rivinvaihto lopusta
        entries[count].value[strcspn(entries[count].value, "\r\n")] = '\0';
        count++;
        }

    fclose(f);
    return count;
}
