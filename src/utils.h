#pragma once
#include <stdint.h>

struct instruction_list {
    uint8_t state;
    uint8_t cmd;
    int64_t val;
    struct instruction_list *next;
};

typedef struct instruction_list instruction_list;

void freeinstlist(instruction_list *list);