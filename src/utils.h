#pragma once
#include <stdint.h>

struct instruction_list {
    uint64_t instruction;
    struct instruction_list *next;
};

typedef struct instruction_list instruction_list;