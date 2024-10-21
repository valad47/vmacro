#pragma once
#include "utils.h"
#include "label.h"
#include <linux/uinput.h>

#define COMMENT '#'

//Definition of key position
typedef enum{
    UP = 1,
    DOWN
} State;

//Posible instructions that macro uses
typedef enum{
    KEYPRESS = 1,
    DELAY,
    LABEL,
    GOTO
} Instruction;
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
