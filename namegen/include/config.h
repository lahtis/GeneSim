#ifndef CONFIG_H
#define CONFIG_H

#define MAX_FILES 10

typedef struct {
    char key[64];
    char value[256];
} ConfigEntry;

typedef struct {
    char *firstMDataPaths;
    char *secondMDataPaths;
    char *firstFDataPaths;
    char *secondFDataPaths;
    char *lastDataPaths;
    char *output_file;
    char *format;
    int count;
    int verbose;
} Config;

int read_config(const char *filename, ConfigEntry *entries, int max_entries);
const char *find_config(ConfigEntry *entries, int n, const char *key);

#endif /* CONFIG_H */
