#include <stdio.h>
#include <stdlib.h>
#include "args.h"
#include "generator.h"
#include <stdlib.h>
#include <time.h>

Name pick_random_name(Name *names, int total) {
    srand((unsigned)time(NULL));
    int idx = rand() % total;
    return names[idx];
}
