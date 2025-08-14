#ifdef UARTCOM
#include "uartservice.h"
#else
#include "tcpservice.h"
#endif

#include <QApplication>
#include "window.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

#ifdef UARTCOM
    UARTService service;
#else
    TCPService service;
#endif

    Window w(service);
    w.show();

    return app.exec();
}
