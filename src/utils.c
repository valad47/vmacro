#include "utils.h"
#include "instructions.h"
#include <stdlib.h>

void freeinstlist(instruction_list *list){
    instruction_list* prev;
    while (list != NULL){
        prev = list;
        list = list->next;
	if(prev->cmd == LABEL || prev->cmd == GOTO) {
	  free((void*)prev->val);
	}
        free(prev);
    }
}
