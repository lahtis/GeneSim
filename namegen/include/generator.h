#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include "loader.h"
#include "args.h"

void generate_family(const Args *args, const Config *cfg, NameData *data, FILE *out);
void generate_single(const Args *args, const Config *cfg, NameData *data, FILE *out);
void generate_couple(const Args *args, const Config *cfg, NameData *data, FILE *out);

#endif /* GENERATOR_H */
