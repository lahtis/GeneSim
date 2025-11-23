#include <string.h>
#include "normalize.h"

void normalize_line(char *line) {
    // Poista UTF-8 BOM alusta (0xEF 0xBB 0xBF)
    if ((unsigned char)line[0] == 0xEF &&
        (unsigned char)line[1] == 0xBB &&
        (unsigned char)line[2] == 0xBF) {
        // Siirrä merkit vasemmalle
        int i = 0;
        while (line[i+3] != '\0') {
            line[i] = line[i+3];
            i++;
        }
        line[i] = '\0';
    }

    // Käy läpi kaikki merkit
    for (int i = 0; line[i] != '\0'; i++) {
        unsigned char c = (unsigned char)line[i];
        // Windows-1252 en dash (–) = 0x96 → ASCII '-'
        if (c == 0x96) {
            line[i] = '-';
        }
        // UTF-8 en dash (–) = 0xE2 0x80 0x93
        if (c == 0xE2 && (unsigned char)line[i+1] == 0x80 && (unsigned char)line[i+2] == 0x93) {
            line[i] = '-';
            // siirrä loput merkit vasemmalle
            int j = i+1;
            while (line[j+2] != '\0') {
                line[j] = line[j+2];
                j++;
            }
            line[j] = '\0';
        }
    }
}
