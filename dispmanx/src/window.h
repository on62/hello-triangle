#ifndef WINDOW_H
#define WINDOW_H

#include <assert.h>

#include "bcm_host.h"

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "global.h"

typedef struct {
   EGLDisplay display;
   EGLContext context;
   EGLSurface surface;
   uint32_t width;
   uint32_t height;
} Window;

Window* window_init(Window* w);

#endif // WINDOW_H
