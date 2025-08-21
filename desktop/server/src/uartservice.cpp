#include <QSerialPort>
#include <QDebug>
#include <mutex>

#include "uartservice.h"

#include "setting.h"

static std::mutex mutex;

void UARTService::run() {
    QSerialPort serial;
    serial.setPortName(UART_SPORT);
    serial.setBaudRate(BAUDRATE);
    serial.setDataBits(QSerialPort::Data8); // Sending one byte per frame
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    
    while (true)
    {
        if (!serial.open(QSerialPort::WriteOnly))
        {
            qDebug() << "Failed to open a serial port";
            QThread::msleep(Setting::INTERVAL);
            continue;
        }
    }

        status = serial.isOpen();

        while (status)
        {

            mutex.lock();
            QByteArray data(reinterpret_cast<const char *>(buffer), BUFLEN);

//#define UART_BLE_TESTING 1
#if UART_BLE_TESTING
            qDebug() << buffer[0] << buffer[1] << buffer[2];
            qDebug() << data.toHex();
#endif

            serial.write(data);

        serial.write(data);
        serial.flush();

        mutex.unlock();

        QThread::msleep(Setting::INTERVAL);
    }

    if (serial.isOpen()) {
        serial.close();
    }
}

UARTService::~UARTService() {
    quit();
    wait();
}
