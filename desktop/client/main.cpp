#ifdef UARTCOM
#include "uartcom.h"
#else
#include "tcpservice.h"
#endif

#include <QApplication>
#include "window.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

#ifdef UARTCOM
    UARTSERVICE service;
#else
    TCPService service;
#endif

    Window w(nullptr, service);
    w.show();

    return app.exec();
}
