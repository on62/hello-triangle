#include "mainwindow.h"

MainWindow::MainWindow() {
    auto refBuilder = Gtk::Builder::create();

    refBuilder->add_from_file("ui/mainwindow.glade");
    refBuilder->get_widget("mainWindow", _window);
    refBuilder->get_widget("xScale", _xScale);
    refBuilder->get_widget("yScale", _yScale);
    refBuilder->get_widget("zScale", _zScale);
    refBuilder->get_widget("animateButton", _animateButton);
    refBuilder->get_widget("quitButton", _quitButton);

    initScales();

    Gtk::Box* box;
    refBuilder->get_widget("vBox", box);

    box->pack_start(_glArea);
    box->reorder_child(_glArea, 0);
    _glArea.show();

    _xScale->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::scaleValueChanged));
    _yScale->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::scaleValueChanged));
    _zScale->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::scaleValueChanged));
    _animateButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::animateButtonClicked));
    _quitButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::quitButtonClicked));

    _dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_notification_from_timer_thread));

    _screenshotMenuItem.set_label("Take screenshot");
    _menu.append(_screenshotMenuItem);
    _screenshotMenuItem.show();

    _window->set_title("Hello Triangle");
    _window->signal_button_press_event().connect(sigc::mem_fun(*this, &MainWindow::on_button_press_event));
    _screenshotMenuItem.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::take_screenshot));

    _animate = false;

}

MainWindow::~MainWindow() {
    delete _window;
}

void MainWindow::initScales() {
    _xScaleAdjustment = Gtk::Adjustment::create(0, 0, 360, 1, 1);
    _yScaleAdjustment = Gtk::Adjustment::create(0, 0, 360, 1, 1);
    _zScaleAdjustment = Gtk::Adjustment::create(0, 0, 360, 1, 1);

    _xScale->set_adjustment(_xScaleAdjustment);
    _yScale->set_adjustment(_yScaleAdjustment);
    _zScale->set_adjustment(_zScaleAdjustment);

    _xScale->set_digits(0);
    _yScale->set_digits(0);
    _zScale->set_digits(0);
}

void MainWindow::scaleValueChanged() {
    _glArea.setXRotation(_xScaleAdjustment->get_value());
    _glArea.setYRotation(_yScaleAdjustment->get_value());
    _glArea.setZRotation(_zScaleAdjustment->get_value());   
}

void MainWindow::animateButtonClicked() {
    _animate = !_animate;

    if (_animate) {
        _timer.setInterval([&](){
            _dispatcher.emit();

            if (_xScale->get_value() >= 360.0f &&
                _yScale->get_value() >= 360.0f &&
                _zScale->get_value() >= 360.0f) {
                    _timer.stop();
                    _animate = false;
                }
        }, static_cast<int>(1000.0f / 30.0f));
    } else {
        _timer.stop();
    }
}

void MainWindow::quitButtonClicked() {
    _window->close();
}

void MainWindow::on_notification_from_timer_thread() {
    _xScale->set_value(_xScale->get_value() + 1);
    _yScale->set_value(_yScale->get_value() + 1);
    _zScale->set_value(_zScale->get_value() + 1);
}

bool MainWindow::on_button_press_event(GdkEventButton* event) {
    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
        if (!_menu.get_attach_widget())
            _menu.attach_to_widget(*this);

        _menu.popup(event->button, event->time);
        return true;
    }
    return false;
}

void MainWindow::take_screenshot() {
    auto window = _window->get_window();
    int x, y;
    window->get_position(x, y);
    int width = window->get_width();
    int height = window->get_height();

    window = Gdk::Window::get_default_root_window();
    auto pixels = Gdk::Pixbuf::create(window, x, y, width, height);
    pixels->save("screenshot.png", "png");
}

MainWindow::operator Gtk::Window&() {
    return *_window;
}
