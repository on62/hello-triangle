#include <gtkmm.h>
#include <GL/glew.h>

#include "mainwindow.h"

int main(int argc, char** argv) {
    auto app = Gtk::Application::create(argc, argv, "org.nirjacobson.hello-triangle");
    
    MainWindow mainWindow;
    
    app->run(mainWindow);

    return 0;
}
