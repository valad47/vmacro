#include <linux/uinput.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#include "instructions.h"

#define KEYBOARD "/dev/input/event0"

uint8_t keys[255] = {0};
bool in_execution = 0;
uint8_t repeat = 0;
int fd = 0;

typedef struct{
    int fd;
    instruction_list* instructions;
} mac_arg;

int msleep(int ms){
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    select(0, NULL, NULL, NULL, &tv);
    return 1;
}

int nsleep(int ns){
    struct timeval tv;
    tv.tv_sec = ns / 1000;
    tv.tv_usec = (ns % 1000000);
    select(0, NULL, NULL, NULL, &tv);
    return 1;
}

void emit(int fd, int type, int code, int val){
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;

    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    write(fd, &ie, sizeof(ie));
}

int createDevice(const char *devName){
    struct uinput_setup usetup;
    
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    /*
     * The ioctls below will enable the device that is about to be
     * created, to pass key events, in this case the space key.
     */
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    for(int i = 0; i <= KEY_MAX; i++)
        ioctl(fd, UI_SET_KEYBIT, i);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1623;
    usetup.id.product = 0x4701;
    strcpy(usetup.name, devName);

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    msleep(100);

    return fd;
}

void deleteDevice(int fd){
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}

void keyEvent(int fd, int key, int state){
    emit(fd, EV_KEY, key, state);
    emit(fd, EV_SYN, SYN_REPORT, 0);
}

void* readKeys(void* argv){
    int fd = open(KEYBOARD, O_RDONLY | O_NONBLOCK);
    if(fd < 0){
    	perror("Could not open keyboard");
    	exit(1);
    }
    while(msleep(1)){
        struct input_event event;
        if(read(fd, &event, sizeof(struct input_event)) == -1){
            continue;
        }

        if(event.type == EV_KEY){
            keys[event.code] = event.value > 0 ? 1 : 0;
        }
    }
    return NULL;
}

void* doEvent(void* argv){
    while(msleep(1)){
        if(keys[KEY_Q] == 1 && keys[KEY_LEFTCTRL] == 1){
            system("notify-send \"vmacro\" \"Quiting...\"");
            _exit(0);
        }

        if(keys[KEY_LEFTCTRL] == 1 && keys[KEY_LEFTBRACE] && in_execution == 0){
            in_execution = true;
            system("notify-send \"vmacro\" \"Playing macro...\"");
        }

        if(keys[KEY_LEFTCTRL] == 1 && keys[KEY_RIGHTBRACE] && in_execution == 1){
            in_execution = false;
            for(int i = 0; i <= 255; i++)
                keyEvent(fd, i, UP);
            system("notify-send \"vmacro\" \"Macro execution is paused\"");
        }

        if(keys[KEY_R] == 1 && keys[KEY_LEFTCTRL] && repeat == 0){
            repeat = 1;
            system("notify-send \"vmacro\" \"Macro is set to repeat\"");
        }   
    } 
    return NULL;
}

void* executeMacro(void* argv){
    int fd = ((mac_arg*)argv)->fd;
    instruction_list* instructions = ((mac_arg*)argv)->instructions;
    instruction_list* iter = instructions;
    while(1){
        while (iter != NULL){
            if(!in_execution){
                nsleep(500);
                continue;
            }
            switch(iter->cmd){
                case KEYPRESS:
                    keyEvent(fd, iter->val, iter->state);
                    break;
                case DELAY:
                    msleep(iter->val);
                    break;
                default:
                    printf("Unknown instruction\n");
            }

            iter = iter->next;
        }
        if(repeat == 1){
            iter = instructions;
            continue;
        }
        in_execution = false;
        iter = instructions;
        system("notify-send \"vmacro\" \"Macro execution is over\"");
    }
    
    freeinstlist(instructions);
}

int main(int argc, char **argv){
    if(argc < 2){
        printf("Usage:\n\t%s [*.vmac]", argv[0]);
        return 0;
    }
    pthread_t thread;
    pthread_t thread2;
    pthread_t macexec;

    pthread_create(&thread, NULL, readKeys, NULL);   
    pthread_create(&thread2, NULL, doEvent, NULL); 

    fd = createDevice("vmacro");
    /* Key press, report the event, send key release, and report again */
    instruction_list* instructions = parseFile(argv[1]);

    mac_arg args = {fd, instructions};
    pthread_create(&macexec, NULL, executeMacro, &args);

    pthread_join(thread, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
