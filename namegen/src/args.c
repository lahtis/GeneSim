#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "args.h"
#include "version.h"

void print_help() {
    printf("Usage: namegen [options]\n\n");

    printf("Documentation:\n");
    printf("  --list-periods, -lp [fi|en]  	Show historical eras and usage guidelines.\n\n");

    printf("Generation & Logic:\n");
	printf("  --period <1-11>           	Select historical period (1: oldest, 11: (newest).\n");
    printf("  --year <1880>               	Set the simulation year (e.g., 1880).\n");
	printf("  --weights, -w                 Use historical weights (default).\n");
    printf("  --no-weights                  Uniform distribution (all names equally common).\n");
    printf("  --count <number>          	Number of names/families to generate.\n");
    printf("  --seed <number>           	Set random seed for reproducible results.\n\n");

    printf("Generation Modes:\n");
    printf("  --single                  	Generate single names (default).\n");
    printf("  --couple                  	Generate couples (male & female).\n");
    printf("  --family                  	Generate families (parents & children).\n");
    printf("  --gender male, female, both   Force male, female names or both.\n\n");

    printf("Advanced Options:\n");
    printf("  --middle-chance <0-100>   	Probability for a middle name (%%).\n");
    printf("  --shared-surname          	Force couples to share a surname.\n");
    printf("  --maiden-name             	Show maiden names for wives (nee.)\n");
    printf("  --force-manual            	Disable automatic period-based logic.\n\n");
    printf("  --age, -A                 	Show birth years for generated names.\n\n");

    printf("Output & Formatting:\n");
    printf("  --output <plain|csv|json>     Print format\n");
    printf("  --stdout                  	Force output to console even if file is set.\n");
    printf("  --log, [on|off]  	            Logging of generated data.\n\n");

    // printf("  --format <str>            	Custom text format (e.g. \"%%f %%l\")\n\n");

    printf("System:\n");
    printf("  --verbose, -v                 Show detailed loading and generation info.\n");
    printf("  --help                    	Show this help.\n");
    printf("  --version                 	Show version information.\n");
}

void print_version(void) {
    printf("\n==================================================\n");
    printf("   GENESIM NAME GENERATOR\n");
    printf("   Version: %s \n", VERSION_STRING);
    printf("   Build Date: %s %s\n",  __DATE__, __TIME__);
    printf("   Author:  %s\n", VERSION_AUTHOR);
    printf("   GitHub:  %s\n", VERSION_GITHUB);
    printf("   License: %s\n", VERSION_LICENSE);
    printf("==================================================\n");
    printf("   Developed as part of the GeneSim project.\n");
    printf("   - Multilingual guidelines (FI/EN) support.\n");
    printf("   - Dynamic historical name logic (Scalable Periods)\n");
    printf("==================================================\n\n");
    // Maininta cJSONista ja lisenssistä
    printf("   This software includes the cJSON library (MIT License).\n");
    printf("   cJSON Copyright (c) 2009-2017 Dave Gamble and cJSON contributors.\n");
}


void parse_args(int argc, char *argv[], Args *args) {
    memset(args, 0, sizeof(Args));

    // Oletusarvot
    args->gender = 2; // RANDOM
    args->count = 1;
    args->use_weights = 1;
    args->middle_chance = 30;
    args->output_mode_file = 0; // Append oletuksena

    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--period") == 0) && i + 1 < argc) {
            args->period = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--year") == 0 && i + 1 < argc) {
            args->year = atoi(argv[++i]); // Tallenna myös vuosi!
            args->period = ((args->year - 1860) / 10) + 1;
        } else if (strcmp(argv[i], "--output-file") == 0 && i + 1 < argc) {
            strncpy(args->output_filename, argv[++i], sizeof(args->output_filename) - 1);
        } else if (strcmp(argv[i], "--output-mode") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "ow") == 0) args->output_mode_file = 1;
            else args->output_mode_file = 0;
        } else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--weights") == 0) {
            args->use_weights = 1;
        } else if (strcmp(argv[i], "--no-weights") == 0) {
            args->use_weights = 0;
        } else if (strcmp(argv[i], "--couple") == 0) {
            args->couple_mode = 1;
        } else if (strcmp(argv[i], "--family") == 0) {
            args->family_mode = 1;
        } else if (strcmp(argv[i], "--count") == 0 && i + 1 < argc) {
            args->count = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--log") == 0 || strcmp(argv[i], "-log") == 0) {
            args->log = 1;
            if (i + 1 < argc) {
                if (strcmp(argv[i+1], "on") == 0) { args->log = 1; i++; }
                else if (strcmp(argv[i+1], "off") == 0) { args->log = 0; i++; }
            }
        } else if (strcmp(argv[i], "--list-periods") == 0 || strcmp(argv[i], "-lp") == 0) {
            args->list_periods = 1;
            if (i + 1 < argc) {
                if (strcmp(argv[i+1], "fi") == 0) {
                    args->lang_en = 0; // Suomi
                    i++;
                }
                else if (strcmp(argv[i+1], "en") == 0) {
                    args->lang_en = 1; // Englanti
                    i++;
                }
            }
        } else if (strcmp(argv[i], "--gender") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "female") == 0) args->gender = 1;
            else if (strcmp(argv[i], "male") == 0) args->gender = 0;
            else args->gender = 2;
        } else if (strcmp(argv[i], "--age") == 0 || strcmp(argv[i], "-A") == 0) {
            args->show_age = 1;
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            args->seed = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--maiden-name") == 0) {
            args->maiden_name = 1;
        } else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            args->verbose = 1;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            args->help = 1;
        } else if (strcmp(argv[i], "--version") == 0) {  //
            args->version = 1;
        }
    } // Tämä on parametrisilmukan loppu
}
