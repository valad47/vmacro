#include <stdlib.h>
#include "label.h"

void freelabellist(label *list){
    label *prev;
    while (list != NULL){
        prev = list;
        list = list->next;
        free(prev->label);
        free(prev);
    }
}