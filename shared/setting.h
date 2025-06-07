#ifndef SETTING_H
#define SETTING_H

#define SIGNAL_LIST                          \
    {{8, 0, 0, 240}, "speed"},               \
        {{7, 0, -60, 60}, "temperature"},    \
        {{7, 15, 0, 100}, "battery"},        \
        {{1, 22, 0, 1}, "left_turn_signal"}, \
        {{1, 22, 0, 1}, "right_turn_signal"}

#define BAUDRATE 1048576
#define BUFLEN 3

#ifdef __cplusplus

#include <map>
#include <climits>
#include <string>

namespace Setting
{
    struct SignalMeta
    {
        int scale;
        int startBit;
        int min;
        int max;
    };

    struct SignalInfo
    {
        SignalMeta meta;
        const char *name;
    };

    constexpr SignalInfo SignalList[] = {
        SIGNAL_LIST};

    constexpr size_t SignalCount = sizeof(SignalList) / sizeof(SignalList[0]);
    constexpr int BaudRate = BAUDRATE;
    constexpr int BufferLength = BUFLEN;

    inline const SignalInfo *getSignalInfo(const std::string &name)
    {
        const SignalInfo *result = nullptr;
        for (const auto &signal : SignalList)
        {
            if (signal.name == name)
            {
                result = &signal;
                break;
            }
        }
        return result;
    }
}

#endif

#endif // SETTING_H
