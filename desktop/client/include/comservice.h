#ifndef COMSERVICE_H
#define COMSERVICE_H

#include <cstdint>
#include <mutex>
#include <atomic>
#include "setting.h"

class COMService
{
    Setting::Signal &signal{Setting::Signal::handle()};

    void extract(uint32_t start, uint32_t length, uint32_t &value);

    void extract(uint32_t start, uint32_t length, int32_t &value);

protected:
    std::mutex mtx;
    uint8_t buffer[BUFLEN];
    std::atomic<bool> status{false};

    virtual void run(void) = 0;

public:
    bool getStatus(void) { return status; };

    uint32_t getSpeed(void);

    uint32_t getBatteryLevel(void);

    int32_t getTemperature(void);

    bool getLeftLight(void);

    bool getRightLight(void);

    virtual ~COMService() = default;
};

#endif // COMSERVICE_H
