#include <QSerialPort>
#include <QDebug>
#include <mutex>

#include "uartservice.h"
#include "setting.h"

static QSerialPort serial;

void UARTService::run() {
    serial.moveToThread(this);
    set_serial_communication();

    if (!serial.isOpen()) {
        if (!serial.open(QSerialPort::WriteOnly)) {
            qDebug() << "Failed to open serial port";
        }
    } {
        std::unique_lock<std::mutex> lock;
        QByteArray data;
        for (int i = 0; i < BUFLEN; i++) {
            data.append(buffer[i]);
        }

        serial.write(data);
    }

    sleep(Setting::INTERVAL);
    exec();
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
    if (serial.isOpen()) {
        serial.close();
    }
    quit();
    wait();
}
