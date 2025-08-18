#ifndef UARTCOM_H
#define UARTCOM_H

#include <QThread>
#include "comservice.h"

#include <QSerialPort>

class UARTService : public COMService, public QThread {

    QSerialPort serial;

    void run(void) override;

public:
    UARTService() = default;
    ~UARTService() {
        if (serial.isOpen())
        {
            serial.close();
        }
    }        
};

#endif