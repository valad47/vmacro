#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "label.h"
#include "instructions.h"
#include "utils.h"

void addLabel(label *labels, instruction_list *instruction){
    if(instruction->cmd != LABEL){
        perror("Provided instruction is not a label");
        exit(1);
    }
    while(labels->next != NULL)
        labels = labels->next;
    
    label *newLabel = malloc(sizeof(label));
    memset(newLabel, 0, sizeof(label));
    newLabel->label = malloc(strlen((char*)instruction->val)+1);
    strcpy(newLabel->label,  (char*)instruction->val);
    newLabel->instruction = instruction;
    labels->next = newLabel;
}

instruction_list* getInstruction(const label *labels, const instruction_list *instruction){
    if(instruction->cmd != GOTO){
        printf("Provided instruction is not a GOTO");
        exit(1);
    }


    while(labels != NULL) {
        if(strcmp(labels->label, (char*)instruction->val) == 0)
            return labels->instruction;

        labels = labels->next;
    }

    printf("No label was found");
    exit(1);
    return NULL;
}

void freelabellist(label *list){
    label *prev;
    while (list != NULL){
        prev = list;
        list = list->next;
        free(prev->label);
        free(prev);
    }
}