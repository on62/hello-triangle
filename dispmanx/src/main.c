#include <sys/time.h>
#include <cairo/cairo.h>

#include "keyboard.h"
#include "mouse.h"
#include "window.h"
#include "matrix.h"

#define DEGREES_TO_RADIANS(d)                       (d * 2 * M_PI / 360)

#define TEXT_WIDTH                                                   524
#define TEXT_HEIGHT                                                   32
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

// Text

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
GLuint textProgram;

cairo_surface_t* textSurface;
cairo_t* textCr;
GLuint textVbo;
GLint textTexture;

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

    // Text

    textVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(textVertexShader, 1, &text_vshader_source, 0);
    glCompileShader(textVertexShader);

    glCheck();

    textFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(textFragmentShader, 1, &text_fshader_source, 0);
    glCompileShader(textFragmentShader);

    glCheck();

    textProgram = glCreateProgram();
    glAttachShader(textProgram, textVertexShader);
    glAttachShader(textProgram, textFragmentShader);
    glLinkProgram(textProgram);

    glCheck();

    glUseProgram(textProgram);

    GLint texUniform = glGetUniformLocation(textProgram, "tex");
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

    glDeleteProgram(textProgram);
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
}

void destroy_buffers() {
    glDeleteBuffers(1, &triangleVbo);
    glDeleteBuffers(1, &textVbo);
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

    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &textTexture);
    glBindTexture(GL_TEXTURE_2D, textTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXT_WIDTH, TEXT_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
}

void destroy_textures() {
    // Text

    glDeleteTextures(1, &textTexture);
    cairo_destroy(textCr);
    cairo_surface_destroy(textSurface);
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
    glUseProgram(textProgram);

    GLint modelUniform = glGetUniformLocation(textProgram, "model");

    GLfloat translation[] = { 16, 16, 0 };
    GLfloat* textModelMatrix = mat4_translate(NULL, NULL, translation);

    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, textModelMatrix);

    free(textModelMatrix);

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
        // Text

        glUseProgram(textProgram);

        GLint projectionUniform = glGetUniformLocation(textProgram, "projection");
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

        glUseProgram(textProgram);
        glBindBuffer(GL_ARRAY_BUFFER, textVbo);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textTexture);

        GLint positionAttribute = glGetAttribLocation(textProgram, "position");
        GLint texcoordAttribute = glGetAttribLocation(textProgram, "texcoord");

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

    update_text_model();
    update_projection();

    int mouseY;
    char mousePressed[] = { 0, 0 };
    
    char animate = 0;
    struct timeval timestamp[2];

    gettimeofday(&timestamp[1], NULL);

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
            struct timeval elapsed;
            gettimeofday(&timestamp[0], NULL);
            timersub(&timestamp[0], &timestamp[1], &elapsed);
            long millisElapsed = (elapsed.tv_sec * 1000000 + elapsed.tv_usec) / 1000;
            if (millisElapsed >= (1000 / FRAMES_PER_SECOND)) {
                timestamp[1] = timestamp[0];
                if (!animation_frame()) {
                    animate = 0;
                }
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
        
        update_triangle_model();

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