#include <QSerialPort>
#include <QDebug>
#include <mutex>

#include "uartservice.h"

#include "setting.h"

static std::mutex mutex;

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
            qDebug() << "Failed to open a serial port";
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
                mutex.lock();
                QByteArray data(reinterpret_cast<const char *>(buffer), BUFLEN);

// #define UART_BLE_TESTING 1
#if UART_BLE_TESTING
                qDebug() << buffer[0] << buffer[1] << buffer[2];
                qDebug() << data.toHex();
#endif

                serial.write(data);
                mutex.unlock();

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
    ebd = true;

    quit();
    wait();
}
