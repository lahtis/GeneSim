#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include "loader.h"
#include "args.h"

// 1.3.2 Social Logic - Sosiaalisen statuksen kantaja
typedef struct {
    int s_class;                // 0 = Labor, 1 = Elite
    int is_elite;               // Boolean

    // KORJAUS: K‰ytet‰‰n NameEntry-osoitinta, jotta painotukset s‰ilyv‰t
    NameEntry *ln_list;         // Osoitin NameEntry-taulukkoon (esim. data->l_common)
    int ln_count;               // Valitun taulukon koko

    int middle_name_bonus;      // JSON: middle_name_chance_bonus
    int max_mid;                // JSON: max_middle_names
    double avg_mid;             // JSON: avg_middle_names
} SocialContext;

void generate_family(const Args *args, const Config *cfg, NameData *data, FILE *out);
void generate_single(const Args *args, const Config *cfg, NameData *data, FILE *out);
void generate_couple(const Args *args, const Config *cfg, NameData *data, FILE *out);

// Uusi apufunktio s‰‰dyn ratkaisemiseen
void build_middle_names(char *dest, size_t dest_size, NameData *data, const Config *cfg, int is_male, int active_chance, SocialContext sc);
SocialContext resolve_social_context(NameData *data, const Config *cfg, const char *focc, const char *mocc);

#endif
