#ifndef LOADER_H
#define LOADER_H

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

typedef struct {
    char *first;
    char *second;
    char *last;
} Name;

Config *load_config(const char *filename);
void free_config(Config *cfg);

Name *load_names(const char *filename, int *count);
void free_names(Name *names, int count);

#endif
