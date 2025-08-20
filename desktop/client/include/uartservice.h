#ifndef UARTCOM_H
#define UARTCOM_H

#include <QThread>
#include "comservice.h"

#include <QSerialPort>

class UARTService : public COMService, public QThread
{
    const QString serial_name = UART_CPORT;

private:
    void run(void) override;

public:
    UARTService()
    {
        QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
    };
    ~UARTService();
};

#endif