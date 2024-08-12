#include "utils.h"
#include <stdlib.h>

void freeinstlist(instruction_list *list){
    instruction_list* prev;
    while (list != NULL){
        prev = list;
        list = list->next;
        free(prev);
    }
}