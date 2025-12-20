#include "output.h"
#include <stdio.h>
#include <string.h>

// Helper function that handles the actual writing to any stream
void write_formatted_name(FILE *fp, const char *format, const Name *f, const Name *s, const Name *l) {
    if (strcmp(format, "csv") == 0) {
        fprintf(fp, "%s,%s,%s\n", f->first, s->second, l->last);
    } else if (strcmp(format, "json") == 0) {
        fprintf(fp, "{\"first\":\"%s\",\"second\":\"%s\",\"last\":\"%s\"}\n",
                f->first, s->second, l->last);
    } else {
        // default "text"
        fprintf(fp, "%s %s %s\n", f->first, s->second, l->last);
    }
}

void print_name(const char *format, const Name *first, const Name *second, const Name *last, int verbose) {
    write_formatted_name(stdout, format, first, second, last);

    if (verbose) {
        fprintf(stderr, "[DEBUG] Printed to stdout in format: %s\n", format);
    }
}

void print_file(FILE *fp, const char *format, const Name *first, const Name *second, const Name *last, int verbose) {
    if (fp == NULL) return;

    write_formatted_name(fp, format, first, second, last);

    if (verbose) {
        fprintf(stderr, "[DEBUG] Printed to file in format: %s\n", format);
    }
}
