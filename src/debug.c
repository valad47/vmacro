#include <stdio.h>
#include <time.h>
#include "debug.h"

void printInstructions(instruction_list *list){
    printf("\tState\tCmd\tValue\n");
    while(list!=NULL) {
        printf("\t%d\t%d\t%d\n", list->state, list->cmd, list->val);
        list = list->next;
    }
    printf("-----------------------------------\n");
}