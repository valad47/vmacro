#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils.h"
#include "instructions.h"

#define BUFSIZE 128

void proccessCode(char *code, instruction_list *node){
    inst_t res = 0;

    memset(node, 0, sizeof(instruction_list));

    char *word = strtok(code, " ");
    while (word != NULL){
        if (strcmp(word, "DOWN") == 0)
            res |= DOWN << STATEBITSHIFT;
        if (strcmp(word, "UP") == 0)
            res |= UP << STATEBITSHIFT;
        if (strcmp(word, "DELAY") == 0)
            res |= DELAY << CMDBITSHIFT;
        if (strcmp(word, "KEYPRESS") == 0)
            res |= KEYPRESS << CMDBITSHIFT;
        inst_t val = strtoul(word, NULL, 10);
        if (val != 0)
            res |= val << VALBITSHIFT;

        word = strtok(NULL, " ");
    }

    node->instruction = res;
}

instruction_list *parseFile(char *path){
    FILE *fd = fopen(path, "r");

    instruction_list *listHead = malloc(sizeof(struct instruction_list));
    instruction_list *last = listHead;

    char buf[BUFSIZE] = {0};
    for (int i = 0;; i++){
        char symbol = getc(fd);
        if (symbol == '\n' || symbol == EOF){
            proccessCode(buf, last);
            if(symbol == EOF)
                break;
            
            last->next = malloc(sizeof(instruction_list));
            last = last->next;
            memset(buf, 0, BUFSIZE);
            i = -1;
            continue;
        }
        printf("%c", symbol);
        buf[i] = symbol;
    }
    printf("\n\n");
    return listHead;
}