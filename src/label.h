#pragma once
#include "utils.h"

struct label {
    char *label;
    struct instruction_list *instruction;
    struct label *next;
};

typedef struct label label;

void freelabellist(label* labels);