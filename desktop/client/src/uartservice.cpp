#include <QDebug>
#include "setting.h"
#include <QSerialPort>
#include "uartservice.h"

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

        uint8_t temp[sizeof(buffer)]{0};
        while (!end)
        {
            if (const QSerialPort::SerialPortError e = serial.error(); e != QSerialPort::NoError)
            {
                serial.close();
                status = false;
                break;
            }

            if (serial.waitForReadyRead(3 * Setting::INTERVAL))
            {
                qint64 bytesRead = serial.read(reinterpret_cast<char *>(temp), sizeof(temp));
                if (bytesRead != BUFLEN)
                {
                    qDebug() << "does not match expected size | " << bytesRead;
                    status = false;
                }
                else
                {
                    status = true;
                    end = false;
                    std::scoped_lock<std::mutex> locker{mtx};
                    std::memcpy(buffer, temp, sizeof(buffer));
                }

                serial.flush();
            }
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
