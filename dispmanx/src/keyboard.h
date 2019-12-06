#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>

#include "global.h"

typedef struct {
    int fd;
    char keys[255];
} Keyboard;

Keyboard* keyboard_init(Keyboard* k, const char* devicePath);
void keyboard_destroy(Keyboard* keyboard);

void keyboard_process_events(Keyboard* keyboard);
char keyboard_key_is_pressed(Keyboard* keyboard, char key);

#endif // KEYBOARD_H
