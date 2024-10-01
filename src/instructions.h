#pragma once
#include "utils.h"
#include <linux/uinput.h>

//Definition of key position
#define DOWN 1
#define UP 0

//Posible instructions that macro uses
#define KEYPRESS 1
#define DELAY 2

//Mouse keycodes
#define MOUSE1 BTN_LEFT
#define MOUSE2 BTN_RIGHT
#define MOUSE3 BTN_MIDDLE

instruction_list* parseFile(char *path);