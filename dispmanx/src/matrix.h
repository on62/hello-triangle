#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "GLES2/gl2.h"

#include "global.h"

GLfloat* mat4_identity(GLfloat* mat4d);
GLfloat* mat4_translate(GLfloat* mat4d, GLfloat* mat4s, GLfloat* vec3);
GLfloat* mat4_rotate(GLfloat* mat4d, GLfloat* mat4s, GLfloat radians, GLfloat* vec3);
GLfloat* mat4_multiply(GLfloat* mat4d, GLfloat* mat4a, GLfloat* mat4b);
GLfloat* mat4_perspective(GLfloat* mat4d, double fov, double aspect, double near, double far);
GLfloat* mat4_orthographic(GLfloat* mat4d, double left, double right, double top, double bottom);

GLfloat mat2_determinate(GLfloat* mat2s);

GLfloat* mat3_sub(GLfloat* mat2d, GLfloat* mat3s, int i);
GLfloat* mat3_transpose(GLfloat* mat3d, GLfloat* mat3s);
GLfloat* mat3_inverse(GLfloat* mat3d, GLfloat* mat3s);
GLfloat mat3_determinate(GLfloat* mat3s);

GLfloat* mat4_sub(GLfloat* mat3d, GLfloat* mat4s, int i);
GLfloat* mat4_transpose(GLfloat* mat4d, GLfloat* mat4s);
GLfloat* mat4_inverse(GLfloat* mat4d, GLfloat* mat4s);
GLfloat mat4_determinate(GLfloat* mat4s);

GLfloat* vec3_add(GLfloat* vec3d, GLfloat* vec3a, GLfloat* vec3b);
GLfloat* vec3_scale(GLfloat* vec3d, GLfloat* vec3s, GLfloat m);
GLfloat* vec3_transform(GLfloat* vec3d, GLfloat* mat4, GLfloat* vec3s);
GLfloat* vec3_normalize(GLfloat* vec3d, GLfloat* vec3s);

GLfloat* vec4_transform(GLfloat* vec4d, GLfloat* mat4, GLfloat* vec4s);

#endif // MATRIX_H
