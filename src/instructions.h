#pragma once
#include "utils.h"
#include "label.h"
#include <linux/uinput.h>

//Definition of key position
#define DOWN 1
#define UP 0

//Posible instructions that macro uses
#define KEYPRESS 1
#define DELAY 2
#define LABEL 3
#define GOTO 4

//Mouse keycodes
#define MOUSE1 BTN_LEFT
#define MOUSE2 BTN_RIGHT
#define MOUSE3 BTN_MIDDLE

typedef struct{
    instruction_list *instructions;
    label *labels;
} inst_head;

inst_head* parseFile(char *path);
void freeinsthead(inst_head *head);