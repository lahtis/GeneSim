#ifndef NAME_DATA_H
#define NAME_DATA_H

#include "config.h"   // jotta ConfigEntry tunnetaan
#include "args.h"

typedef struct {
    char **firstMNames;  int firstMCount;   // miesten etunimet
    char **secondMNames; int secondMCount;  // miesten keskinimet
    char **firstFNames;  int firstFCount;   // naisten etunimet
    char **secondFNames; int secondFCount;  // naisten keskinimet
    char **lastNames;    int lastCount;     // sukunimet
} NameData;

// Funktiot
int init_name_data(NameData *data, const Args *args, ConfigEntry *entries, int n);
int load_lastnames(const char *filename, char ***names, int *count, const Args *args);
void free_name_data(NameData *data);

#endif /* NAME_DATA_H */
