#pragma once
#include "utils.h"

// First bit //

#define DOWN 1
#define UP 0

#define MILISECONDS 0
#define SECONDS 1

// Second bit + 8

#define KEYPRESS 1
#define DELAY 2

// Bit shifts

#define STATEBITSHIFT   0
#define CMDBITSHIFT     1
#define VALBITSHIFT     32

instruction_list* parseFile(char *path);