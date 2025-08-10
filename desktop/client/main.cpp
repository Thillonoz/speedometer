#ifdef UARTCOM
#include "uartcom.h"
#else
#include "tcpservice.h"
#endif
#include <QApplication>
#include "canvas.h"
#include "window.h"
#include <QTimer>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

#ifdef UARTCOM
    UARTSERVICE service;
#else
    TCPService service;
#endif

    Window w(nullptr, service);
    w.show();

    QTimer *timer = new QTimer(&w);
    QObject::connect(timer, &QTimer::timeout, [&]() {
        w.set_speed(service.getSpeed());
        w.set_temperature(service.getTemperature());

        if (service.getLeftLight() == 1 && service.getRightLight() == 1)
            w.set_blinker(3); // warning
        else if (service.getLeftLight() == 1)
            w.set_blinker(2); // left
        else if (service.getRightLight() == 1)
            w.set_blinker(1); // right
        else
            w.set_blinker(0); // off

        w.set_battery(service.getBatteryLevel());
        w.set_connection_status(service.getStatus());
    });
    timer->start(100);

    return app.exec();
}
