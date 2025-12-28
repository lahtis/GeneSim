#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ENTRIES 50

// 1. Sisällytetään cJSON-otsikko heti alussa, jotta kääntäjä tunnistatyyppi
#include "cJSON.h"
#include "args.h"

// Apurakenne raakadatalla lukemiseen (avain-arvo -parit)
typedef struct {
    char key[64];
    char value[256];
} ConfigEntry;

// Varsinainen konfiguraatiostruct, jota ohjelma käyttää
typedef struct {
    // Maa ja kieli
    char *locale;               // "fi-FI"
    char *country;              // "Finland"

    // Polku master-tiedostoon
    char *master_config_path;   // "namegen_master_config.json"

    // Simulaation tila
    char *default_region;       // "yleinen_suomi"
    int year;
    int max_periods;
    int current_period;         // CSV-sarake (1-11)
    int current_period_idx;     // Indeksi JSON-taulukkoon (0-X)

    // Generointiasetukset
    int count;                  // Esim. 50
    char *output_file;          // "output.txt"
    char *output_format;        // "text", "json", "csv"

    // Tekniset kytkimet ja logiikka
    int verbose;                // Debug-tulosteet
    int use_historical_logic;   // Käytetäänkö tarkkaa aikakausilogiikkaa
    int deterministic_seed;     // Toistettavuus (0 tai seed-luku)

    // Linkkuveitsi-ominaisuudet
    int use_weights;            // 0 = Tasajakauma, 1 = Painotukset CSV:stä
    int middle_chance;
    int max_middle_names;

    // v1.3.4 Social Logic & Regions (VAIN YKSI KERTA TÄSSÄ!)
    cJSON *social_logic_root;   // JSON-puu sosiaaliselle logiikalle
    cJSON *regions_root;        // JSON-puu alueasetuksille

    // v1.3.2 Social Logic - Dynaamiset avainsanat
    char **elite_keywords;      // Dynaaminen merkkijonotaulukko
    int elite_key_count;        // Avainsanojen määrä
    int logging_enabled;
} Config;

// Funktiot config.conf-tiedoston käsittelyyn
int read_config(const char *filename, ConfigEntry *entries, int max_entries);
const char *find_config(ConfigEntry *entries, int n, const char *key);

// Uudet funktiot asetusten hallintaan
void populate_config(Config *cfg, ConfigEntry *entries, int count);
void save_config(const char *filename, Config *cfg);
void load_technical_settings(Config *cfg, const char *filename);

// HUOM: Args-rakenteen eteenpäinviittaus, jotta emme tarvitse args.h:ta tässä
struct Args;
void sync_args_to_config(Args *args, Config *cfg);

// Muistin vapautus
void free_config(Config *cfg);

#endif /* CONFIG_H */
