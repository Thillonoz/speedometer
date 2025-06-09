#ifndef SETTING_H
#define SETTING_H

#define BAUDRATE 1048576
#define BUFLEN 3

#ifdef __cplusplus

#define SIGNAL_LIST {                    \
    {{8, 0, 0, 240}, "speed"},           \
    {{7, 0, -60, 60}, "temperature"},    \
    {{7, 15, 0, 100}, "battery"},        \
    {{1, 22, 0, 1}, "left_turn_signal"}, \
    {{1, 22, 0, 1}, "right_turn_signal"}}

#include <map>
#include <string>

namespace Setting
{
    constexpr int BaudRate = BAUDRATE;
    constexpr int BufferLength = BUFLEN;

    class Signal
    {
        struct value_t
        {
            int length, start, min, max;
        };
        using key_t = std::string;

        std::map<key_t, value_t> signal;

        Signal()
        {
            const std::tuple<value_t, key_t> list[] = SIGNAL_LIST;
#undef SIGNAL_LIST

            for (const auto &elem : list)
            {
                signal.insert({std::get<key_t>(elem), std::get<value_t>(elem)});
            }
        }

    public:
        const value_t &operator[](const key_t &key)
        {
            return signal[key];
        }

        static Signal &handle(void)
        {
            static Signal instance;
            return instance;
        }
    };
}

#endif // __cplusplus

#endif // SETTING_H
