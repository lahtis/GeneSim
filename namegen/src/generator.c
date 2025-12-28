#include "generator.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "args.h" // Tarvitaan OUTPUT_MODE vakioita varten

// --- APUFUNKTIOT ---

#include "generator.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "args.h"

// --- APUFUNKTIOT ---

static const char* get_rand(NameEntry *list, int count, const Config *cfg) {
    if (!list || count <= 0) return "Unknown";

    if (cfg && !cfg->use_weights) {
        return list[rand() % count].name;
    }

    double total_weight = 0;
    for (int i = 0; i < count; i++) total_weight += list[i].weight;

    if (total_weight <= 0) return list[0].name;

    double r = ((double)rand() / (double)RAND_MAX) * total_weight;
    double current_sum = 0;
    for (int i = 0; i < count; i++) {
        current_sum += list[i].weight;
        if (r <= current_sum) return list[i].name;
    }
    return list[count - 1].name;
}

int calculate_middle_chance(int period, int user_chance) {
    if (user_chance >= 0) return user_chance;
    if (period <= 0) period = 4;
    return 15 + (period * 11);
}

void build_middle_names(char *dest, size_t dest_size, NameData *data, const Config *cfg, int is_male, int active_chance, SocialContext sc) {
    dest[0] = '\0';

    // 1. Säädetään todennäköisyyttä säädyn mukaan
    // (Arvot haetaan myöhemmin JSONista, tässä käytetään sc-rakenteen tietoa)
    int final_chance = active_chance;
    if (sc.is_elite) {
        final_chance += 30; // Säätyläisillä lähes aina toinen nimi
    } else {
        // Ennen 1900-lukua työläisillä oli harvoin toisia nimiä
        if (cfg->current_period < 4) final_chance -= 15;
    }

    if (final_chance > 100) final_chance = 100;
    if (final_chance < 0) final_chance = 0;

    if ((rand() % 100) >= final_chance) {
        return;
    }

    // 2. Päätetään määrä (1 - 3 nimeä) säädyn ja periodin mukaan
    int count_to_gen = 1;
    int roll = rand() % 100;

    if (sc.is_elite) {
        // Elite: Suositaan 2-3 nimeä
        if (roll < 40) count_to_gen = 3;
        else if (roll < 90) count_to_gen = 2;
        else count_to_gen = 1;
    } else {
        // Labor: Pääosin 1, uudemmilla aikakausilla joskus 2
        if (cfg->current_period >= 5 && roll < 20) {
            count_to_gen = 2;
        } else {
            count_to_gen = 1;
        }
    }

    // Varmistetaan, ettei ylitetä Configin asettamaa kovaa rajaa
    if (count_to_gen > cfg->max_middle_names) {
        count_to_gen = cfg->max_middle_names;
    }

    // 3. Valitaan oikea allas
    NameEntry *pool = is_male ? data->m2 : data->f2;
    int pool_count = is_male ? data->m2_count : data->f2_count;
    if (pool_count == 0) return;

    // 4. Rakennetaan nimijono
    char temp_buf[256] = "";
    for (int i = 0; i < count_to_gen; i++) {
        int idx = rand() % pool_count;
        if (i > 0) strncat(temp_buf, " ", sizeof(temp_buf) - strlen(temp_buf) - 1);
        strncat(temp_buf, pool[idx].name, sizeof(temp_buf) - strlen(temp_buf) - 1);
    }

    strncpy(dest, temp_buf, dest_size - 1);
}

void prepare_person(const Args *args, const Config *cfg, NameData *data, int gender, int age, Name *f, Name *s, char *fb, char *sb, char *occ) {
    // 1. ETUNIMI (Pidetään ennallaan)
    if (gender == 0) {
        const char *name = get_rand(data->m1, data->m1_count, cfg);
        if (name) strncpy(fb, name, 63);
    } else {
        const char *name = get_rand(data->f1, data->f1_count, cfg);
        if (name) strncpy(fb, name, 63);
    }
    f->first = fb;

    // 2. KESKINIMI (Pidetään ennallaan, build_middle_names hoitaa loput)
    s->first = NULL;

    // 3. AMMATTI - TÄSSÄ UUSI LOGIIKKA
    const char *job = NULL;
    if (age < 7) {
        strncpy(occ, "Lapsi", 63); // Aina englanniksi kuten pyydetty CHILD (LAPSI)
    } else if (age < 15) {
        job = (gender == 0) ?
            get_rand(data->occupations_child_m, data->occ_cm_count, cfg) :
            get_rand(data->occupations_child_f, data->occ_cf_count, cfg);
        if (job) strncpy(occ, job, 63);
    } else {
        job = (gender == 0) ?
            get_rand(data->occupations_m, data->occ_m_count, cfg) :
            get_rand(data->occupations_f, data->occ_f_count, cfg);
        if (job) strncpy(occ, job, 63);
    }
}

static const char* get_fmt(const Args *args) {
    if (args->output_mode == OUTPUT_JSON) return "json";
    if (args->output_mode == OUTPUT_CSV) return "csv";
    if (args->verbose) return "plain_verbose";
    return "plain";
}

SocialContext resolve_social_context(NameData *data, const Config *cfg, const char *focc, const char *mocc) {
    SocialContext ctx = {0};

    // 1. TURVALLINEN ALUSTUS: Oletuksena Common-lista
    ctx.ln_list = data->l;
    ctx.ln_count = data->l_count;
    ctx.is_elite = 0;
    ctx.middle_name_bonus = 0;
    ctx.max_mid = 1;
    ctx.avg_mid = 0.5;

    // Jos JSONia ei ole tai se on viallinen, palautetaan oletukset
    if (!cfg || !cfg->social_logic_root) return ctx;

    // 2. PORRAS-TARKISTUKSET
    cJSON *locales = cJSON_GetObjectItem(cfg->social_logic_root, "data_locale");
    if (!locales) return ctx;

    cJSON *fi = cJSON_GetObjectItem(locales, "fi-FI");
    if (!fi) return ctx;

    cJSON *periods = cJSON_GetObjectItem(fi, "periods");
    if (!periods) return ctx;

    // KÄYTETÄÄN PAIKALLISTA MUUTTUJAA, ei muuteta cfg-oliota
    int p_size = cJSON_GetArraySize(periods);
    int safe_idx = cfg->current_period_idx;

    // Tarkistetaan indeksin rajat paikallisesti
    if (safe_idx < 0) safe_idx = 0;
    if (safe_idx >= p_size) safe_idx = p_size - 1;

    // Jos taulukko on tyhjä, poistutaan
    if (p_size == 0) return ctx;

    cJSON *period_obj = cJSON_GetArrayItem(periods, safe_idx);
    if (!period_obj) return ctx;

    cJSON *classes = cJSON_GetObjectItem(period_obj, "social_classes");
    if (!classes) return ctx;

    cJSON *elite_cfg = cJSON_GetObjectItem(classes, "elite");
    cJSON *labor_cfg = cJSON_GetObjectItem(classes, "labor");
    if (!elite_cfg || !labor_cfg) return ctx;

    // 3. TUNNISTUS
    cJSON *keywords = cJSON_GetObjectItem(elite_cfg, "keywords");
    if (keywords) {
        int kw_count = cJSON_GetArraySize(keywords);
        for (int i = 0; i < kw_count; i++) {
            cJSON *kw_item = cJSON_GetArrayItem(keywords, i);
            if (kw_item && kw_item->valuestring) {
                if (focc && strstr(focc, kw_item->valuestring)) {
                    ctx.is_elite = 1;
                    break;
                }
            }
        }
    }

    // 4. VALINTA: Sukunimiallas ja parametrit
    if (ctx.is_elite && data->l_elite_count > 0) {
        ctx.ln_list = data->l_elite;
        ctx.ln_count = data->l_elite_count;
    }

    cJSON *target = ctx.is_elite ? elite_cfg : labor_cfg;
    if (target) {
        cJSON *mb = cJSON_GetObjectItem(target, "middle_name_chance_bonus");
        cJSON *mm = cJSON_GetObjectItem(target, "max_middle_names");
        cJSON *am = cJSON_GetObjectItem(target, "avg_middle_names");

        if (mb) ctx.middle_name_bonus = mb->valueint;
        if (mm) ctx.max_mid = mm->valueint;
        if (am) ctx.avg_mid = am->valuedouble;
    }

    return ctx;
}

// --- GENEROINTIFUNKTIOT ---

void generate_single(const Args *args, const Config *cfg, NameData *nd, FILE *out) {
    char fb[128], sb[256], occ[256];
    Name f={0}, s={0}, l={0};

    // Alustetaan occ tyhjäksi varmuuden vuoksi
    memset(occ, 0, 256);

    int age = 20 + (rand() % 40);
    int gender = (args->gender == 1) ? 1 : 0;

    // TÄMÄ KUTSU TÄYTTÄÄ 'occ' PUSKURIN
    prepare_person(args, cfg, nd, gender, age, &f, &s, fb, sb, occ);

    const char *ln = get_rand(nd->l, nd->l_count, cfg);
    l.first = (char*)(ln ? ln : "Sukunimi");

    // LÄHETETÄÄN occ JA ROOLI -1
    write_formatted_name_full(stdout, NULL, &f, &s, &l, NULL, (cfg->year - age), occ, -1, args);
}

void generate_couple(const Args *args, const Config *cfg, NameData *data, FILE *out) {
    // Nimien puskurit
    char hfb[128], hsb[256], hocc[128], hlb[128];
    char wfb[128], wsb[256], wocc[128], wlb[128];

    // Rakenteet (Nämä ovat ne, joita käytetään tulostusfunktiossa)
    Name hf={0}, hs={0}, hl={0}, wf={0}, ws={0}, wl={0};

    int h_age = 18 + (rand() % 40);
    int w_age = h_age + (rand() % 7) - 2;

    // 1. MIES (Husband)
    prepare_person(args, cfg, data, 0, h_age, &hf, &hs, hfb, hsb, hocc);
    SocialContext sc_h = resolve_social_context(data, cfg, hocc, NULL);
    int chance_h = calculate_middle_chance(cfg->current_period, cfg->middle_chance);
    build_middle_names(hsb, 256, data, cfg, 0, chance_h, sc_h);
    if (strlen(hsb) > 0) hs.first = hsb;

    const char *sn_h = get_rand(sc_h.ln_list, sc_h.ln_count, cfg);
    strncpy(hlb, sn_h ? sn_h : "Sukunimi", 127);
    hl.first = hlb;

    // 2. NAINEN (Wife)
    prepare_person(args, cfg, data, 1, w_age, &wf, &ws, wfb, wsb, wocc); // Käytä wocc (ei w_occ)
    SocialContext sc_w = resolve_social_context(data, cfg, wocc, NULL);
    int chance_w = calculate_middle_chance(cfg->current_period, cfg->middle_chance);
    build_middle_names(wsb, 256, data, cfg, 0, chance_w, sc_w); // 0 koska is_male=0 naiselle
    if (strlen(wsb) > 0) ws.first = wsb;

    // Vaimo saa miehen sukunimen, wl toimii tyttönimenä
    const char *sn_w = get_rand(sc_w.ln_list, sc_w.ln_count, cfg);
    strncpy(wlb, sn_w ? sn_w : "Tyttönimi", 127);
    wl.first = wlb; // Tallennetaan tyttönimi wl-rakenteeseen

    // TULOSTUS: Käytetään &hf, &hs, &hl jne.
    // Mies
    write_formatted_name_full(stdout, NULL, &hf, &hs, &hl, NULL, (cfg->year - h_age), hocc, 0, args);

    // Vaimo: annetaan hl (miehen sukunimi) last-parametriksi ja wl (tyttönimi) maiden-parametriksi
    write_formatted_name_full(stdout, NULL, &wf, &ws, &hl, &wl, (cfg->year - w_age), wocc, 1, args);

    if (args->output_mode == 0) fprintf(out, "\n");
}

void generate_family(const Args *args, const Config *cfg, NameData *data, FILE *out) {
    Name ff={0}, fs={0}, mf={0}, ms={0}, fl={0}, fmaid={0};
    char ffb[128], fsb[256], focc[256], mlb[128];
    char mfb[128], msb[256], mocc[256], maidb[128];

    memset(ffb, 0, 128); memset(fsb, 0, 256); memset(focc, 0, 256); memset(mlb, 0, 128);
    memset(mfb, 0, 128); memset(msb, 0, 256); memset(mocc, 0, 256); memset(maidb, 0, 128);

    int f_age = 25 + rand() % 25;
    int m_age = f_age + (rand() % 7 - 3);

    // 1. ISÄ
    prepare_person(args, cfg, data, 0, f_age, &ff, &fs, ffb, fsb, focc);
    SocialContext sc = resolve_social_context(data, cfg, focc, NULL);
    int chance = calculate_middle_chance(cfg->current_period, cfg->middle_chance);
    build_middle_names(fsb, 256, data, cfg, 0, chance, sc);
    if (strlen(fsb) > 0) fs.first = fsb;

    const char *family_name = get_rand(sc.ln_list, sc.ln_count, cfg);
    strncpy(mlb, family_name ? family_name : "Sukunimi", 127);
    fl.first = mlb;

    // Isä: ff (first), fs (second), fl (last), f_age (ikä), focc (ammatti)
    write_formatted_name_full(stdout, NULL, &ff, &fs, &fl, NULL, (cfg->year - f_age), focc, 0, args);


    // 2. ÄITI
    prepare_person(args, cfg, data, 1, m_age, &mf, &ms, mfb, msb, mocc);
    build_middle_names(msb, 256, data, cfg, 1, chance, sc);
    if (strlen(msb) > 0) ms.first = msb;

    const char *raw_maid = get_rand(sc.ln_list, sc.ln_count, cfg);
    strncpy(maidb, raw_maid ? raw_maid : "Tyttönimi", 127);
    fmaid.first = maidb;
    mf.last = fl.first; // Vaimo saa isän sukunimen

    // Äiti: mf (first), ms (second), fl (last), fmaid (tyttönimi), m_age (ikä), mocc (ammatti)
    write_formatted_name_full(stdout, NULL, &mf, &ms, &fl, &fmaid, (cfg->year - m_age), mocc, 1, args);

    // 3. LAPSET
    int child_count = (rand() % 5) + 1;
    for (int i = 0; i < child_count; i++) {
        Name cf={0}, cs={0}, cl={0};
        char cfb[128], csb[256], cocc[128];
        int c_gender = rand() % 2;
        int c_age = rand() % 18;

        // Uniikin etunimen arvonta (isä-lapsi esto)
        int is_unique = 0;
        while (!is_unique) {
            prepare_person(args, cfg, data, c_gender, c_age, &cf, &cs, cfb, csb, cocc);
            is_unique = 1;
            if (strcmp(cfb, ffb) == 0) is_unique = 0; // Vertaa isän etunimeen
        }

        cl.first = fl.first;

        // Korjattu build_middle_names kutsu (gender check)
        build_middle_names(csb, 256, data, cfg, (c_gender == 0), chance, sc);
        if (strlen(csb) > 0) cs.first = csb;

        write_formatted_name_full(stdout, NULL, &cf, &cs, &cl, NULL, (cfg->year - c_age), cocc, 2, args);
    }

    if (args->output_mode == OUTPUT_PLAIN) fprintf(out, "\n");
}
