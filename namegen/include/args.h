#ifndef ARGS_H
#define ARGS_H

// Versiotieto
#define NAMEGEN_VERSION "0.1.0"

// Sukupuoli
typedef enum {
    OUTPUT_PLAIN,
    OUTPUT_CSV,
    OUTPUT_JSON
} OutputMode;

// Output-moodit
typedef enum {
    MALE,
    FEMALE
} Gender;

// Parametrirakenne
typedef struct {
    int verbose;
    int help;
    int version;
    int period;         // 0=random, 1–7
    int couple_mode;    // 1=pari
    int family_mode;    // 1=perhe
    int count;          // montako generointia
    Gender gender;        // MALE or FEMALE
    OutputMode output_mode; // OUTPUT_PLAIN, OUTPUT_CSV, OUTPUT_JSON
} Args;

// Funktiot
void parse_args(int argc, char *argv[], Args *args);
void print_help(void);
void print_version(void);

#endif /* ARGS_H */

