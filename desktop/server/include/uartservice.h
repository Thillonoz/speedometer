#ifndef UARTSERVICE_H
#define UARTSERVICE_H

#include <QThread>
#include "comservice.h"

class UARTService : public COMService, public QThread {
    static constexpr char port_name[13] = "/dev/ttyACM0";

private:
    void run(void) override;

    static void set_serial_communication();

public:
    UARTService() = default;

    ~UARTService();
};

#endif
