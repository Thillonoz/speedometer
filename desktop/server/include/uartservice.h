#ifndef UARTSERVICE_H
#define UARTSERVICE_H

#include <QThread>
#include "comservice.h"

class UARTService : public COMService, public QThread {
    static constexpr char port_name[13] = "/dev/ttyUSB0";

private:
    void run(void) override;

public:
    UARTService() {
        QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
    };

    ~UARTService();
};

#endif
