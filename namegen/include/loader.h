#ifndef LOADER_H
#define LOADER_H

#include "config.h"
#include "cJSON.h"

// 1. Nimirakenne generointia varten
typedef struct {
    char *first;
    char *second;
    char *last;
} Name;

// 2. Yksitt‰inen rivi CSV:st‰ (nimi + painoarvo)
typedef struct {
    char *name;
    double weight;
} NameEntry;

// 3. NameData: Sis‰lt‰‰ kaikki ladatut resurssit
typedef struct {
    NameEntry *m1; int m1_count; // male_first
    NameEntry *m2; int m2_count; // male_middle
    NameEntry *f1; int f1_count; // female_first
    NameEntry *f2; int f2_count; // female_middle

    // Sukunimet
    NameEntry *l;  int l_count;

    // Ammatit - Aikuiset
    NameEntry *occupations_m;       int occ_m_count;
    NameEntry *occupations_f;       int occ_f_count;

    // Ammatit - Lapset (UUDET)
    NameEntry *occupations_child_m; int occ_cm_count;
    NameEntry *occupations_child_f; int occ_cf_count;

} NameData;

// --- Funktioiden prototyypit ---

// P‰‰funktio: Lataa kaiken JSON-konfiguraation perusteella
NameData* load_all_data_with_config(Config *cfg, int period, int verbose);

// Tiedoston luku merkkijonoksi cJSONia varten
char* read_file_to_string(const char *filename);

// CSV-lataus (HUOM: Lis‰tty max_cols tuki)
NameEntry *load_names(const char *filename, int target_period, int *count, int verbose, int max_cols, NameEntry *old_entries);
// Konfiguraation hallinta
Config* load_config(const char *filename);
cJSON* load_master_config(const char *filename);
void free_config(Config *cfg);
void free_all_data(NameData *nd);

// Apuohjelmat
void list_file_periods(const char *filename);
char *my_strsep(char **stringp, const char *delim);
int is_valid_name(const char *name);

#endif
