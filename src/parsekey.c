#include <linux/uinput.h>
#include <string.h>

int get_keycode(char *str, int offset){
    #define CASE(s)\
        if(strcmp(str+offset, #s+offset) == 0)\
            return s;

    CASE(KEY_A)
    CASE(KEY_S)
    CASE(KEY_D)
    CASE(KEY_F)
    CASE(KEY_G)
    return 0;
}