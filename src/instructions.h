#pragma once
#include "utils.h"

#define DOWN 1UL
#define UP 0UL

#define KEYPRESS 0UL
#define DELAY 1UL

#define STATEBITSHIFT 0
#define CMDBITSHIFT 1
#define VALBITSHIFT 32

typedef uint64_t inst_t;

instruction_list* parseFile(char *path);