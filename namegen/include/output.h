#ifndef OUTPUT_H
#define OUTPUT_H

#include "loader.h"
#include <stdio.h>

void print_name(const char *format,
                const Name *first,
                const Name *second,
                const Name *last,
                int verbose);

void print_file(FILE *fp,
                const char *format,
                const Name *first,
                const Name *second,
                const Name *last,
                int verbose);

#endif /* OUTPUT_H */
