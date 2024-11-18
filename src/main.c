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
#include "debug.h"
#include "label.h"
#include "utils.h"

#define KEYBOARD "/dev/input/event4"

bool keys[KEY_MAX] = {0};
bool prevKeys[KEY_MAX] = {0};
bool in_execution = false;
bool repeat = false;
int fd = 0;

typedef struct{
    int fd;
    inst_head* instructions;
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
    if(fd < 0){
        perror("Failed to create macro virtual device");
        exit(1);
    }
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
    emit(fd, EV_KEY, key, state-1);
    emit(fd, EV_SYN, SYN_REPORT, 0);
}

void readKeys(int fd){
  struct input_event event;
  if(read(fd, &event, sizeof(struct input_event)) == -1){
    return;
  }
  
  if(event.type == EV_KEY){
    keys[event.code] = event.value > 0 ? 1 : 0;
  }
}

void switchInstructions(char file[static 1]);

bool isPressed(int key) {
  return (!keys[key]) && (prevKeys[key]);
}

bool isHold(int key) {
  return (prevKeys[key] && keys[key] || keys[key]);
}

void hotkeys(void* argv){
  if(isPressed(KEY_Q) && isHold(KEY_LEFTCTRL)){
    system("notify-send \"vmacro\" \"Quiting...\"");
    exit(0);
  } 
  else
  if(isPressed(KEY_KPPLUS) && !in_execution){
    in_execution = true;
    system("notify-send \"vmacro\" \"Playing macro...\"");
  }
  else
  if(isPressed(KEY_KPPLUS) && in_execution){
    in_execution = false;
    for(int i = 0; i <= KEY_MAX; i++)
      keyEvent(fd, i, UP);
    system("notify-send \"vmacro\" \"Macro execution is paused\"");
  }
  else
  if(isPressed(KEY_R) && isHold(KEY_LEFTCTRL) && !repeat){
    repeat = true;
    system("notify-send \"vmacro\" \"Macro is set to repeat\"");
  }
  else
  if(isPressed(KEY_R) && isHold(KEY_LEFTALT)) {
    if(in_execution) {
      system("notify-send \"vmacro\" \"Can reload file only when macro execution is paused\"");
    } else {
      switchInstructions((char*)argv);
      system("notify-send \"vmacro\" \"Succesfully reloaded file. You can play macro after last delay is ended\"");
    }
  }
}

void *eventLoop(void* argv) {
  int fd = open(KEYBOARD, O_RDONLY | O_NONBLOCK);
  if(fd < 0){
    perror("Could not open keyboard");
    exit(1);
  }

  while(msleep(1)) {
    memcpy(prevKeys, keys, sizeof(bool)*255);
    readKeys(fd);
    hotkeys(argv);
  }
}

inst_head *_inst_head = NULL;
instruction_list *instructions = NULL;
instruction_list *iter = NULL;
label *labels = NULL;

void* executeMacro(void* argv){
    int fd = ((mac_arg*)argv)->fd;
    _inst_head = ((mac_arg*)argv)->instructions;
    labels = _inst_head->labels;
    instructions = _inst_head->instructions;
    iter = instructions;
    printInstructions(instructions);
    while(1){
        while (iter != NULL){
            if(!in_execution){
                nsleep(500);
                continue;
            }
            switch(iter->cmd){
                case 0:
                    break;
                case KEYPRESS:
                    keyEvent(fd, iter->val, iter->state);
                    break;
                case DELAY:
                    msleep(iter->val);
                    break;
                case GOTO:
                    iter = getInstruction(labels, iter);
                    continue;
                    break;
                case LABEL:
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

void switchInstructions(char file[static 1]) {
  in_execution = false;
  if (_inst_head == NULL) {
    printf("[ERROR] Trying to reload file with NULL [inst_head]\n");
    exit(1);
  }
  freeinsthead(_inst_head);
  _inst_head = parseFile(file);
  instructions = _inst_head->instructions;
  labels = _inst_head->labels;
  iter = instructions;
  printInstructions(instructions);
}

int main(int argc, char **argv){
    if(argc < 2){
        printf("Usage:\n\t%s [*.vmac]", argv[0]);
        return 0;
    }
    pthread_t thread;
    pthread_t macexec;

    pthread_create(&thread, NULL, eventLoop, argv[1]); 

    fd = createDevice("vmacro");
    inst_head* instructions = parseFile(argv[1]);

    mac_arg args = {fd, instructions};
    pthread_create(&macexec, NULL, executeMacro, &args);

    pthread_join(thread, NULL);

    return 0;
}
