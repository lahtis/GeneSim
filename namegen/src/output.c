#include "output.h"
#include <stdio.h>
#include <string.h>

void print_name(const char *format,
                const Name *first,
                const Name *second,
                const Name *last,
                int verbose) {
    if (strcmp(format, "text") == 0) {
        printf("%s %s %s\n", first->first, second->second, last->last);
    } else if (strcmp(format, "csv") == 0) {
        printf("%s,%s,%s\n", first->first, second->second, last->last);
    } else if (strcmp(format, "json") == 0) {
        printf("{\"first\":\"%s\",\"second\":\"%s\",\"last\":\"%s\"}\n",
               first->first, second->second, last->last);
    }

    if (verbose) {
        fprintf(stderr, "Printed name: %s %s %s\n",
                first->first, second->second, last->last);
    }
}

void print_file(FILE *fp,
                const char *format,
                const Name *first,
                const Name *second,
                const Name *last,
                int verbose) {
    if (strcmp(format, "text") == 0) {
        fprintf(fp, "%s %s %s\n", first->first, second->second, last->last);
    } else if (strcmp(format, "csv") == 0) {
        fprintf(fp, "%s,%s,%s\n", first->first, second->second, last->last);
    } else if (strcmp(format, "json") == 0) {
        fprintf(fp, "{\"first\":\"%s\",\"second\":\"%s\",\"last\":\"%s\"}\n",
                first->first, second->second, last->last);
    }

    if (verbose) {
        fprintf(stderr, "Printed name to file: %s %s %s\n",
                first->first, second->second, last->last);
    }
}
