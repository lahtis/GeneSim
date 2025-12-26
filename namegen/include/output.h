#ifndef OUTPUT_H
#define OUTPUT_H

#include "loader.h"
#include <stdio.h>

#define OUTPUT_PLAIN 0
#define OUTPUT_CSV 1
#define OUTPUT_JSON 2

// 1. Kirjoittaa suoraan tiedostoon tai stdoutiin (kaikki tiedot mukana)
void write_formatted_name_full(FILE *fp, const char *format,
                               const Name *f_ptr, const Name *s_ptr,
                               const Name *l_ptr, const Name *m_ptr,
                               int birth_year);

// 2. Apufunktio ruudulle tulostukseen (lisätty m_ptr)
void print_name(const char *format, const Name *first, const Name *second,
                const Name *last, const Name *maiden, int birth_year, int verbose);

// 3. Apufunktio tiedostoon tulostukseen (lisätty m_ptr)
void print_file(FILE *fp, const char *format, const Name *first, const Name *second,
                const Name *last, const Name *maiden, int birth_year, int verbose);

#endif /* OUTPUT_H */
