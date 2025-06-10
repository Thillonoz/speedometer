#include <QApplication>
#include "canvas.h"
#include "window.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    auto *w = new Window();

    w->set_speed(225);
    w->set_temperature(35);
    w->set_battery(100);
    w->show();

    return app.exec();
}
