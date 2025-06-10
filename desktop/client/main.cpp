#ifdef UARTCOM
#include "uartservice.h"
#else
#include "tcpservice.h"
#endif

#include "window.h"
#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

#ifdef UARTCOM
    UARTService comService;
#else
    TCPService comService;
#endif
    Window window(comService);
    window.show();

    return app.exec();
}
