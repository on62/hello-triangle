#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMenu>
#include <QAction>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer _timer;
    bool _animate;

    QMenu _menu;
    QAction* _screenshotAction;

private slots:
    void sliderValueChanged();
    void animateButtonClicked();
    void takeScreenshot();

protected:
    void mousePressEvent(QMouseEvent *event);
};
#endif // MAINWINDOW_H
