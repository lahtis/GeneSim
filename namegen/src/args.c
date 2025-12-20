#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "args.h"
#include "version.h"

void print_help(void) {
    printf("Usage: namegen [options]\n");
    printf("\n");
    printf("  --period <number>     Select period (1–7, 0=random)\n");
    printf("  --couple              Generate couples\n");
    printf("  --family              Generate families\n");
    printf("  --count <number>      Number of entries\n");
    printf("  --output <mode>       plain, csv, json\n");
    printf("  --help                Show this help\n");
    printf("  --version             Show version\n");
}

void print_version(void) {
    printf("namegen version %s (built %s %s)\n", VERSION_STRING, __DATE__, __TIME__);
}

void parse_args(int argc, char *argv[], Args *args) {
    memset(args, 0, sizeof(Args));
    args->output_mode = OUTPUT_PLAIN;
    args->verbose = 0;
    args->help = 0;
    args->version = 0;
    args->count = 1;
    args->family_mode = 0;
    args->couple_mode = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0) {
            args->verbose = 1;
        } else if (strcmp(argv[i], "--help") == 0) {
            args->help = 1;
        } else if (strcmp(argv[i], "--version") == 0) {
            args->version = 1;
        } else if (strcmp(argv[i], "--period") == 0 && i+1 < argc) {
            args->period = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--couple") == 0) {
            args->couple_mode = 1;
        } else if (strcmp(argv[i], "--family") == 0) {
            args->family_mode = 1;
        } else if (strcmp(argv[i], "--count") == 0 && i+1 < argc) {
            args->count = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--output") == 0 && i+1 < argc) {
            const char *mode = argv[++i];
            if (strcmp(mode, "csv") == 0) args->output_mode = OUTPUT_CSV;
            else if (strcmp(mode, "json") == 0) args->output_mode = OUTPUT_JSON;
        }
    }
}
