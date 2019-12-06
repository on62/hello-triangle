#include "triangleglarea.h"

TriangleGLArea::TriangleGLArea() {

}

TriangleGLArea::~TriangleGLArea() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteProgram(_program);
    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader);
}

void TriangleGLArea::setXRotation(const double x) {
    _xRotation = x;
    queue_render();
}

void TriangleGLArea::setYRotation(const double y) {
    _yRotation = y;
    queue_render();
}

void TriangleGLArea::setZRotation(const double z) {
    _zRotation = z;
    queue_render();
}

void TriangleGLArea::on_realize() {
    Gtk::GLArea::on_realize();

    make_current();
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "glewInit() failed." << std::endl;
    }

    glClearColor(0, 0, 0, 1);

    init_vertex_array();
    init_vertex_buffer();
    init_program();
    layout();
}

bool TriangleGLArea::on_render(const Glib::RefPtr< Gdk::GLContext >& context) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();

    Gtk::Container* container = get_toplevel();
    Gtk::Window* window = dynamic_cast<Gtk::Window*>(container);
    window->queue_draw();

    return true;
}

void TriangleGLArea::on_resize(int width, int height) {
    Gtk::GLArea::on_resize(width, height);
    
    float aspect = static_cast<float>(width) / height;
    glm::mat4 projection(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, aspect, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    glUseProgram(_program);

    GLint projectionLocation = glGetUniformLocation(_program, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

void TriangleGLArea::init_vertex_array() {
    glGenVertexArrays(1, &_vao);
}

void TriangleGLArea::init_vertex_buffer() {
    glGenBuffers(1, &_vbo);

    const GLfloat data[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // left vertex, red
         0.0f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,    // center vertex, green
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,    // right vertex, blue
    };

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), reinterpret_cast<const void*>(data), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TriangleGLArea::init_program() {
    _vertexShader = glCreateShader(GL_VERTEX_SHADER);
    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    _program = glCreateProgram();

    std::ifstream vertIfstream("src/program.vert");
    std::string vertSource((std::istreambuf_iterator<char>(vertIfstream)), std::istreambuf_iterator<char>());
    const char* vertSourceCstr = vertSource.c_str();
    glShaderSource(_vertexShader, 1, &vertSourceCstr, NULL);
    glCompileShader(_vertexShader);

    std::ifstream fragIfstream("src/program.frag");
    std::string fragSource((std::istreambuf_iterator<char>(fragIfstream)), std::istreambuf_iterator<char>());
    const char* fragSourceCstr = fragSource.c_str();
    glShaderSource(_fragmentShader, 1, &fragSourceCstr, NULL);
    glCompileShader(_fragmentShader);

    glAttachShader(_program, _vertexShader);
    glAttachShader(_program, _fragmentShader);
    glLinkProgram(_program);
}

void TriangleGLArea::layout() {
    glBindVertexArray(_vao);

    GLint positionLocation = glGetAttribLocation(_program, "position");
    GLint colorLocation = glGetAttribLocation(_program, "color");

    glEnableVertexAttribArray(positionLocation);
    glEnableVertexAttribArray(colorLocation);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(colorLocation, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), reinterpret_cast<const void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void TriangleGLArea::draw() {
    glm::mat4 model(1.0f);

    float xRadians = _xRotation * 2 * M_PI / 360.0f;
    float yRadians = _yRotation * 2 * M_PI / 360.0f;
    float zRadians = _zRotation * 2 * M_PI / 360.0f;
    model = glm::rotate(model, xRadians, glm::vec3(1, 0, 0));
    model = glm::rotate(model, yRadians, glm::vec3(0, 1, 0));
    model = glm::rotate(model, zRadians, glm::vec3(0, 0, 1));

    glUseProgram(_program);

    GLint modelLocation = glGetUniformLocation(_program, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(_vao);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
}
