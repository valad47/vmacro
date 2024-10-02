#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils.h"
#include "instructions.h"
#include "parsekey.h"
#include "label.h"

#define BUFSIZE 128

void proccessCode(char *code, instruction_list *node, label *labels){
    memset(node, 0, sizeof(instruction_list));

    #define add_opt(a)\
        if(strcmp(word, #a) == 0)\
            node->state = a
    #define add_cmd(a)\
        if(strcmp(word, #a) == 0)\
            node->cmd = a
    #define add_val(a)\
        if(strcmp(word, #a) == 0)\
            node->val = a

    #define add_if(a)\
        if(strcmp(word, #a) == 0)

    char *word = strtok(code, " ");
    while (word != NULL){
        add_opt(DOWN);
        add_opt(UP);

        add_cmd(DELAY);
        add_cmd(KEYPRESS);

        add_val(MOUSE1);
        add_val(MOUSE2);
        add_val(MOUSE3);
        if(strncmp(word, "KEY_", 4) == 0)
            node->val = get_keycode(word, 4);
        int32_t val = strtoul(word, NULL, 10);
        if (val != 0)
            node->val = val;

        add_if(LABEL){
            node->cmd = LABEL;
            word = strtok(NULL, " ");
            if(word == NULL){
                printf("No label specified\n");
                exit(1);
            }
            char* newWord = malloc(strlen(word)+1);
            strcpy(newWord, word);
            node->val = (int64_t)newWord;
            addLabel(labels, node);
            return;
        }
        add_if(GOTO){
            node->cmd = GOTO;
            if(word == NULL){
                printf("No label specified\n");
                exit(1);
            }
            char* newWord = malloc(strlen(word)+1);
            strcpy(newWord, word);
            node->val = (int64_t)newWord;
        }

        word = strtok(NULL, " ");
    }
    #undef add_opt
    #undef add_cmd
    #undef add_val
    #undef add_if
}

inst_head *parseFile(char *path){
    FILE *fd = fopen(path, "r");
    if (!fd){
        perror("Failed to open file");
        exit(1);
    }
    #define allocate(var)     \
        var = malloc(sizeof(typeof(var))); \
        if(var == NULL){            \
            perror("Failed to allocate memory");\
            exit(1);\
        }

    inst_head *inst_headp;
    allocate(inst_headp);
    allocate(inst_headp->instructions);
    allocate(inst_headp->labels);

    instruction_list *last = inst_headp->instructions;

    char *buf = malloc(BUFSIZE);
    int currentbuff = BUFSIZE;
    for (int i = 0;; i++){
        if(i+1>currentbuff){
            currentbuff += BUFSIZE;
            buf = realloc(buf, currentbuff);
            printf("New buffer size: %d\n", currentbuff);
        }
        char symbol = getc(fd);
        if (symbol == '\n' || symbol == EOF){
            proccessCode(buf, last, inst_headp->labels);
            if(symbol == EOF)
                break;
            
            allocate(last->next);
            last = last->next;
            buf = realloc(buf, BUFSIZE);
            memset(buf, 0, BUFSIZE);
            currentbuff = BUFSIZE;
            i = -1;
            continue;
        }
        buf[i] = symbol;
    }
    free(buf);
    fclose(fd);
    return inst_headp;
}

void freeinsthead(inst_head *head){
    freeinstlist(head->instructions);
    freelabellist(head->labels);
    free(head);
}