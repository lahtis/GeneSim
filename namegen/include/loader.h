#ifndef LOADER_H
#define LOADER_H

#include "config.h" // T‰m‰ tuo Config-tyypin oikeasta paikasta (config.h)

// Name-rakenne on OK pit‰‰ t‰ss‰, jos sit‰ ei ole name.h:ssa
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

// Funktioiden esittelyt (prototyypit)
Config *load_config(const char *filename);
void free_config(Config *cfg);

Name *load_names(const char *filename, int target_period, int *count, int verbose);
void free_names(Name *names, int count);

NameData* load_all_data_with_config(Config *cfg, int target_period, int verbose);
void free_all_data(NameData *nd);

void list_file_periods(const char *filename);

#endif
