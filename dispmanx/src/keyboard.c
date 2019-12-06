#include "keyboard.h"

Keyboard* keyboard_init(Keyboard* k, const char* devicePath) {
    Keyboard* keyboard = k ? k : NEW(Keyboard, 1);

    memset(keyboard, 0, sizeof(Keyboard));

    keyboard->fd = open(devicePath, O_RDONLY | O_NONBLOCK);
    if (keyboard->fd == -1) {
        perror("Keyboard: unable to open device.");
        return NULL;
    }

    return keyboard;
}

void keyboard_destroy(Keyboard* keyboard) {
    close(keyboard->fd);
}

void keyboard_process_events(Keyboard* keyboard) {
    int n;
    struct input_event inputEvent;
    while ((n = read(keyboard->fd, (void*)&inputEvent, sizeof(struct input_event))) > 0) {
        if (inputEvent.type == EV_KEY && inputEvent.value != 2) {     
            keyboard->keys[inputEvent.code] = inputEvent.value;
        }
    }
}

char keyboard_key_is_pressed(Keyboard* keyboard, char key) {
    keyboard_process_events(keyboard);
    return keyboard->keys[key];
}
