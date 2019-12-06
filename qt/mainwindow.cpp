#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QWindow>
#include <QScreen>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _animate(false)
{
    ui->setupUi(this);

    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->xSlider, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
    connect(ui->ySlider, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
    connect(ui->zSlider, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);

    connect(ui->animateButton, &QPushButton::clicked, this, &MainWindow::animateButtonClicked);
    connect(ui->quitButton, &QPushButton::clicked, this, &MainWindow::close);

    _timer.callOnTimeout([&](){
        ui->xSlider->setValue(ui->xSlider->value() + 1);
        ui->ySlider->setValue(ui->ySlider->value() + 1);
        ui->zSlider->setValue(ui->zSlider->value() + 1);

        if (ui->xSlider->value() >= ui->xSlider->maximum() &&
            ui->ySlider->value() >= ui->ySlider->maximum() &&
            ui->zSlider->value() >= ui->zSlider->maximum()) {
            _timer.stop();
            _animate = false;
            ui->fpsLabel->setText("");
        }

        float fps = 1000.0 / _time.restart();

        ui->fpsLabel->setText(QString::number(fps, 'f', 2)+" FPS");
    });
    _timer.setInterval(1000 / 60);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sliderValueChanged()
{
    ui->xValueLabel->setText(QString::number(ui->xSlider->value()));
    ui->yValueLabel->setText(QString::number(ui->ySlider->value()));
    ui->zValueLabel->setText(QString::number(ui->zSlider->value()));

    ui->openGLwidget->setXRotation(ui->xSlider->value());
    ui->openGLwidget->setYRotation(ui->ySlider->value());
    ui->openGLwidget->setZRotation(ui->zSlider->value());
    ui->openGLwidget->update();
}

void MainWindow::animateButtonClicked()
{
    _animate = !_animate;

    if (!_animate)
        ui->fpsLabel->setText("");

    if (_animate) {
        if (ui->xSlider->value() >= ui->xSlider->maximum() ||
            ui->ySlider->value() >= ui->ySlider->maximum() ||
            ui->zSlider->value() >= ui->zSlider->maximum()) {
            ui->xSlider->setValue(0);
            ui->ySlider->setValue(0);
            ui->zSlider->setValue(0);
        }

        _timer.start();
    } else {
        _timer.stop();
    }
}




