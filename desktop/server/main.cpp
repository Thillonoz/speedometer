#ifdef UARTCOM
#include "tcpservice.h"
#else
#include "uartservice.h"
#endif
#include <QApplication>
#include "window.h"

int main(int argc, char **argv) {
#ifdef UARTCOM
    TCPService service;
#else
    UARTService service;
#endif

    QApplication app(argc, argv);

    Window w{service};
    w.show();

    return app.exec();
}
