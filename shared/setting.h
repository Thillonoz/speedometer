#ifndef SETTING_H
#define SETTING_H

#define BAUDRATE 1048576
#define BUFLEN 3

#ifdef __cplusplus

#define SIGNAL_LIST                          \
    {{8, 0, 0, 240}, "speed"},               \
        {{7, 0, -60, 60}, "temperature"},    \
        {{7, 15, 0, 100}, "battery"},        \
        {{1, 22, 0, 1}, "left_turn_signal"}, \
        {{1, 22, 0, 1}, "right_turn_signal"}

#include <map>
#include <string>

namespace Setting
{
    constexpr int BaudRate = BAUDRATE;
    constexpr int BufferLength = BUFLEN;

    struct SignalInfo
    {
        uint8_t length;
        uint8_t start;
        int min;
        int max;

        int operator[](const std::string &key) const
        {
            if (key == "length")
                return length;
            if (key == "start")
                return start;
            if (key == "min")
                return min;
            if (key == "max")
                return max;
            throw std::out_of_range("Invalid key: " + key);
        }
    };

    class Signals
    {
    private:
        std::map<std::string, SignalInfo> signal_map;

        Signals();

    public:
        static Signals &handle();

        Signals(const Signals &) = delete;
        void operator=(const Signals &) = delete;

        const SignalInfo &operator[](const std::string &name) const;
    };

    static const std::pair<SignalInfo, const char *> signalArray[] = {
        SIGNAL_LIST};

    inline Signals::Signals()
    {
        for (const auto &[info, name] : signalArray)
        {
            signal_map[name] = info;
        }
    }

    inline Signals &Signals::handle()
    {
        static Signals instance;
        return instance;
    }

    inline const SignalInfo &Signals::operator[](const std::string &name) const
    {
        auto it = signal_map.find(name);
        if (it == signal_map.end())
            throw std::out_of_range("Signal not found: " + name);
        return it->second;
    }

}

#endif // __cplusplus

#endif // SETTING_H
