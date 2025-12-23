#include "output.h"
#include <stdio.h>
#include <string.h>

// Tämä funktio hoitaa muotoilun. Huomaa uudet muuttujanimet, jotta ne eivät törmää tyyppien kanssa.
void write_formatted_name_full(FILE *fp, const char *format, const Name *f_ptr, const Name *s_ptr, const Name *l_ptr, const Name *m_ptr, int birth_year) {
    if (!fp || !f_ptr || !l_ptr) return;

    const char *f_str = f_ptr->first;
    const char *s_str = (s_ptr && s_ptr->first) ? s_ptr->first : "";
    const char *l_str = l_ptr->first;
    const char *m_str = (m_ptr && m_ptr->first) ? m_ptr->first : "";

    if (strcmp(format, "csv") == 0) {
        fprintf(fp, "%s,%s,%s,%s,%d\n", f_str, s_str, l_str, m_str, birth_year);
    }
    else if (strcmp(format, "json") == 0) {
        // Tulostetaan JSON-objekti ilman rivinvaihtoa, jotta generaattori voi lisätä pilkut
        fprintf(fp, "{\"first\":\"%s\",\"middle\":\"%s\",\"last\":\"%s\",\"maiden\":\"%s\",\"birth\":%d}",
                f_str, s_str, l_str, m_str, birth_year);
    }
    else {
        // Tavallinen teksti (plain)
        if (strlen(s_str) > 0) fprintf(fp, "%s %s %s", f_str, s_str, l_str);
        else fprintf(fp, "%s %s", f_str, l_str);

        if (strlen(m_str) > 0) fprintf(fp, " nee %s", m_str);
        if (birth_year > 0) fprintf(fp, " (b. %d)", birth_year);

        fprintf(fp, "\n");
    }
}

void print_name(const char *format, const Name *first, const Name *second, const Name *last, int birth_year, int verbose) {
    write_formatted_name_full(stdout, format, first, second, last, NULL, birth_year);
    if (verbose) {
        fprintf(stderr, "[DEBUG] Output printed in format: %s\n", format);
    }
}

void print_file(FILE *fp, const char *format, const Name *first, const Name *second, const Name *last, int birth_year, int verbose) {
    if (fp == NULL) return;
    write_formatted_name_full(fp, format, first, second, last, NULL, birth_year);
}
