#ifndef LOADER_H
#define LOADER_H

typedef struct {
    char *first;
    char *second;
    char *last;
} Name;

typedef struct {
    char *firstMDataPaths;
    char *secondMDataPaths;
    char *firstFDataPaths;
    char *secondFDataPaths;
    char *lastDataPaths;
    char *output_file;
    int count;
    char *format;
    int verbose;
} Config;



Config *load_config(const char *filename);

Name *load_names(const char *filename, int target_period, int *count);
void free_names(Name *names, int count);
void free_config(Config *cfg);
#endif
