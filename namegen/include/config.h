#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ENTRIES 50

// Apurakenne raakadatalla lukemiseen (avain-arvo -parit)
typedef struct {
    char key[64];
    char value[256];
} ConfigEntry;

// Varsinainen konfiguraatiostruct, jota ohjelma k‰ytt‰‰
typedef struct {
    // Maa ja kieli
    char *locale;               // "FI"
    char *country;              // "Finland"

    // Polku master-tiedostoon
    char *master_config_path;   // "master_config.json"

    // Simulaation tila
    char *default_region;       // "yleinen_suomi"
    int year;
    int max_periods;

    // Generointiasetukset
    int count;                  // 50
    char *output_file;          // "output.txt"
    char *output_format;        // "text"

    // Tekniset kytkimet
    int verbose;                // 0 tai 1
    int use_historical_logic;   // 0 tai 1
    int deterministic_seed;     // 0 tai 1
} Config;

// Funktiot config.conf-tiedoston k‰sittelyyn
int read_config(const char *filename, ConfigEntry *entries, int max_entries);
const char *find_config(ConfigEntry *entries, int n, const char *key);

// Muistin vapautus (t‰rke‰‰, koska k‰yt‰mme strdup-funktiota)
void free_config(Config *cfg);

#endif /* CONFIG_H */
