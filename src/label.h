#pragma once
#include "utils.h"

struct label {
    char *label;
    struct instruction_list *instruction;
    struct label *next;
};

typedef struct label label;

void addLabel(label labels[static 1], instruction_list instruction[static 1]);
instruction_list* getInstruction(const label labels[static 1], const instruction_list instruction[static 1]);
void freelabellist(label labels[static 1]);