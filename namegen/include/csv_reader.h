#ifndef CSV_READER_H
#define CSV_READER_H

#include "loader.h" // NameEntryn m‰‰rittely t‰‰lt‰

// P‰ivitetty vastaamaan loader.c:n toteutusta
NameEntry *load_names(const char *filename, int target_period, int *count, int verbose, int max_cols, NameEntry *old_entries, int use_weights);

#endif /* CSV_READER_H */
