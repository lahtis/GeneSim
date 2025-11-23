#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

// Esimerkkidata
const char *first_names[]  = {"Sulo","Jaakko","Oiva","Liisa","Åsa"};
const char *second_names[] = {"Kustaa","Mikael","Rikhard","Helena","Yrjö"};
const char *last_names[]   = {"Laaksonen","Mäkelä","Virtanen","Aaltonen","Östman"};

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, ""); // ääkköset käyttöön

    int count = 1;
    const char *outfile = NULL;
    const char *format = "text";
    int verbose = 0;

    // Lipunparsaus
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--count") == 0 && i+1 < argc) {
            count = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--output-file") == 0 && i+1 < argc) {
            outfile = argv[++i];
        } else if (strcmp(argv[i], "--format") == 0 && i+1 < argc) {
            format = argv[++i];
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        }
    }

    FILE *fp = stdout;
    if (outfile) {
        fp = fopen(outfile, "w");
        if (!fp) {
            perror("fopen");
            return 1;
        }
    }

    srand((unsigned)time(NULL));

    for (int i = 0; i < count; i++) {
        const char *first  = first_names[rand() % 5];
        const char *second = second_names[rand() % 5];
        const char *last   = last_names[rand() % 5];

        if (strcmp(format, "csv") == 0) {
            fprintf(fp, "%s,%s,%s\n", first, second, last);
        } else if (strcmp(format, "json") == 0) {
            fprintf(fp, "{\"first\":\"%s\",\"second\":\"%s\",\"last\":\"%s\"}\n",
                    first, second, last);
        } else {
            fprintf(fp, "%s %s %s\n", first, second, last);
        }

        if (verbose) {
            fprintf(stderr, "DEBUG: generated %s %s %s\n", first, second, last);
        }
    }

    if (outfile) fclose(fp);
    return 0;
}
