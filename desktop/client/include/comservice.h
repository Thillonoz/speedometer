#ifndef COMSERVICE_H
#define COMSERVICE_H

#include <cstdint>
#include <mutex>
#include <atomic>
#include <array>
#include <string>
#include "setting.h"

class COMService
{
public:
    virtual ~COMService() = default;

    // Start thread that receives and stores buffer
    virtual void run() = 0;

    // Get extracted signals
    virtual uint32_t getSpeed() const = 0;
    virtual int32_t getTemperature() const = 0;
    virtual uint32_t getBatteryLevel() const = 0;
    virtual bool getLeftSignal(void) const = 0;
    virtual bool getRightSignal(void) const = 0;

    // Get communication status
    bool getStatus() const;

protected:
    std::atomic<bool> status{false};
    mutable std::mutex mtx;
    std::array<uint8_t, Setting::BufferLength> buffer{};

    uint32_t extractBits(int startBit, int bitLength) const;
    uint32_t extractSignal(const std::string &name) const;
};

#endif // COMSERVICE_H
