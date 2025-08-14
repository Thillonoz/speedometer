#ifndef UARTSERVICE_H
#define UARTSERVICE_H

#include <QThread>
#include "comservice.h"

class UARTService : public COMService, public QThread
{
    static constexpr char port_name[13] = "/dev/ttyUSB0"; // Serial port name

public:
    UARTService()
    {
        // start the thread
        QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
    }

    ~UARTService();

private:
    void run(void) override;
};

#endif // UARTSERVICE_H