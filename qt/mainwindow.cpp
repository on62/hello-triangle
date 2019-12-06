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
    , _menu(this)
{
    ui->setupUi(this);

    _screenshotAction = _menu.addAction("Take Screenshot");

    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->xSlider, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
    connect(ui->ySlider, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
    connect(ui->zSlider, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);

    connect(ui->animateButton, &QPushButton::clicked, this, &MainWindow::animateButtonClicked);
    connect(ui->quitButton, &QPushButton::clicked, this, &MainWindow::close);

    connect(_screenshotAction, &QAction::triggered, this, &MainWindow::takeScreenshot);

    _timer.callOnTimeout([&](){
        ui->xSlider->setValue(ui->xSlider->value() + 1);
        ui->ySlider->setValue(ui->ySlider->value() + 1);
        ui->zSlider->setValue(ui->zSlider->value() + 1);

        if (ui->xSlider->value() >= ui->xSlider->maximum() &&
            ui->ySlider->value() >= ui->ySlider->maximum() &&
            ui->zSlider->value() >= ui->zSlider->maximum()) {
            _timer.stop();
            _animate = false;
        }
    });
    _timer.setInterval(1000 / 30);
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

    if (_animate) {
        _timer.start();
    } else {
        _timer.stop();
    }
}

void MainWindow::takeScreenshot()
{
    const QWindow* window = windowHandle();
    QPixmap screenShot = window->screen()->grabWindow(winId());
    screenShot.save("screenshot.png");
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        _menu.exec(event->globalPos());
    }
}



