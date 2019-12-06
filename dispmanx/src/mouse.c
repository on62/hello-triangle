#include "mouse.h"

Mouse* mouse_init(Mouse* m, const char* devicePath) {
    Mouse* mouse = m ? m : NEW(Mouse, 1);

    memset(mouse, 0, sizeof(Mouse));

    mouse->fd = open(devicePath, O_RDONLY | O_NONBLOCK);
    if (mouse->fd == -1) {
        perror("Mouse: unable to open device.");
        return NULL;
    }

    return mouse;
}

void mouse_destroy(Mouse* mouse) {
    close(mouse->fd);
}

void mouse_process_events(Mouse* mouse) {
    int n;
    struct input_event inputEvent;
    while ((n = read(mouse->fd, (void*)&inputEvent, sizeof(struct input_event))) > 0) {
        if (inputEvent.type == EV_REL) {     
            if (inputEvent.code == 0) {
                mouse->x += inputEvent.value;
            } else {
                mouse->y += inputEvent.value;
            }
        } else if (inputEvent.type == EV_KEY && inputEvent.value != 2) {     
            if (inputEvent.code == BTN_LEFT) {
                if (inputEvent.value) {
                    mouse->buttons |= BUTTON_LEFT;
                } else {
                    mouse->buttons &= ~BUTTON_LEFT;
                }
            } else if (inputEvent.code == BTN_RIGHT) {
                if (inputEvent.value) {
                    mouse->buttons |= BUTTON_RIGHT;
                } else {
                    mouse->buttons &= ~BUTTON_RIGHT;
                }
            }
        }
    }
}

char mouse_state(Mouse* mouse, int* x, int* y) {
    mouse_process_events(mouse);
    if (x) *x = mouse->x;
    if (y) *y = mouse->y;
        
    return mouse->buttons;
}

