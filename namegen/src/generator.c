#include "generator.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "args.h" // Tarvitaan OUTPUT_MODE vakioita varten

// --- APUFUNKTIOT ---

static const char* get_rand(NameEntry *list, int count) {
    if (!list || count <= 0) return "Tuntematon";
    return list[rand() % count].name;
}

int calculate_middle_chance(int period, int user_chance) {
    if (user_chance >= 0) return user_chance;
    if (period <= 0) period = 4;
    return 15 + (period * 11);
}

static void build_middle_names(char *dest, int max_count, int chance, NameEntry *list, int list_count, const char *first_name) {
    dest[0] = '\0';
    if (!list || list_count <= 0) return;
    for (int i = 0; i < max_count; i++) {
        if ((rand() % 100) < chance) {
            const char *tmp_name = get_rand(list, list_count);
            if (tmp_name && strcmp(tmp_name, first_name) != 0 && !strstr(dest, tmp_name)) {
                if (dest[0] != '\0') strcat(dest, " ");
                strcat(dest, tmp_name);
            }
        }
    }
}

static void prepare_person(const Args *args, NameData *data, int gender, int age,
                          Name *f_out, Name *s_out, char *f_b, char *s_b, char *occ_b) {

    // 1. Lasketaan keskinimen todennäköisyys
    int chance = calculate_middle_chance(args->period, args->middle_chance);

    // 2. Valitaan etunimi (get_rand palauttaa const char*)
    const char *chosen_name = (gender == MALE) ?
        get_rand(data->m1, data->m1_count) : get_rand(data->f1, data->f1_count);

    if (chosen_name) {
        strncpy(f_b, chosen_name, 63);
        f_out->first = f_b;
    }

    // 3. Rakennetaan keskinimet
    build_middle_names(s_b, args->max_middle_names, chance,
                       (gender == MALE ? data->m2 : data->f2),
                       (gender == MALE ? data->m2_count : data->f2_count),
                       f_b);

    if (s_b[0] != '\0') s_out->first = s_b;

    // 4. VALITAAN AMMATTI IÄN JA SUKUPUOLEN MUKAAN
    const char *job = "Ei ammattia";
    if (age < 15) {
        // Lapsen ammatit (tämä vastaa lisäämiäsi JSON-kategorioita)
        job = (gender == MALE) ?
            get_rand(data->occupations_child_m, data->occ_cm_count) :
            get_rand(data->occupations_child_f, data->occ_cf_count);
    } else {
        // Aikuisen ammatit
        job = (gender == MALE) ?
            get_rand(data->occupations_m, data->occ_m_count) :
            get_rand(data->occupations_f, data->occ_f_count);
    }

    if (job && occ_b) {
        strncpy(occ_b, job, 63);
    }
}

// Apufunktio oikean formaatti-merkkijonon valintaan
static const char* get_fmt(const Args *args) {
    if (args->output_mode == OUTPUT_JSON) return "json";
    if (args->output_mode == OUTPUT_CSV) return "csv";
    return "plain";
}

// --- GENEROINTIFUNKTIOT ---

void generate_single(const Args *args, const Config *cfg, NameData *data, FILE *out) {
    Name f = {0}, s = {0}, l = {0};
    static char f_b[64], s_b[128], l_b[64], occ_b[64]; // Lisätty occ_b

    memset(f_b, 0, 64); memset(s_b, 0, 128); memset(l_b, 0, 64); memset(occ_b, 0, 64);

    int gender = (args->gender == 2) ? rand() % 2 : args->gender;

    // Arvotaan ikä (esim. 0-80 vuotta)
    int age = rand() % 81;

    // Kutsutaan uutta prepare_personia (huomaa ikä ja ammatti-puskuri)
    prepare_person(args, data, gender, age, &f, &s, f_b, s_b, occ_b);

    // Sukunimen valinta
    const char *ln = get_rand(data->l, data->l_count);
    if (ln) {
        strncpy(l_b, ln, 63);
        l.first = l_b;
    }

    // Tulostus (lisätään ammatti mukaan jos output-funktiosi tukee sitä)
    int yr = (args->show_age) ? (1850 + (args->period * 10)) : 0;
    write_formatted_name_full(out, get_fmt(args), &f, &s, &l, NULL, yr);

    // Jos haluat ammatin näkyviin plain textissä:
    if (args->output_mode == OUTPUT_PLAIN) {
        fprintf(out, " [%s, %d v, Ammatti: %s]\n", (gender == MALE ? "M" : "N"), age, occ_b);
    }
}

void generate_couple(const Args *args, const Config *cfg, NameData *data, FILE *out) {
    /*
    Name m_f={0}, m_s={0}, m_l={0}, f_f={0}, f_s={0}, f_l={0}, f_maid={0};
    static char mf_b[64], ms_b[128], ff_b[64], fs_b[128], fl_b[64], fm_b[64];
    memset(mf_b, 0, 64); memset(ms_b, 0, 128); memset(ff_b, 0, 64);
    memset(fs_b, 0, 128); memset(fl_b, 0, 64); memset(fm_b, 0, 64);

    char main_l[64];
    if (strlen(args->forced_surname) > 0) strcpy(main_l, args->forced_surname);
    else { Name *ln = get_rand(data->l, data->l_count); strcpy(main_l, ln ? ln->first : "Laine"); }

    prepare_person(args, data, MALE, 30, &m_f, &m_s, mf_b, ms_b, m_occ);

    m_l.first = main_l;

    prepare_person(args, data, FEMALE, 28, &f_f, &f_s, ff_b, fs_b, f_occ);

    int use_shared = args->shared_surname || (!args->force_manual && args->period > 4);
    if (use_shared || strlen(args->forced_surname) > 0) {
        f_l.first = main_l;
        if (args->maiden_name) {
            Name *m = get_rand(data->l, data->l_count);
            if (m) { strcpy(fm_b, m->first); f_maid.first = fm_b; }
        }
    } else {
        Name *ln = get_rand(data->l, data->l_count);
        if (ln) { strcpy(fl_b, ln->first); f_l.first = fl_b; }
    }

    int base = 1850 + (args->period * 10);
    int m_yr = (args->show_age) ? base + (rand() % 15) : 0;
    int f_yr = (args->show_age) ? m_yr + (rand() % 10 - 5) : 0;

    // Tulostus
    write_formatted_name_full(out, get_fmt(args), &m_f, &m_s, &m_l, NULL, m_yr);
    if (args->output_mode == OUTPUT_JSON) fprintf(out, ",\n");

    write_formatted_name_full(out, get_fmt(args), &f_f, &f_s, &f_l, &f_maid, f_yr);

    if (args->output_mode == OUTPUT_PLAIN) fprintf(out, "\n");
    */
}

void generate_family(const Args *args, const Config *cfg, NameData *data, FILE *out) {
    /*
    Name m_f={0}, m_s={0}, m_l={0}, f_f={0}, f_s={0}, f_l={0}, f_maid={0};
    static char mf_b[64], ms_b[128], ff_b[64], fs_b[128], fl_b[64], fm_b[64];
    memset(mf_b, 0, 64); memset(ms_b, 0, 128); memset(ff_b, 0, 64);
    memset(fs_b, 0, 128); memset(fl_b, 0, 64); memset(fm_b, 0, 64);

    char main_l[64];
    if (strlen(args->forced_surname) > 0) strcpy(main_l, args->forced_surname);
    else { Name *ln = get_rand(data->l, data->l_count); strcpy(main_l, ln ? ln->first : "Laine"); }

    // Valmistellaan isä ja äiti
    prepare_person(args, data, MALE, &m_f, &m_s, mf_b, ms_b);
    m_l.first = main_l;
    prepare_person(args, data, FEMALE, &f_f, &f_s, ff_b, fs_b);
    f_l.first = main_l;

    if (args->maiden_name) {
        Name *m = get_rand(data->l, data->l_count);
        if (m) { strcpy(fm_b, m->first); f_maid.first = fm_b; }
    }

    int base = 1850 + (args->period * 10);
    int m_yr = (args->show_age) ? base + (rand() % 10) : 0;
    int f_yr = (args->show_age) ? m_yr + (rand() % 6 - 3) : 0;

    // TULOSTUS ALKAA
    write_formatted_name_full(out, get_fmt(args), &m_f, &m_s, &m_l, NULL, m_yr);
    if (args->output_mode == OUTPUT_JSON) fprintf(out, ",\n");

    write_formatted_name_full(out, get_fmt(args), &f_f, &f_s, &f_l, &f_maid, f_yr);

    // Lapset
    int children = (rand() % 4) + 1;
    int child_yr = f_yr + 20 + (rand() % 5);

    for (int i = 0; i < children; i++) {
        // Lisätään pilkku JSON-muodossa ennen jokaista lasta
        if (args->output_mode == OUTPUT_JSON) fprintf(out, ",\n");

        Name c_f={0}, c_s={0}, c_l={0};
        static char cfb[64], csb[128];
        memset(cfb, 0, 64); memset(csb, 0, 128);

        int c_gen = rand() % 2;
        prepare_person(args, data, c_gen, &c_f, &c_s, cfb, csb);
        c_l.first = main_l;

        if (args->output_mode == OUTPUT_PLAIN) fprintf(out, " + Child: ");

        write_formatted_name_full(out, get_fmt(args), &c_f, &c_s, &c_l, NULL, (args->show_age ? child_yr : 0));
        child_yr += (rand() % 4) + 2;
    }

    // Vain plain text tarvitsee loppuun yhden tyhjän rivin erottamaan perheet toisistaan
    if (args->output_mode == OUTPUT_PLAIN) fprintf(out, "\n");
    */
}
