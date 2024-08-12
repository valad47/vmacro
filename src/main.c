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

#define KEYBOARD "/dev/input/by-path/platform-i8042-serio-0-event-kbd"

uint8_t keys[255];

int msleep(int ms){
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    select(0, NULL, NULL, NULL, &tv);
    return 1;
}

int nsleep(int ms){
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000000);
    select(0, NULL, NULL, NULL, &tv);
    return 1;
}

void emit(int fd, int type, int code, int val){
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;
    /* timestamp values below are ignored */
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
    ioctl(fd, UI_SET_KEYBIT, KEY_SPACE);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1623;  /* sample vendor */
    usetup.id.product = 0x4701; /* sample product */
    strcpy(usetup.name, devName);

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
    /*
     * On UI_DEV_CREATE the kernel will create the device node for this
     * device. We are inserting a pause here so that userspace has time
     * to detect, initialize the new device, and can start listening to
     * the event, otherwise it will not notice the event we are about
     * to send. This pause is only needed in our example code!
     */
    msleep(100);

    return fd;
}

void* readKeys(void* argv){
    int fd = open(KEYBOARD, O_RDONLY | O_NONBLOCK);
    while(msleep(1)){
        struct input_event event;
        if(read(fd, &event, sizeof(struct input_event)) == -1){
            continue;
        }

        if(event.type == EV_KEY){
            keys[event.code] = event.value > 0 ? 1 : 0;
        }
    }
}

void* doEvent(void* argv){
    while(msleep(1)){
        if(keys[KEY_A] == 1 && keys[KEY_LEFTCTRL] == 1){
            system("notify-send \"vmacro\" \"Quiting...\"");
            _exit(0);
        }
    }
    
}

void deleteDevice(int fd){
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}

void keyEvent(int fd, int key, int state){
    emit(fd, EV_KEY, key, state);
    emit(fd, EV_SYN, SYN_REPORT, 0);
}

int main(int argc, char **argv){
    pthread_t thread;
    pthread_t thread2;

    pthread_create(&thread, NULL, readKeys, NULL);   
    pthread_create(&thread2, NULL, doEvent, NULL); 

    int fd = createDevice("vmacro");
    /* Key press, report the event, send key release, and report again */
    instruction_list* instructions = parseFile(argv[1]);
    while (instructions != NULL){
        printf("%d %d %d\n", instructions->cmd, instructions->val, instructions->state);
        switch(instructions->cmd){
            case KEYPRESS:
                keyEvent(fd, instructions->val, instructions->state);
                break;
            case DELAY:
                msleep(instructions->val);
                break;
            default:
                printf("Unknown instruction\n");
        }

        instructions = instructions->next;
    }
    printf("End execution of macro\n");

    freeinstlist(instructions);
    /*
     * Give userspace some time to read the events before we destroy the
     * device with UI_DEV_DESTOY.
     */
    msleep(1000);

    //pthread_join(thread, NULL);
    //pthread_join(thread2, NULL);

    return 0;
}