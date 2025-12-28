#include "output.h"
#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <string.h>
#include "generator.h"

#include <stdio.h>
#include <string.h>
#include "output.h"
#include "logger.h"

extern Args args;           // "Etsi tämä main.c:stä"
extern unsigned int actual_seed;
extern Config *cfg;

void write_formatted_name_full(FILE *fp, const char *format, const Name *f_ptr,
                               const Name *s_ptr, const Name *l_ptr, const Name *m_ptr,
                               int birth_year, const char *occupation, int role,
                               const Args *args) {

    FILE *target = (fp != NULL) ? fp : stdout;

    const char *f_str = (f_ptr && f_ptr->first) ? f_ptr->first : "";
    const char *s_str = (s_ptr && s_ptr->first) ? s_ptr->first : "";
    const char *l_str = (l_ptr && l_ptr->first) ? l_ptr->first : "";
    const char *m_str = (m_ptr && m_ptr->first) ? m_ptr->first : "";
    const char *job   = (occupation) ? occupation : "";

    int mode = (args) ? args->output_mode : 0;

    if (mode == 2 || (format && strstr(format, "json"))) {
        fprintf(target, "{\"role\":%d,\"first\":\"%s\",\"last\":\"%s\",\"birth\":%d}\n",
                role, f_str, l_str, birth_year);
    } else if (mode == 1 || (format && strstr(format, "csv"))) {
        fprintf(target, "%d;%s;%s;%s;%s;%d;%s\n", role, f_str, s_str, l_str, m_str, birth_year, job);
    } else {
        // 1. ROOLI (v2.0.0 Standardi)
        if (role == 0) {
            if (args && args->family_mode) fprintf(target, "Father: ");
            else if (args && args->couple_mode) fprintf(target, "Men:    ");
            else fprintf(target, "Mies:   "); // Oletus jos ei kumpikaan
        }
        else if (role == 1) {
            if (args && args->family_mode) fprintf(target, "Mother: ");
            else if (args && args->couple_mode) fprintf(target, "Women:  ");
            else fprintf(target, "Nainen: "); // Oletus
        }
        else if (role == 2) {
            fprintf(target, "  +Children: ");
        }
        // Rooli -1 ei tulosta mitään (Single mode)

        // 2. NIMI (Siistitty välilyönti)
        fprintf(target, "%s", f_str);
        if (s_str[0] != '\0') fprintf(target, " %s", s_str);
        if (l_str[0] != '\0') fprintf(target, " %s", l_str);

        // Tyttönimi
        if (m_str[0] != '\0') fprintf(target, " (os. %s)", m_str);

        // 3. AMMATTI
        if (job[0] != '\0') {
            fprintf(target, ", %s", job);
        }

        // 4. SYNTYMÄVUOSI
        // Varmista onko muuttuja show_age vai age_logic
        if (args && args->show_age) {
            fprintf(target, " [s. %d]", birth_year);
        }

        fprintf(target, "\n");
    }
}
