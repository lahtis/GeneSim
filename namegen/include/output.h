#ifndef OUTPUT_H
#define OUTPUT_H

#include "loader.h"
#include <stdio.h>

void write_formatted_name_full(FILE *fp,
                            const char *format,
                            const Name *f,
                            const Name *s,
                            const Name *l,
                            const Name *maiden,
                            int birth_year);

void print_name(const char *format,
                const Name *first,
                const Name *second,
                const Name *last,
                int birth_year,
                int verbose);

void print_file(FILE *fp,
                const char *format,
                const Name *first,
                const Name *second,
                const Name *last,
                int birth_year,
                int verbose);

#endif /* OUTPUT_H */
