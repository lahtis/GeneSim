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

    Config *cfg = load_config("config.txt");
    if (!cfg) return 1;

    // Oletusarvot configista
    int count   = cfg->count;
    char *format= cfg->format;
    int verbose = cfg->verbose;
    char *outfile = NULL;

    // Ladataan kaikki nimilistat
    int menFirstCount, menSecondCount, womenFirstCount, womenSecondCount, lastCount;
    Name *menFirst   = load_names(cfg->firstMDataPaths, &menFirstCount);
    Name *menSecond  = load_names(cfg->secondMDataPaths, &menSecondCount);
    Name *womenFirst = load_names(cfg->firstFDataPaths, &womenFirstCount);
    Name *womenSecond= load_names(cfg->secondFDataPaths, &womenSecondCount);
    Name *lastNames  = load_names(cfg->lastDataPaths, &lastCount);

    if (args.help) { print_help(); return 0; }
    if (args.version) { print_version(); return 0; }

    // K‰yd‰‰n komentoriviargumentit l‰pi
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-count") == 0 && i+1 < argc) {
            count = atoi(argv[++i]);
       } else if (strcmp(argv[i], "--output-file") == 0 && i+1 < argc) {
        outfile = argv[++i];   // tallennetaan tiedostonimi muuttujaan
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

    srand((unsigned)time(NULL));

    // Esimerkki: tulostetaan vain count ja format
    fprintf(stderr, "Generating %d names in format %s (verbose=%d)\n",
            count, format, verbose);

    if (!menFirst || !menSecond || !womenFirst || !womenSecond || !lastNames) {
        fprintf(stderr, "Error loading name data files.\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    FILE *fp = stdout;
if (cfg->output_file && strlen(cfg->output_file) > 0) {
    fp = fopen(cfg->output_file, "w");
    if (!fp) { perror("fopen"); return 1; }
}

    const Name *first  = &menFirst[rand() % menFirstCount];
    const Name *second = &menSecond[rand() % menSecondCount];
    const Name *last   = &lastNames[rand() % lastCount];

for (int i = 0; i < cfg->count; i++) {
    // valitaan nimet...
    print_name(cfg->format, first, second, last, cfg->verbose);   // ruudulle
    print_file(fp, cfg->format, first, second, last, cfg->verbose); // tiedostoon
}

if (fp != stdout) fclose(fp);

    // Vapautukset
    free_names(menFirst, menFirstCount);
    free_names(menSecond, menSecondCount);
    free_names(womenFirst, womenFirstCount);
    free_names(womenSecond, womenSecondCount);
    free_names(lastNames, lastCount);
    free_config(cfg);

    free_config(cfg);
    return 0;
}
