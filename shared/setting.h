#ifndef SETTING_H
#define SETTING_H

#define BAUDRATE 1048576
#define BUFLEN 3

#define SIGNAL_LIST {                     \
    {{8, 0, 0, 240}, "speed"},            \
    {{7, 8, -60, 60}, "temperature"},     \
    {{7, 15, 0, 100}, "battery"},         \
    {{1, 22, 0, 1}, "left_turn_signal"},  \
    {{1, 23, 0, 1}, "right_turn_signal"}, \
}
#ifdef __cplusplus


#include <map>
#include <tuple>
#include <string>

namespace Setting
{
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

    constexpr int INTERVAL{40};

    namespace TCPIP
    {
        constexpr int PORT{12345};
        constexpr char IP[]{"127.0.0.1"};
    }
}
#else

#include <stdint.h>

typedef struct
{
    int length;
    int start;
    int min;
    int max;
} signal_value_t;

typedef struct
{
    signal_value_t value;
    const char *name;
} signal_entry_t;

static const signal_entry_t signal_table[] = SIGNAL_LIST;

#undef SIGNAL_LIST

#define SIGNAL_COUNT (sizeof(signal_table) / sizeof(signal_table[0]))

// Access macros for C
#define SIGNAL_NAME(i) (signal_table[i].name)
#define SIGNAL_LENGTH(i) (signal_table[i].value.length)
#define SIGNAL_START(i) (signal_table[i].value.start)
#define SIGNAL_MIN(i) (signal_table[i].value.min)
#define SIGNAL_MAX(i) (signal_table[i].value.max)

#define INTERVAL 40

#endif // __cplusplus

#endif // SETTING_H
