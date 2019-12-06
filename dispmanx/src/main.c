#include <sys/time.h>
#include <cairo/cairo.h>

#include "keyboard.h"
#include "mouse.h"
#include "window.h"
#include "matrix.h"

#define DEGREES_TO_RADIANS(d)                       (d * 2 * M_PI / 360)

#define TEXT_WIDTH                                                   524
#define TEXT_HEIGHT                                                   32
#define FPS_WIDTH                                                    128
#define FPS_HEIGHT                                                    16
#define BYTES_PER_PIXEL                                                4

#define FRAMES_PER_SECOND                                             60

Window* window;

GLfloat X[] = { 1, 0, 0 };
GLfloat Y[] = { 0, 1, 0 };
GLfloat Z[] = { 0, 0, 1 };
GLfloat rotation[] = {0, 0, 0};

// Triangle

const GLchar* triangle_vshader_source = 
    "attribute vec3 position;"
    "attribute vec3 color;"

    "uniform mat4 model;"
    "uniform mat4 projection;"

    "varying vec3 Color;"

    "void main() {"
    " Color = color;"
    " gl_Position = projection * model * vec4(position, 1.0);"
    "}";

const GLchar* triangle_fshader_source = 
    "varying vec3 Color;"

    "void main() {"
    " gl_FragColor = vec4(Color, 1.0);"
    "}";

GLuint triangleVertexShader;
GLuint triangleFragmentShader;
GLuint triangleProgram;

GLuint triangleVbo;

GLfloat* triangleModelMatrix;

// Text & FPS

const GLchar* text_vshader_source =
    "attribute vec2 position;"
    "attribute vec2 texcoord;"

    "uniform mat4 model;"
    "uniform mat4 projection;"

    "varying vec2 Texcoord;"

    "void main() {"
    " Texcoord = texcoord;"
    " gl_Position = projection * model * vec4(position, 0.0, 1.0);"
    "}";

const GLchar* text_fshader_source = 
    "varying vec2 Texcoord;"

    "uniform sampler2D tex;"
    "void main() {"
    " gl_FragColor = texture2D(tex, Texcoord).bgra;\n"
    "}";

GLuint textVertexShader;
GLuint textFragmentShader;
GLuint textFpsProgram;

cairo_surface_t* textSurface;
cairo_t* textCr;
GLuint textVbo;
GLint textTexture;

cairo_surface_t* fpsSurface;
cairo_t* fpsCr;
GLuint fpsVbo;
GLint fpsTexture;

void init_shaders() {
    // Triangle

    triangleVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(triangleVertexShader, 1, &triangle_vshader_source, 0);
    glCompileShader(triangleVertexShader);

    glCheck();

    triangleFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(triangleFragmentShader, 1, &triangle_fshader_source, 0);
    glCompileShader(triangleFragmentShader);

    glCheck();

    triangleProgram = glCreateProgram();
    glAttachShader(triangleProgram, triangleVertexShader);
    glAttachShader(triangleProgram, triangleFragmentShader);
    glLinkProgram(triangleProgram);

    glCheck();

    // Text & FPS

    textVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(textVertexShader, 1, &text_vshader_source, 0);
    glCompileShader(textVertexShader);

    glCheck();

    textFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(textFragmentShader, 1, &text_fshader_source, 0);
    glCompileShader(textFragmentShader);

    glCheck();

    textFpsProgram = glCreateProgram();
    glAttachShader(textFpsProgram, textVertexShader);
    glAttachShader(textFpsProgram, textFragmentShader);
    glLinkProgram(textFpsProgram);

    glCheck();

    glUseProgram(textFpsProgram);

    GLint texUniform = glGetUniformLocation(textFpsProgram, "tex");
    glUniform1i(texUniform, 0);

    glUseProgram(0);

    glCheck();
}

void destroy_shaders() {
    // Triangle

    glDeleteProgram(triangleProgram);
    glDeleteShader(triangleFragmentShader);
    glDeleteShader(triangleVertexShader);

    // Text

    glDeleteProgram(textFpsProgram);
    glDeleteShader(textFragmentShader);
    glDeleteShader(textVertexShader);
}

void init_buffers() {
    // Triangle

    const GLfloat triangle_vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // left vertex, red
         0.0f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,    // center vertex, green
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,    // right vertex, blue
    };

    glGenBuffers(1, &triangleVbo);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glCheck();

    // Text

    const GLfloat text_vertices[] = {
          0.0f, TEXT_HEIGHT, 0.0f, 1.0f,       // upper left
          0.0f, 0.0f, 0.0f, 0.0f,              // lower left
          TEXT_WIDTH, TEXT_HEIGHT, 1.0f, 1.0f, // upper right
          TEXT_WIDTH, 0.0f, 1.0f, 0.0f,        // lower right
    };

    glGenBuffers(1, &textVbo);
    glBindBuffer(GL_ARRAY_BUFFER, textVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(text_vertices), text_vertices, GL_STATIC_DRAW);

    glCheck();

    // FPS

    const GLfloat fps_vertices[] = {
          0.0f, FPS_HEIGHT, 0.0f, 1.0f,        // upper left
          0.0f, 0.0f, 0.0f, 0.0f,              // lower left
          FPS_WIDTH, FPS_HEIGHT, 1.0f, 1.0f,   // upper right
          FPS_WIDTH, 0.0f, 1.0f, 0.0f,         // lower right
    };

    glGenBuffers(1, &fpsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, fpsVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fps_vertices), fps_vertices, GL_STATIC_DRAW);

    glCheck();
}

void destroy_buffers() {
    glDeleteBuffers(1, &fpsVbo);
    glDeleteBuffers(1, &textVbo);
    glDeleteBuffers(1, &triangleVbo);
}

void init_textures() {
    // Text

    textSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, TEXT_WIDTH, TEXT_HEIGHT);
    textCr = cairo_create(textSurface);

    cairo_set_source_rgb(textCr, 1, 1, 1);

    cairo_select_font_face(textCr, "Cantarell Regular", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(textCr, 12);

    cairo_move_to(textCr, 0, 15);
    cairo_show_text(textCr, "Use the X, Y and Z keys to select axes of rotation while moving the mouse up or down.");
    cairo_move_to(textCr, 0, 30);
    cairo_show_text(textCr, "Click to animate.");

    unsigned char* pixels = cairo_image_surface_get_data(textSurface);

    glGenTextures(1, &textTexture);
    glBindTexture(GL_TEXTURE_2D, textTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXT_WIDTH, TEXT_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

    // FPS

    fpsSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, FPS_WIDTH, FPS_HEIGHT);
    fpsCr = cairo_create(fpsSurface);

    glGenTextures(1, &fpsTexture);
    glBindTexture(GL_TEXTURE_2D, fpsTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
}

void update_fps_texture(float fps) {
    char fpsStr[10];
    sprintf(fpsStr, "%.2f FPS", fps);

    cairo_set_source_rgba(fpsCr, 0, 0, 0, 0);
    cairo_set_operator(fpsCr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(fpsCr);
    cairo_set_operator(fpsCr, CAIRO_OPERATOR_OVER);

    cairo_set_source_rgb(fpsCr, 1, 1, 1);

    cairo_select_font_face(fpsCr, "Cantarell Regular", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(fpsCr, 12);

    cairo_move_to(fpsCr, 0, 15);
    cairo_show_text(fpsCr, fpsStr);

    unsigned char* pixels = cairo_image_surface_get_data(fpsSurface);

    glBindTexture(GL_TEXTURE_2D, fpsTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FPS_WIDTH, FPS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void destroy_textures() {
    // Text

    glDeleteTextures(1, &textTexture);
    cairo_destroy(textCr);
    cairo_surface_destroy(textSurface);

    // FPS

    glDeleteTextures(1, &fpsTexture);
    cairo_destroy(fpsCr);
    cairo_surface_destroy(fpsSurface);
}

void update_triangle_model() {
    glUseProgram(triangleProgram);

    GLint modelUniform = glGetUniformLocation(triangleProgram, "model");
    triangleModelMatrix = mat4_identity(NULL);

    mat4_rotate(triangleModelMatrix, triangleModelMatrix, rotation[0], X);
    mat4_rotate(triangleModelMatrix, triangleModelMatrix, rotation[1], Y);
    mat4_rotate(triangleModelMatrix, triangleModelMatrix, rotation[2], Z);

    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, triangleModelMatrix);

    free(triangleModelMatrix);
    triangleModelMatrix = NULL;

    glCheck();
}

void update_text_model() {
    glUseProgram(textFpsProgram);

    GLint modelUniform = glGetUniformLocation(textFpsProgram, "model");

    GLfloat translation[] = { 16, 16, 0 };
    GLfloat* textModelMatrix = mat4_translate(NULL, NULL, translation);

    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, textModelMatrix);

    free(textModelMatrix);

    glCheck();
}

void update_fps_model() {
    glUseProgram(textFpsProgram);

    GLint modelUniform = glGetUniformLocation(textFpsProgram, "model");

    GLfloat translation[] = { 16, window->height - 32, 0 };
    GLfloat* fpsModelMatrix = mat4_translate(NULL, NULL, translation);

    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, fpsModelMatrix);

    free(fpsModelMatrix);

    glCheck();
}

void update_projection() {
    {
        // Triangle

        glUseProgram(triangleProgram);

        GLint projectionUniform = glGetUniformLocation(triangleProgram, "projection");
        GLfloat* projectionMatrix = mat4_identity(NULL);

        float aspect = (float)window->width / window->height;
        projectionMatrix[5] = aspect;

        glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, projectionMatrix);

        free(projectionMatrix);

        glCheck();
    }
    {
        // Text & FPS

        glUseProgram(textFpsProgram);

        GLint projectionUniform = glGetUniformLocation(textFpsProgram, "projection");
        GLfloat* projectionMatrix = mat4_orthographic(NULL, 0, window->width, 0, window->height);

        glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, projectionMatrix);

        free(projectionMatrix);

        glCheck();
    }
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        // Triangle

        glUseProgram(triangleProgram);

        update_triangle_model();

        glBindBuffer(GL_ARRAY_BUFFER, triangleVbo);

        GLint positionAttribute = glGetAttribLocation(triangleProgram, "position");
        GLint colorAttribute = glGetAttribLocation(triangleProgram, "color");

        glEnableVertexAttribArray(positionAttribute);
        glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, 0, 6*sizeof(GLfloat), (const GLvoid*)0);
        glEnableVertexAttribArray(colorAttribute);
        glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, 0, 6*sizeof(GLfloat), (const GLvoid*)(3*sizeof(GLfloat)));
        
        glCheck();

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glCheck();
    }
    {
        // Text

        glUseProgram(textFpsProgram);

        update_text_model();

        glBindBuffer(GL_ARRAY_BUFFER, textVbo);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textTexture);

        GLint positionAttribute = glGetAttribLocation(textFpsProgram, "position");
        GLint texcoordAttribute = glGetAttribLocation(textFpsProgram, "texcoord");

        glEnableVertexAttribArray(positionAttribute);
        glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, 0, 4*sizeof(GLfloat), (const GLvoid*)0);
        glEnableVertexAttribArray(texcoordAttribute);
        glVertexAttribPointer(texcoordAttribute, 2, GL_FLOAT, 0, 4*sizeof(GLfloat), (const GLvoid*)(2*sizeof(GLfloat)));
        
        glCheck();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glCheck();
    }
    {
        // FPS

        glUseProgram(textFpsProgram);

        update_fps_model();
        glBindBuffer(GL_ARRAY_BUFFER, fpsVbo);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fpsTexture);

        GLint positionAttribute = glGetAttribLocation(textFpsProgram, "position");
        GLint texcoordAttribute = glGetAttribLocation(textFpsProgram, "texcoord");

        glEnableVertexAttribArray(positionAttribute);
        glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, 0, 4*sizeof(GLfloat), (const GLvoid*)0);
        glEnableVertexAttribArray(texcoordAttribute);
        glVertexAttribPointer(texcoordAttribute, 2, GL_FLOAT, 0, 4*sizeof(GLfloat), (const GLvoid*)(2*sizeof(GLfloat)));
        
        glCheck();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glCheck();
    }
}

void swap_buffers() {
    glFlush();
    glFinish();
    
    glCheck();

    eglSwapBuffers(window->display, window->surface);

    glCheck();
}

char animation_frame() {
    char modified = 0;

    if (rotation[0] < 2 * M_PI) {
        rotation[0] += DEGREES_TO_RADIANS(1);
        modified = 1;
    }
    if (rotation[1] < 2 * M_PI) {
        rotation[1] += DEGREES_TO_RADIANS(1);
        modified = 1;
    }
    if (rotation[2] < 2 * M_PI) {
        rotation[2] += DEGREES_TO_RADIANS(1);
        modified = 1;
    }

    return modified;
}

int main(int argc, char** argv) {
    Keyboard* keyboard = keyboard_init(NULL, "/dev/input/event1");
    Mouse* mouse = mouse_init(NULL, "/dev/input/event0");
    window = window_init(NULL);

    init_shaders();
    init_buffers();
    init_textures();

    update_projection();

    int mouseY;
    char mousePressed[] = { 0, 0 };
    
    char animate = 0;
    struct timeval frameTime[2];
    struct timeval fpsUpdateTime;

    gettimeofday(&frameTime[1], NULL);
    gettimeofday(&fpsUpdateTime, NULL);

    update_fps_texture(0);

    while (1) {
        if (keyboard_key_is_pressed(keyboard, KEY_ESC)) {
            break;
        }

        mousePressed[1] = mousePressed[0];
        mousePressed[0] = mouse_state(mouse, NULL, &mouseY) & BUTTON_LEFT;

        if (!mousePressed[0] && mousePressed[1]) {
            animate = !animate;
        }

        if (animate) {
            if (!animation_frame()) {
                animate = 0;
            }
        } else {
            float mouseYndc = 1 - 2 * (float)mouseY / window->height;
            float radians = 2 * M_PI * mouseYndc;

            memset(rotation, 0, sizeof(rotation));
            if (keyboard_key_is_pressed(keyboard, KEY_X)) {
                rotation[0] = radians;
            }
            if (keyboard_key_is_pressed(keyboard, KEY_Y)) {
                rotation[1] = radians;
            }
            if (keyboard_key_is_pressed(keyboard, KEY_Z)) {
                rotation[2] = radians;
            }
        }

        struct timeval elapsed;
        gettimeofday(&frameTime[0], NULL);
        timersub(&frameTime[0], &frameTime[1], &elapsed);
        frameTime[1] = frameTime[0];

        long millisElapsed = (elapsed.tv_sec * 1000000 + elapsed.tv_usec) / 1000;
        float fps = 1000.0f / millisElapsed;

        timersub(&frameTime[0], &fpsUpdateTime, &elapsed);
        millisElapsed = (elapsed.tv_sec * 1000000 + elapsed.tv_usec) / 1000;

        if (millisElapsed >= 100) {
            fpsUpdateTime = frameTime[0];
            update_fps_texture(fps);
        }

        draw();
        swap_buffers();
    }

    destroy_textures();
    destroy_buffers();
    destroy_shaders();

    mouse_destroy(mouse);
    keyboard_destroy(keyboard);
    free(mouse);
    free(keyboard);
    free(window);
}