#ifndef ARGS_H
#define ARGS_H

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
    int count;              // how many generations
    int period;             // 0=random, 1–7
    int seed;               // seed <number>
    int gender;             // MALE or FEMALE
    int couple_mode;        // 1=couple
    int family_mode;        // 1=family
    int output_mode;        // OUTPUT_PLAIN, OUTPUT_CSV, OUTPUT_JSON
    int verbose;
    int help;
    int version;
    int use_stdout;
    int middle_chance;      // 0-100
    int shared_surname;
    int maiden_name;
    int force_manual;
    char format[256];
    int show_age;
} Args;

// Functions
void parse_args(int argc, char *argv[], Args *args);
void print_help(void);
void print_version(void);

#endif /* ARGS_H */

