#include <stdlib.h>

const char *first_names[]  = {"Sulo","Jaakko","Oiva","Liisa","Åsa"};
const char *second_names[] = {"Kustaa","Mikael","Rikhard","Helena","Yrjö"};
const char *last_names[]   = {"Laaksonen","Mäkelä","Virtanen","Aaltonen","Östman"};

char *pick_first_name(void) {
    return (char*)first_names[rand() % 5];
}

char *pick_second_name(void) {
    return (char*)second_names[rand() % 5];
}

char *pick_last_name(void) {
    return (char*)last_names[rand() % 5];
}
