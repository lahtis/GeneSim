#include "loader.h"
#include "output.h"
#include "generator.h"
#include "args.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    Args args;
    parse_args(argc, argv, &args);

    // 1. Check help/version right at the start
    if (args.help) { print_help(); return 0; }
    if (args.version) { print_version(); return 0; }

    // 2. Load the settings
    Config *cfg = load_config("config.txt");
    if (!cfg) return 1;

    // 3. Set default values from config
    int count           = cfg->count;
    char *format        = cfg->format;
    int verbose         = cfg->verbose;
    char *final_outfile = cfg->output_file;
    int period_idx      = 0;

    // 4. Let's go through the arguments that override the config
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--period") == 0 && i + 1 < argc) {
            period_idx = atoi(argv[++i]) - 1;
       } else if (strcmp(argv[i], "--count") == 0 && i+1 < argc) {
            count = atoi(argv[++i]);
       } else if (strcmp(argv[i], "--output-file") == 0 && i+1 < argc) {
            final_outfile = argv[++i];   // save the filename to a variable
        } else if (strcmp(argv[i], "-format") == 0 && i+1 < argc) {
            format = argv[++i];
        } else if (strcmp(argv[i], "-verbose") == 0) {
            verbose = 1;
        } /* else if (args.family_mode) {
            generate_family(&args, &data);
        } else if (args.couple_mode) {
            generate_couple(&args, &data);
        } else {
            generate_single(&args, &data);
        }*/

    }

    if (period_idx < 0 || period_idx > 6) period_idx = 0;

    // 5. Loading data (at this stage only dynamic memory is used)
    int m1C, m2C, w1C, w2C, lastC;
    Name *menFirst    = load_names(cfg->firstMDataPaths, period_idx, &m1C);
    Name *menSecond   = load_names(cfg->secondMDataPaths, period_idx, &m2C);
    Name *womenFirst  = load_names(cfg->firstFDataPaths, period_idx, &w1C);
    Name *womenSecond = load_names(cfg->secondFDataPaths, period_idx, &w2C);
    Name *lastNames   = load_names(cfg->lastDataPaths, 0, &lastC);

    if (!menFirst || !menSecond || !womenFirst || !womenSecond || !lastNames) {
        fprintf(stderr, "Error: Missing data for period %d\n", period_idx + 1);
        return 1;
    }

    srand((unsigned)time(NULL));

    // 6. Opening the file
    FILE *fp = stdout;
    if (final_outfile && strlen(final_outfile) > 0) {
        fp = fopen(final_outfile, "w");
        if (!fp) { perror("fopen"); return 1; }
    }

    if (verbose) {
        fprintf(stderr, "Generating %d names in format %s from period index %d (verbose=%d))\n", count, format, period_idx, verbose);
    }

    // 7. Name generation - RANDOM SELECTION INSIDE LOOP
    for (int i = 0; i < count; i++) {
        // Here you can add logic for selecting gender (e.g., rand() % 2)
        const Name *first = &menFirst[rand() % m1C];
        const Name *second = &menSecond[rand() % m2C];
        const Name *last = &lastNames[rand() % lastC];


        print_name(format, first, second, last, cfg->verbose);   // print in screen
        print_file(fp, format, first, second, last, cfg->verbose); // print in file
    }

    // 8. Final cleaning
    if (fp != stdout) fclose(fp);
    free_names(menFirst, m1C);
    free_names(menSecond, m2C);
    free_names(womenFirst, w1C);
    free_names(womenSecond, w2C);
    free_names(lastNames, lastC);
    free_config(cfg);
    return 0;
}
