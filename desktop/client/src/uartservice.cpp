#include <QDebug>
#include "setting.h"
#include <QSerialPort>
#include "uartservice.h"

void UARTService::run()
{
    QSerialPort serial;

    serial.setPortName(port_name);
    serial.setBaudRate(BAUDRATE);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial.isOpen())
    {
        if (!serial.open(QSerialPort::ReadOnly))
        { // RECEIVE-ONLY
            qDebug() << "Failed to open client serial port";
            return;
        }
    }

    QByteArray rx;
    rx.reserve(BUFLEN * 2);

    while (serial.isOpen())
    {

        if (serial.waitForReadyRead(50))
        {
            rx += serial.readAll();

            while (serial.waitForReadyRead(2))
                rx += serial.readAll();

            // Copy latest BUFLEN bytes into COMService::buffer (thread-safe)
            mtx.lock();
            int take = rx.size() < BUFLEN ? rx.size() : BUFLEN;
            int start = rx.size() - take;

            for (int i = 0; i < take; ++i)
                buffer[i] = static_cast<uint8_t>(rx.at(start + i));
            for (int i = take; i < BUFLEN; ++i)
                buffer[i] = 0;
            mtx.unlock();

            // Keep memory bounded
            if (rx.size() > BUFLEN * 2)
                rx = rx.right(BUFLEN);

            status = true; // mark as “connected/receiving”
        }
        else
        {
            QThread::msleep(2); // tiny breather when idle
        }

        if (serial.error() == QSerialPort::ResourceError)
        {
            qDebug() << "Serial resource error:" << serial.errorString();
            break;
        }
    }

    if (serial.isOpen())
        serial.close();

    status = false;
}

UARTService::~UARTService()
{
    quit();
    wait();
}
