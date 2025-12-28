#ifndef LOGGER_H
#define LOGGER_H

#include "args.h"

// Tallentaa generaation tiedot namegen.log -tiedostoon
void log_generation_result(const Args *args, unsigned int seed, int config_year, const char *log_text);

#endif
