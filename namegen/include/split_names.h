#ifndef SPLIT_NAMES_H
#define SPLIT_NAMES_H

// Pilkkoo rivin pilkuilla ja palauttaa nimet taulukossa.
// Ohittaa tyhj‰t kent‰t ja trimmaa whitespacea.
// Palauttaa lukum‰‰r‰n.
int split_names(const char *line, char ***names);

#endif
