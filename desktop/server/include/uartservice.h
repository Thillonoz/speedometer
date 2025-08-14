#ifndef UARTSERVICE_H
#define UARTSERVICE_H

#include <QThread>
#include <QSerialPort>
#include "comservice.h"

class UARTService : public COMService, public QThread {
    QSerialPort *serial;
    static constexpr char port_name[13] = "/dev/ttyUSB0";

private:
    void run(void) override;

    void set_serial_communication();

public:
    UARTService() {
        QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
    };

    ~UARTService();
};

#endif
