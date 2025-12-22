#ifndef LOADER_H
#define LOADER_H

typedef struct {
    char *first;
    char *second;
    char *last;
} Name;

typedef struct {
    Name *m1; int m1_count;
    Name *m2; int m2_count;
    Name *f1; int f1_count;
    Name *f2; int f2_count;
    Name *l;  int l_count;
} NameData;

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

// Funktioiden esittelyt (prototyypit)
Config *load_config(const char *filename);
void free_config(Config *cfg);

Name *load_names(const char *filename, int target_period, int *count, int verbose);
void free_names(Name *names, int count);

// LISÄTTY: Nämä puuttuivat ja aiheuttivat "implicit declaration" virheet
NameData* load_all_data_with_config(Config *cfg, int target_period, int verbose);
void free_all_data(NameData *nd);

void list_file_periods(const char *filename);

#endif
