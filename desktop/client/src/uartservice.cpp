#include <QDebug>
#include "setting.h"
#include <QSerialPort>
#include "uartservice.h"

#include <iostream>

void UARTService::run(void)
{

    QSerialPort serial;
    serial.setPortName("/dev/ttyUSB1");
    serial.setBaudRate(BAUDRATE);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial.open(QIODevice::ReadOnly))
    {
        qDebug() << "Faled to open serial port";
        status = false;
        return;
    }
    status = true;

    uint8_t temp[sizeof(buffer)]{0};

    while (status)
    {
        qint64 bytesRead = serial.read(reinterpret_cast<char *>(temp), sizeof(temp));
        if (bytesRead != sizeof(temp))
        {
            status = false;
            break;
        }
        else
        {
            std::scoped_lock<std::mutex> locker{mtx};
            std::memcpy(buffer, temp, sizeof(buffer));
        }

        std::cout << "Speed: " << getSpeed()
                  << "\nTemperature: " << getTemperature()
                  << "\nBattery: " << getBatteryLevel()
                  << "\nLeft: " << getLeftLight()
                  << "\nRight: " << getRightLight() << std::endl;

        msleep(Setting::INTERVAL);
    }

    if (serial.isOpen())
    {
        serial.close();
    }
    status = false;
}

UARTService::~UARTService()
{
    quit();
    wait();
}
