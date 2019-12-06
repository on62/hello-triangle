#ifndef TRIANGLEWIDGET_H
#define TRIANGLEWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QFile>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QtMath>

class TriangleWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    TriangleWidget(QWidget* parent);

    void setXRotation(const double xRotation);
    void setYRotation(const double yRotation);
    void setZRotation(const double zRotation);
    void update();

protected:
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void initializeGL() override;

private:
    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo;

    QOpenGLShader* _vertexShader;
    QOpenGLShader* _fragmentShader;
    QOpenGLShaderProgram* _program;

    void initVertexArray();
    void initVertexBuffer();
    void initProgram();
    void layout();
    void draw();

    double _xRotation;
    double _yRotation;
    double _zRotation;

};

#endif // TRIANGLEWIDGET_H
