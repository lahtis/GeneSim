#ifndef ARGS_H
#define ARGS_H
#include "cJSON.h"
struct cJSON;

// Output-mode
typedef enum {
    OUTPUT_PLAIN,
    OUTPUT_CSV,
    OUTPUT_JSON
} OutputMode;

// Sexual
typedef enum {
    MALE,
    FEMALE,
    RANDOM_GENDER
} Gender;

// Parameter structure
typedef struct {
    int period;
    int year;
    int count;
    int use_weights;
    int verbose;
    int help;
    int version;
    int list_periods;
    int lang_en;
    int couple_mode;
    int family_mode;
    int gender;
    int show_age;
    int seed;
    int middle_chance;
    int max_middle_names;
    char forced_surname[64];
    int output_mode;
    int output_mode_file;        // 0 = append, 1 = overwrite
    char output_filename[256];   // Tiedostonimi tallennusta varten
    int maiden_name;
    int shared_surname;
    int force_manual;
    int log;
} Args;

// Functions
void print_dynamic_ohjesaanto(cJSON *root, Args *args);
void parse_args(int argc, char *argv[], Args *args);
void print_help(void);
void print_version(void);

#endif /* ARGS_H */
