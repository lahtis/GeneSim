#ifndef OUTPUT_H
#define OUTPUT_H

#include "loader.h"
#include <stdio.h>
#define OUTPUT_PLAIN 0
#define OUTPUT_CSV 1
#define OUTPUT_JSON 2

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

void write_formatted_name_full(FILE *fp, const char *format, const Name *f_ptr, const Name *s_ptr, const Name *l_ptr, const Name *m_ptr, int birth_year);
void print_name(const char *format, const Name *first, const Name *second, const Name *last, int birth_year, int verbose);
void print_file(FILE *fp, const char *format, const Name *first, const Name *second, const Name *last, int birth_year, int verbose);

#endif /* OUTPUT_H */
