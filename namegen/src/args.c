#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "version.h"
#include "args.h"

void print_help() {
    printf("Usage: namegen [options]\n\n");

    printf("Documentation:\n");
    printf("  --list-periods, -lp [fi|en]  Show historical eras and usage guidelines.\n");
    printf("\nExample:\n");
    printf("  namegen.exe -lp en\n\n");

    printf("Generation:\n");
    printf("  --period <1-11>           Select historical period (1: 1850, 11: 1960([Latest]))\n");
    printf("  --count <number>          Number of names/families to generate\n");
    printf("  --seed <number>           Set random seed for reproducible results\n\n");

    printf("Generation Modes:\n");
    printf("  --single                  Generate single names (default)\n");
    printf("  --couple                  Generate couples (male & female)\n");
    printf("  --family                  Generate families (parents & children)\n");
    printf("  --male                    Force male names\n");
    printf("  --female                  Force female names\n\n");

    printf("Advanced Options:\n");
    printf("  --middle-chance <0-100>   Probability for a middle name (%%)\n");
    printf("  --shared-surname          Force couples to share a surname\n");
    printf("  --maiden-name             Show maiden names for wives (nee.)\n");
    printf("  --force-manual            Disable automatic period-based logic\n\n");

    printf("Output & Formatting:\n");
    printf("  --output <mode>           Output format: plain, csv, json\n");
    printf("  --stdout                  Force output to console even if file is set\n");
    printf("  --age, -A                 Show birth years for generated names\n\n");
    // printf("  --format <str>            Custom text format (e.g. \"%%f %%l\")\n\n");

    printf("System:\n");
    printf("  --verbose                 Show detailed loading and generation info\n");
    printf("  --help                    Show this help message\n");
    printf("  --version                 Show version information\n");
}

void print_version(void) {
    printf("\n==================================================\n");
    printf("   GENESIM NAME GENERATOR\n");
    printf("   Version: %s \n", VERSION_STRING);
    printf("   Build Date: %s\n",  __DATE__, __TIME__);
    printf("   Author:  %s\n", VERSION_AUTHOR);
    printf("   GitHub:  %s\n", VERSION_GITHUB);
    printf("   License: %s\n", VERSION_LICENSE);
    printf("==================================================\n");
    printf("   Developed as part of the GeneSim project.\n");
    printf("   - Multilingual guidelines (FI/EN) support.\n");
    printf("   - Historical name logic for periods 1-7.\n");
    printf("==================================================\n\n");
}


void parse_args(int argc, char *argv[], Args *args) {
    // 1. Alustus (Nollataan koko 432 tavun lohko kerralla)
    memset(args, 0, sizeof(Args));

    // 2. Oletusarvot
    args->output_mode = OUTPUT_PLAIN;
    args->gender = RANDOM_GENDER;
    args->count = 1;

    // 3. YKSI selke‰ silmukka
    for (int i = 1; i < argc; i++) {

        // --- LISTAUS-KOMENTO ---
        if (strcmp(argv[i], "-lp") == 0 || strcmp(argv[i], "--list-periods") == 0) {
             printf("\n!!! PARSE_ARGS: LIST_PERIODS ASETETTU !!!\n");
            args->list_periods = 1;
            // Katsotaan onko seuraava sana "fi" tai "en"
            if (i + 1 < argc && argv[i+1][0] != '-') {
                if (strcmp(argv[i+1], "en") == 0) {
                    args->lang_en = 1;
                    i++;
                } else if (strcmp(argv[i+1], "fi") == 0) {
                    args->lang_en = 0;
                    i++;
                }
            }
        }
        // --- MUUT VIPU-EHDOT ---
        else if (strcmp(argv[i], "--verbose") == 0) {
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
        } else if (strcmp(argv[i], "--male") == 0) {
            args->gender = MALE;
        } else if (strcmp(argv[i], "--female") == 0) {
            args->gender = FEMALE;
        } else if (strcmp(argv[i], "--age") == 0 || strcmp(argv[i], "-A") == 0) {
            args->show_age = 1;
        } else if (strcmp(argv[i], "--seed") == 0 && i+1 < argc) {
            args->seed = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--middle-chance") == 0 && i + 1 < argc) {
            args->middle_chance = atoi(argv[++i]);
        }
        // Lis‰‰ t‰h‰n muut tarvittavat else ifit samalla kaavalla
    }
}
