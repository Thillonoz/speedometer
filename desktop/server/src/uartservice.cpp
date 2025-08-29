#include <QSerialPort>
#include <QDebug>
#include <mutex>

#include "uartservice.h"

#include "setting.h"

void UARTService::run()
{
    QSerialPort serial;
    serial.setPortName(UART_SPORT);
    serial.setBaudRate(BAUDRATE);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    while (true)
    {
        if (!serial.open(QSerialPort::WriteOnly))
        {
            status = false;
            QThread::msleep(Setting::INTERVAL);
            continue;
        }

        while (!end)
        {

            if (serial.error() != QSerialPort::NoError)
            {
                serial.close();
                status = false;
                break;
            }
            else
            {
                status = true;
                end = false;
                std::scoped_lock<std::mutex>locker {mtx};
                QByteArray data(reinterpret_cast<const char *>(buffer), BUFLEN);

                serial.write(data);
                QThread::msleep(Setting::INTERVAL / 2);
            }
            serial.flush();
        }
    }
    if (serial.isOpen())
    {
        serial.clear(QSerialPort::AllDirections);
        serial.setDataTerminalReady(false);
        serial.setRequestToSend(false);
        serial.close();
    }
    end = true;
}

UARTService::~UARTService()
{
    status = false;
    end = true;

    quit();
    wait();
}
