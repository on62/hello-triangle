#include "trianglewidget.h"

TriangleWidget::TriangleWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

void TriangleWidget::setXRotation(const double xRotation)
{
    _xRotation = xRotation;
}

void TriangleWidget::setYRotation(const double yRotation)
{
    _yRotation = yRotation;
}

void TriangleWidget::setZRotation(const double zRotation)
{
    _zRotation = zRotation;
}

void TriangleWidget::update()
{
    QOpenGLWidget::update();
}

void TriangleWidget::resizeGL(int w, int h)
{
    float aspect = static_cast<float>(w) / h;
    QMatrix4x4 projection({
          1.0f, 0.0f, 0.0f, 0.0f,
          0.0f, aspect, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f
      });

    _program->setUniformValue("projection", projection);
}

void TriangleWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
}

void TriangleWidget::initializeGL()
{
    initializeOpenGLFunctions();

    initVertexArray();
    initVertexBuffer();
    initProgram();
    layout();

    glClearColor(0, 0, 0, 1);

}

void TriangleWidget::initVertexArray()
{
    _vao.create();
}

void TriangleWidget::initVertexBuffer()
{
    const GLfloat vertices[] = {
        -0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f,
         0.0f,  0.5f, 0, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0, 0.0f, 0.0f, 1.0f
    };

    _vbo.create();
    _vbo.bind();
    _vbo.allocate(3 * 6 * sizeof(GLfloat));
    _vbo.write(0, vertices, sizeof(vertices));
    _vbo.release();
}

void TriangleWidget::initProgram()
{
    _vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    _vertexShader->compileSourceFile(":/program.vert");

    _fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    _fragmentShader->compileSourceFile(":/program.frag");

    _program = new QOpenGLShaderProgram;
    _program->addShader(_vertexShader);
    _program->addShader(_fragmentShader);
    _program->link();
}

void TriangleWidget::layout()
{
    _vao.bind();
    _program->bind();

    GLint positionLocation = _program->attributeLocation("position");
    GLint colorLocation = _program->attributeLocation("color");

    _vbo.bind();

    _program->enableAttributeArray(positionLocation);
    _program->enableAttributeArray(colorLocation);
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0 * sizeof(GLfloat)));
    glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    _vao.release();
    _vbo.release();
}

void TriangleWidget::draw()
{
    QMatrix4x4 rotation;
    rotation.rotate(_xRotation, {1, 0, 0});
    rotation.rotate(_yRotation, {0, 1, 0});
    rotation.rotate(_zRotation, {0, 0, 1});

    _program->setUniformValue("model", rotation);

    QOpenGLVertexArrayObject::Binder binder(&_vao);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
