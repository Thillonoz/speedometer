#ifdef UARTCOM
#include "uartcom.h"
#else
#include "tcpservice.h"
#endif
#include <QApplication>
#include "window.h"

int main(int argc, char **argv)
{
#ifdef UARTCOM
    UARTSERVICE service;
#else
    TCPService service;
#endif

    QApplication app(argc, argv);

    Window w{service};
    w.show();

    return app.exec();
}