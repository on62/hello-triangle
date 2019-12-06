#ifndef MOUSE_H
#define MOUSE_H

#define BUTTON_LEFT  0b01
#define BUTTON_RIGHT 0b10

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>

#include "global.h"

typedef struct {
    int fd;
    char buttons;
    int x;
    int y;
} Mouse;

Mouse* mouse_init(Mouse* m, const char* devicePath);
void mouse_destroy(Mouse* mouse);

void mouse_process_events(Mouse* mouse);
char mouse_state(Mouse* mouse, int* x, int* y);

#endif // MOUSE_H
