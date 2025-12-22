#include "generator.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Apufunktio satunnaisen nimen hakuun
static Name* get_rand(Name *list, int count) {
    if (!list || count <= 0) return NULL;
    return &list[rand() % count];
}

// Historiallinen todennäköisyys kakkosnimelle
int calculate_middle_chance(int period, int user_chance) {
    if (user_chance > 0) return user_chance;
    if (period <= 0) period = 4;
    return 15 + (period * 11);
}

void generate_single(const Args *args, const Config *cfg, NameData *data, FILE *out) {
    Name *f_name = NULL, *s_name = NULL, *l_name = NULL;

    int gender = args->gender;
    if (gender == RANDOM_GENDER) gender = (rand() % 2 == 0) ? MALE : FEMALE;

    int chance = calculate_middle_chance(args->period, args->middle_chance);

    // Arvonta (MALE / FEMALE)
    if (gender == MALE) {
        f_name = get_rand(data->m1, data->m1_count);
        if ((rand() % 100) < chance) {
            do { s_name = get_rand(data->m2, data->m2_count); } while (s_name == f_name);
        }
    } else {
        f_name = get_rand(data->f1, data->f1_count);
        if ((rand() % 100) < chance) {
            do { s_name = get_rand(data->f2, data->f2_count); } while (s_name == f_name);
        }
    }
    l_name = get_rand(data->l, data->l_count);

    const char *mode = (args->output_mode == OUTPUT_CSV) ? "csv" :
                       (args->output_mode == OUTPUT_JSON) ? "json" : "plain";

    // KORJAUS 1: Laske vuosi suoraan tässä (korvaa getRandomYear)
    int year_to_pass = 0;
    if (args->show_age) {
        year_to_pass = 1850 + (args->period * 10) + (rand() % 10);
    }

    // KORJAUS 2: Funktio hoitaa nyt kaiken tulostuksen (nimet + iän + rivinvaihdon)
    write_formatted_name_full(out, mode, f_name, s_name, l_name, NULL, year_to_pass);
}

void generate_couple(const Args *args, const Config *cfg, NameData *data, FILE *out) {
    Name *m_f = NULL, *m_s = NULL, *m_l = NULL;
    Name *f_f = NULL, *f_s = NULL, *f_l = NULL, *f_maiden = NULL;

    int chance = calculate_middle_chance(args->period, args->middle_chance);

    // Määritetään käytetäänkö yhteistä sukunimeä
    int use_shared = args->shared_surname;
    if (!args->force_manual && args->period > 4) use_shared = 1;

    // Mies
    m_f = get_rand(data->m1, data->m1_count);
    if ((rand() % 100) < chance) m_s = get_rand(data->m2, data->m2_count);
    m_l = get_rand(data->l, data->l_count);

    // Nainen
    f_f = get_rand(data->f1, data->f1_count);
    if ((rand() % 100) < chance) f_s = get_rand(data->f2, data->f2_count);

    if (use_shared) {
    f_l = m_l; // Nainen ottaa miehen sukunimen
    if (args->maiden_name) {
        // Arvotaan tyttönimi, kunnes se on eri kuin nykyinen sukunimi
        int attempts = 0;
        do {
            f_maiden = get_rand(data->l, data->l_count);
            attempts++;
        } while (f_maiden == m_l && attempts < 10);
        // attempts-rajoitus estää ikuisen loopin, jos listassa on vain yksi nimi
        }
    }

    const char *mode = (args->output_mode == OUTPUT_CSV) ? "csv" :
                       (args->output_mode == OUTPUT_JSON) ? "json" : "plain";


    int male_year = 0, female_year = 0;
    if (args->show_age) {
        int base = 1850 + (args->period * 10);
        male_year = base + (rand() % 15);
        female_year = male_year + (rand() % 5 - 2);
    }

    // Tulostetaan molemmat
    write_formatted_name_full(out, mode, m_f, m_s, m_l, NULL, male_year);
    write_formatted_name_full(out, mode, f_f, f_s, f_l, f_maiden, female_year);

    if (args->output_mode == OUTPUT_PLAIN) fprintf(out, "\n");
}


void generate_family(const Args *args, const Config *cfg, NameData *data, FILE *out) {
    if (!data || data->m1_count == 0 || data->l_count == 0) {
        fprintf(stderr, "Error: Name data not loaded or files empty.\n");
        return;
    }

    // ... (muuttujien alustukset pysyvät samoina) ...
    int chance = calculate_middle_chance(args->period, args->middle_chance);
    int base_year = 1850 + (args->period * 10);
    int father_birth = base_year + (rand() % 10);
    int mother_birth = father_birth + (rand() % 5 - 2);

    // Arvotaan nimet (kuten aiemmin)
    Name *m_f = get_rand(data->m1, data->m1_count);
    Name *m_s = (data->m2_count > 0 && (rand() % 100) < chance) ? get_rand(data->m2, data->m2_count) : NULL;
    Name *m_l = get_rand(data->l, data->l_count);

    Name *f_f = get_rand(data->f1, data->f1_count);
    Name *f_s = (data->f2_count > 0 && (rand() % 100) < chance) ? get_rand(data->f2, data->f2_count) : NULL;
    Name *f_l = m_l;
    Name *f_maid = get_rand(data->l, data->l_count);
    // (tyttönimen tarkistuslogiikka...)

    const char *mode = (args->output_mode == OUTPUT_CSV) ? "csv" :
                       (args->output_mode == OUTPUT_JSON) ? "json" : "plain";

    if (args->output_mode == OUTPUT_PLAIN) fprintf(out, "--- FAMILY ---\n");

    // LASKETAAN TULOSTETTAVA VUOSI (vain jos show_age on päällä)
    int f_yr = (args->show_age) ? father_birth : 0;
    int m_yr = (args->show_age) ? mother_birth : 0;

    // TULOSTUS: Nyt vuosi menee funktion sisään, ei erillistä fprintf-kutsua!
    write_formatted_name_full(out, mode, m_f, m_s, m_l, NULL, f_yr);
    write_formatted_name_full(out, mode, f_f, f_s, f_l, f_maid, m_yr);

    // LAPSET
    int children = (rand() % 4) + 1;
    for (int i = 0; i < children; i++) {
        Name *c_f = NULL, *c_s = NULL;
        int c_gen = (rand() % 2 == 0) ? 0 : 1;

        if (c_gen == 0) {
            c_f = get_rand(data->m1, data->m1_count);
            if (data->m2_count > 1 && (rand() % 100) < chance) c_s = get_rand(data->m2, data->m2_count);
        } else {
            c_f = get_rand(data->f1, data->f1_count);
            if (data->f2_count > 1 && (rand() % 100) < chance) c_s = get_rand(data->f2, data->f2_count);
        }

        int child_birth = mother_birth + 20 + (i * 3) + (rand() % 5);
        int c_yr = (args->show_age) ? child_birth : 0;

        if (args->output_mode == OUTPUT_PLAIN) fprintf(out, " + Child: ");

        // TÄSSÄ SE MUUTOS: Lisätty c_yr parametriksi
        write_formatted_name_full(out, mode, c_f, c_s, m_l, NULL, c_yr);
    }
    fprintf(out, "\n");
}
