// debug.c
#include <stdio.h>
#include "debug.h"

void debug_print_hex(const char *line) {
    for (int i = 0; line[i] != '\0'; i++) {
        printf("%02X ", (unsigned char)line[i]);
    }
    printf(" -> '%s'\n", line);
}

