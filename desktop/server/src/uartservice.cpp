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
            QThread::msleep(Setting::INTERVAL);
            continue;
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

        while (status)
        {

            mutex.lock();
            QByteArray data(reinterpret_cast<const char *>(buffer), BUFLEN);

#define UART_BLE_TESTING 1
#if UART_BLE_TESTING
            qDebug() << buffer[0] << buffer[1] << buffer[2];
            qDebug() << data.toHex();
#endif

            serial.write(data);

            mutex.unlock();

            if (serial.error() == QSerialPort::ResourceError)
            {
                qDebug() << "Device disconnected (ResourceError). Exiting loop.";
                serial.close();
                status = false;
                break;
            }

            serial.flush();
            QThread::msleep(Setting::INTERVAL/2);
        }

        if (!serial.isOpen())
        {
            serial.close();
            status = false;
        }
    }
}

UARTService::~UARTService()
{
    quit();
    wait();
}
