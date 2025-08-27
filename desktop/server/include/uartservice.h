#ifndef UARTSERVICE_H
#define UARTSERVICE_H

#include <QThread>
#include "comservice.h"

class UARTService : public COMService, public QThread {
std::atomic<bool>end{false};

private:
    void run(void) override;

public:
    UARTService() {
        QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
    };

    ~UARTService();
};

#endif
