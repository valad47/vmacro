#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils.h"
#include "instructions.h"

#define BUFSIZE 128

void proccessCode(char *code, instruction_list *node){
    memset(node, 0, sizeof(instruction_list));

    char *word = strtok(code, " ");
    while (word != NULL){
        if (strcmp(word, "DOWN") == 0)
            node->state = DOWN;
        if (strcmp(word, "UP") == 0)
            node->state = UP;
        if (strcmp(word, "DELAY") == 0)
            node->cmd = DELAY;
        if (strcmp(word, "KEYPRESS") == 0)
            node->cmd = KEYPRESS;
        int32_t val = strtoul(word, NULL, 10);
        if (val != 0)
            node->val = val;

        word = strtok(NULL, " ");
    }
}

instruction_list *parseFile(char *path){
    FILE *fd = fopen(path, "r");

    instruction_list *listHead = malloc(sizeof(struct instruction_list));
    if(listHead == NULL){
        perror("Failed to allocate memory for list");
        exit(1);
    }
    

    instruction_list *last = listHead;

    char buf[BUFSIZE] = {0};
    for (int i = 0;; i++){
        char symbol = getc(fd);
        if (symbol == '\n' || symbol == EOF){
            proccessCode(buf, last);
            if(symbol == EOF)
                break;
            
            last->next = malloc(sizeof(instruction_list));
            if(last->next == NULL){
                perror("Failed to allocate memory for list");
                exit(1);
            }
            last = last->next;
            memset(buf, 0, BUFSIZE);
            i = -1;
            continue;
        }
        buf[i] = symbol;
    }

    fclose(fd);
    return listHead;
}