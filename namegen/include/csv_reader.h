#ifndef CSV_READER_H
#define CSV_READER_H

#include "args.h"


int load_names(const char *filename, char ***names, int *count, const Args *args);

#endif /* CSV_READER_H */
