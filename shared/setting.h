#ifndef SETTING_H
#define SETTING_H

#define SIGNAL_LIST                        \
    {8, 0, 0, 240, "Speed"},               \
        {7, 0, -60, 60, "Temperature"},    \
        {7, 15, 0, 100, "Battery"},        \
        {1, 22, 0, 1, "Left Turn Signal"}, \
        {1, 22, 0, 1, "Right Turn Signal"}

#define BAUDRATE 1048576
#define BUFLEN 3

#ifdef __cplusplus

#include <map>
#include <climits>
#include <string>

namespace Setting
{
    // Communication protocol selection
    enum class Protocol
    {
        UART,
        TCP,
        COM
    };

    struct SignalInfo
    {
        int scale;
        int startBit;
        int min;
        int max;
        const char *name;
    };

    constexpr SignalInfo SignalList[] = {
        SIGNAL_LIST};

    constexpr size_t SignalCount = sizeof(SignalList) / sizeof(SignalList[0]);
    constexpr int BaudRate = BAUDRATE;
    constexpr int BufferLength = BUFLEN;
}
#endif

#endif // SETTING_H
