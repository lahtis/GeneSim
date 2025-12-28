#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

// Rakenteessa on tilaa tuleville s‰‰dˆille (esim. social_bias)
typedef struct {
    int year;
    int count;
    int use_weights;
} Config;

void save_config(const char *filename, Config cfg) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Virhe: Tiedostoa %s ei voitu avata kirjoitusta varten!\n", filename);
        return;
    }
    fprintf(fp, "year=%d\n", cfg.year);
    fprintf(fp, "count=%d\n", cfg.count);
    fprintf(fp, "use_weights=%d\n", cfg.use_weights);
    fclose(fp);
    printf("\nAsetukset p‰ivitetty onnistuneesti!\n");
}

int main() {
    Config cfg = {1860, 10, 1}; // Oletusarvot
    char input[10];

    printf("========================================\n");
    printf("     GENESIM CONFIGURATOR v1.0\n");
    printf("========================================\n\n");

    printf("Nykyinen vuosi: %d\n", cfg.year);
    printf("Anna uusi vuosi (paina Enter s‰ilytt‰‰ksesi): ");
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') cfg.year = atoi(input);

    printf("\nNykyinen oletusm‰‰r‰: %d\n", cfg.count);
    printf("Anna uusi m‰‰r‰: ");
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') cfg.count = atoi(input);

    printf("\nPainotukset (1=P‰‰ll‰, 0=Pois): %d\n", cfg.use_weights);
    printf("Valinta: ");
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') cfg.use_weights = atoi(input);

    printf("Lokitus tiedostoon (1=Kyll‰, 0=Ei): ");
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') cfg.logging_enabled = atoi(input);

    save_config("config.conf", cfg);

    printf("\nVoit nyt ajaa namegen.exe:n uusilla asetuksilla.\n");
    return 0;
}
