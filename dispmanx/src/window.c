#include "window.h"

Window* window_init(Window* w) {
   Window* window = w ? w : NEW(Window, 1);

   // Initialize OpenGL

   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      // EGL_SAMPLES, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };
   
   static const EGLint context_attributes[] = 
   {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
   EGLConfig config;

   bcm_host_init();

   // get an EGL display connection
   window->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(window->display!=EGL_NO_DISPLAY);
   glCheck();

   // initialize the EGL display connection
   result = eglInitialize(window->display, NULL, NULL);
   assert(EGL_FALSE != result);
   glCheck();

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(window->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);
   glCheck();

   // get an appropriate EGL frame buffer configuration
   result = eglBindAPI(EGL_OPENGL_ES_API);
   assert(EGL_FALSE != result);
   glCheck();

   // create an EGL rendering context
   window->context = eglCreateContext(window->display, config, EGL_NO_CONTEXT, context_attributes);
   assert(window->context!=EGL_NO_CONTEXT);
   glCheck();

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &window->width, &window->height);
   assert( success >= 0 );

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = window->width;
   dst_rect.height = window->height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = window->width << 16;
   src_rect.height = window->height << 16;        

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);
      
   nativewindow.element = dispman_element;
   nativewindow.width = window->width;
   nativewindow.height = window->height;
   vc_dispmanx_update_submit_sync( dispman_update );
   glCheck();

   window->surface = eglCreateWindowSurface( window->display, config, &nativewindow, NULL );
   assert(window->surface != EGL_NO_SURFACE);
   glCheck();

   // connect the context to the surface
   result = eglMakeCurrent(window->display, window->surface, window->surface, window->context);
   assert(EGL_FALSE != result);
   glCheck();

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_DEPTH_TEST);

   glCheck();

   glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
   glClear( GL_COLOR_BUFFER_BIT );
   
   glCheck();

   return window;
}