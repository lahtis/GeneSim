#include "output.h"
#include <stdio.h>
#include <string.h>

// Helper function that handles the actual writing to any stream
void write_formatted_name_full(FILE *fp, const char *format, const Name *f, const Name *s, const Name *l, const Name *maiden, int birth_year) {
    if (!fp || !f || !l) return;

    const char *first = f->first;
    const char *mid   = (s && s->first) ? s->first : "";
    const char *last  = l->first;
    const char *maid  = (maiden && maiden->first) ? maiden->first : "";

    if (strcmp(format, "csv") == 0) {
        // Lisätään vuosi viimeiseksi sarakkeeksi
        if (birth_year > 0)
            fprintf(fp, "%s,%s,%s,%s,%d\n", first, mid, last, maid, birth_year);
        else
            fprintf(fp, "%s,%s,%s,%s,\n", first, mid, last, maid);
    }
    else if (strcmp(format, "json") == 0) {
        // Tulostetaan alku ilman loppusulkeita
        fprintf(fp, "{\"first\":\"%s\",\"middle\":\"%s\",\"last\":\"%s\",\"maiden\":\"%s\"",
                first, mid, last, maid);

        // Jos vuosi on annettu, lisätään se objektin sisään
        if (birth_year > 0) {
            fprintf(fp, ",\"birth\":%d", birth_year);
        }
        fprintf(fp, "}\n"); // Suljetaan vasta nyt
    }
    else {
        // Plain text: "Johannes Alfred Mäkinen nee Heikkinen (b. 1890)"
        if (strlen(mid) > 0) fprintf(fp, "%s %s %s", first, mid, last);
        else fprintf(fp, "%s %s", first, last);

        if (strlen(maid) > 0) fprintf(fp, " nee %s", maid);

        if (birth_year > 0) fprintf(fp, " (b. %d)", birth_year);

        fprintf(fp, "\n");
    }
}

void print_name(const char *format, const Name *first, const Name *second, const Name *last, int birth_year, int verbose) {
    write_formatted_name_full(stdout, format, first, second, last, NULL, birth_year);

    if (verbose) {
        fprintf(stderr, "[DEBUG] Printed to stdout in format: %s\n", format);
    }
}

void print_file(FILE *fp, const char *format, const Name *first, const Name *second, const Name *last, int birth_year, int verbose) {
    if (fp == NULL) return;

    write_formatted_name_full(fp, format, first, second, last, NULL, birth_year);

    if (verbose) {
        fprintf(stderr, "[DEBUG] Printed to file in format: %s\n", format);
    }
}
