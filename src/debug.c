#include <stdio.h>
#include <time.h>
#include "debug.h"
#include "instructions.h"

void printInstructions(instruction_list *list){
    printf("\tState\tCmd\tValue\n");
    while(list!=NULL) {
        char *cmd = NULL, *state = NULL;
        #define CASE(s, b)\
            case s:\
                b = #s;\
                break;

        switch (list->cmd) {
            CASE(KEYPRESS, cmd)
            CASE(DELAY, cmd)
        }
        switch (list->state) {
            CASE(UP, state)
            CASE(DOWN, state)
        }

        printf("\t%s\t%s\t%d\n", state, cmd, list->val);
        list = list->next;
    }
    printf("-----------------------------------\n");
}