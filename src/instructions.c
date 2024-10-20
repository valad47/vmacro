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
            word = strtok(NULL, " ");
            if(word == NULL){
                printf("No label specified\n");
                exit(1);
            }
            char* newWord = malloc(strlen(word)+1);
            strcpy(newWord, word);
            node->val = (int64_t)newWord;
            return;
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

    inst_head *inst_headp = malloc(sizeof(inst_head));
    if(inst_headp == NULL){
        perror("Failed to allocate memory");
        exit(1);
    }
    inst_headp->instructions = malloc(sizeof(instruction_list));
    inst_headp->labels = malloc(sizeof(label));
    if(inst_headp->instructions == NULL || inst_headp->labels == NULL){
        perror("Failed to allocate memory");
        exit(1);
    }
    memset(inst_headp->labels, 0, sizeof(label));
    inst_headp->labels->label = malloc(sizeof(char));
    *inst_headp->labels->label = 0;
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
            
            last->next = malloc(sizeof(instruction_list));
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
