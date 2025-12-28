#include <stdio.h>
#include <time.h>
#include "logger.h"

void log_generation_result(const Args *args, unsigned int seed, int config_year, const char *log_text) {
    FILE *log_fp = fopen("namegen.log", "a");
    if (log_fp == NULL) {
        perror("LOG ERROR");

        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", t);

    // Kirjataan tiedot selkeässä muodossa
    fprintf(log_fp, "[%s] SEED: %u | YR: %d | CONF: %d | %s\n",
            time_str, seed, args->year, config_year, log_text);

    fflush(log_fp);
    fclose(log_fp);
}
