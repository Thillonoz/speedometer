#ifndef COMSERVICE_H
#define COMSERVICE_H

#include <mutex>
#include <atomic>
#include <cstdint>
#include "setting.h"

class COMService
{
public:
    Setting::Signal &SIGNALS{Setting::Signal::handle()};
    void insert(const uint32_t _start, const uint32_t _length, uint32_t _value);
    
    void extract(const uint32_t _start, const uint32_t _length, uint32_t &_var);
    
protected:
    std::mutex mtx;
    uint8_t buffer[BUFLEN]{};
    std::atomic<bool> status{false};

    virtual void run(void) = 0;

public:
    bool getStatus(void) { return status; };
    void insertSpeed(uint32_t _value);
    void insertBatteryLevel(uint32_t _value);
    void insertTemperature(int32_t _value);
    void insertLeftLight(bool _value);
    void insertRightLight(bool _value);

    virtual ~COMService() = default;
};

#endif