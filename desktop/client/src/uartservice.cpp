#include <QDebug>
#include "setting.h"
#include <QSerialPort>
#include "uartservice.h"

#include <iostream>

void UARTService::run(void)
{
    QSerialPort serial;
    serial.setPortName(UART_CPORT);
    serial.setBaudRate(BAUDRATE);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    while (true)
    {
        if (!serial.open(QIODevice::ReadOnly))
        {
            qDebug() << "Failed to open serial port";
            status = false;
            QThread::msleep(Setting::INTERVAL);
            continue;
        }

        qDebug() << "Serial port opened successfully";
        status = true;

        uint8_t temp[sizeof(buffer)]{0};

        while (status)
        {
            QSerialPort::SerialPortError e = serial.error();
            if (e != QSerialPort::NoError)
            {
                serial.clear(QSerialPort::AllDirections);
                serial.setDataTerminalReady(false);
                serial.setRequestToSend(false);
                serial.close();

                status = false;
                break;
            }

            if (serial.waitForReadyRead(Setting::INTERVAL))
            {
                qint64 bytesRead = serial.read(reinterpret_cast<char *>(temp), sizeof(temp));
                if (bytesRead != BUFLEN)
                {
                    qDebug() << "does not match expected size | " << bytesRead;
                    status = false;
                    break;
                }
                else
                {
                    std::scoped_lock<std::mutex> locker{mtx};
                    std::memcpy(buffer, temp, sizeof(buffer));
                }

                serial.flush();
            }

            msleep(Setting::INTERVAL);
        }
    }

    if (serial.isOpen())
    {
        serial.clear(QSerialPort::AllDirections);
        serial.setDataTerminalReady(false);
        serial.setRequestToSend(false);
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
