#include <QSerialPort>
#include <QDebug>
#include <mutex>

#include "uartservice.h"

#include <iostream>

#include "setting.h"

static QSerialPort serial;
static std::mutex mutex;

void UARTService::run() {
    if (!serial.isOpen()) {
        if (!serial.open(QSerialPort::WriteOnly)) {
            qDebug() << "Failed to open serial port";
        }
    }

    while (serial.isOpen()) {
        mutex.lock();
        QByteArray data(reinterpret_cast<const char *>(buffer), BUFLEN);

        serial.write(data);
        serial.flush();

        mutex.unlock();

        QThread::msleep(Setting::INTERVAL);
    }

    if (serial.isOpen()) {
        serial.close();
    }
}

void UARTService::set_serial_communication() {
    serial.setPortName(port_name);
    serial.setBaudRate(BAUDRATE);
    serial.setDataBits(QSerialPort::Data8); // Sending one byte per frame
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
}

UARTService::~UARTService() {
    quit();
    wait();
}
