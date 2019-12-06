#ifndef TRIANGLEGLAREA_H
#define TRIANGLEGLAREA_H

#include <gtkmm/glarea.h>
#include <gtkmm/window.h>
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class TriangleGLArea : public Gtk::GLArea {

    public:
        TriangleGLArea();
        ~TriangleGLArea();

        void setXRotation(const double x);
        void setYRotation(const double y);
        void setZRotation(const double z);

    private:
        GLuint _vao;
        GLuint _vbo;
        GLuint _vertexShader;
        GLuint _fragmentShader;
        GLuint _program;

        double _xRotation;
        double _yRotation;
        double _zRotation;

        void init_vertex_array();
        void init_vertex_buffer();
        void init_program();
        void layout();
        void draw();

        void on_realize() override;
        bool on_render(const Glib::RefPtr< Gdk::GLContext >& context) override;
        void on_resize(int width, int height) override;
};

#endif // TRIANGLEGLAREA_H
