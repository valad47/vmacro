#pragma once
#include "utils.h"

//Definition of key position
#define DOWN 1
#define UP 0

//Posible instructions that macro uses
#define KEYPRESS 1
#define DELAY 2

instruction_list* parseFile(char *path);