#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
#include <iostream>

#include "triangleglarea.h"
#include "timercpp.h"

class MainWindow {
    public:
        MainWindow();
        ~MainWindow();

        operator Gtk::Window&();

    private:
        Gtk::Window* _window;
        Gtk::Scale* _xScale;
        Gtk::Scale* _yScale;
        Gtk::Scale* _zScale;
        Gtk::Button* _animateButton;
        Gtk::Button* _quitButton;

        TriangleGLArea _glArea;

        Gtk::Menu _menu;
        Gtk::MenuItem _screenshotMenuItem;

        Glib::RefPtr<Gtk::Adjustment> _xScaleAdjustment;
        Glib::RefPtr<Gtk::Adjustment> _yScaleAdjustment;
        Glib::RefPtr<Gtk::Adjustment> _zScaleAdjustment;

        Timer _timer;
        Glib::Dispatcher _dispatcher;
        bool _animate;

        void initScales();
        void scaleValueChanged();
        void animateButtonClicked();
        void quitButtonClicked();
        void on_notification_from_timer_thread();
        bool on_button_press_event(GdkEventButton* event);
        void take_screenshot();
};

#endif // MAINWINDOW_H
