#include <stdio.h>
#include <time.h>
#include "debug.h"

void printInstructions(instruction_list *list){
#ifdef DEBUG

    printf("\tState\tCmd\tValue\n");
    while(list) {
        printf("\t%d\t%d\t%d\n", list->state, list->cmd, list->val);
        list = list->next;
    }
    printf("-----------------------------------");

#endif
}