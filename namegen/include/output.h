#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "args.h"
#include "generator.h"

void write_formatted_name_full(FILE *fp, const char *format, const Name *f_ptr,
                               const Name *s_ptr, const Name *l_ptr, const Name *m_ptr,
                               int birth_year, const char *occupation, int role,
                               const Args *args);

#endif
