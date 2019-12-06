#version 120

attribute vec3 position;
attribute vec3 color;

uniform mat4 model;
uniform mat4 projection;

varying vec3 vertColor;

void main() {
    vertColor = color;

    gl_Position = projection * model * vec4(position, 1.0);
}
