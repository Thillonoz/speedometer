#include <QApplication>
#include "canvas.h"
#include "window.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    auto *w = new Window();

    w->set_speed(225);
    w->set_temperature(35);
    w->set_battery(50);
    w->set_blinker(1); // 1 = right, 2 = left, 3 = warning 0 = off
    w->show();
    w->set_connection_status(false);

    return app.exec();
}
