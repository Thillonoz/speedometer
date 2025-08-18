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
    qDebug() << "Serial port opened successfully";
    status = true;

    uint8_t temp[sizeof(buffer)]{0};

    while (status)
    {

        if (serial.waitForReadyRead(Setting::INTERVAL))
        {
            qint64 bytesRead = serial.read(reinterpret_cast<char *>(temp), sizeof(temp));
            if (bytesRead != BUFLEN)
            {
                qDebug() << "dies, not match expected size | " << bytesRead;
            }
            else
            {
                std::scoped_lock<std::mutex> locker{mtx};
                std::memcpy(buffer, temp, sizeof(buffer));
            }

            qDebug() << temp[0] << temp[1] << temp[2];
            qDebug() << buffer[0] << buffer[1] << buffer[2];
            qDebug() << "Speed: " << getSpeed()
                     << "\nTemperature: " << getTemperature()
                     << "\nBattery: " << getBatteryLevel()
                     << "\nLeft: " << getLeftLight()
                     << "\nRight: " << getRightLight();
        }

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
    status = false;
    quit();
    wait();
}
