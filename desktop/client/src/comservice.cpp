#include "comservice.h"
#include <climits>

void COMService::extract(uint32_t start, uint32_t length, uint32_t &value)
{
    value = 0;

    if ((length > 0) && (start + length <= (BUFLEN * CHAR_BIT)))
    {
        int cursor = start % CHAR_BIT;
        int index = start / CHAR_BIT;

        for (size_t i = 0; i < length; i++)
        {
            uint8_t bit = (uint8_t)((this->buffer[index] >> cursor) & 1);

            if (bit == 1)
            {
                value |= (1 << i);
            }

            cursor++;
            if (cursor == CHAR_BIT)
            {
                cursor = 0;
                index++;
            }
        }
    }
}

void COMService::extract(uint32_t start, uint32_t length, int32_t &value)
{
    uint32_t temp = 0;
    extract(start, length, temp);

    if (length < 32 && (temp & (1U << (length - 1))))
        temp |= (~0U << length);

    value = static_cast<int32_t>(temp);
}

uint32_t COMService::getSpeed(void)
{
    uint32_t value{0};
    extract(signal["speed"].start, signal["speed"].length, value);
    return value;
}

uint32_t COMService::getBatteryLevel(void)
{
    uint32_t value{0};
    extract(signal["battery"].start, signal["battery"].length, value);
    return value;
}

int32_t COMService::getTemperature(void)
{
    int32_t value{0};
    extract(signal["temperature"].start, signal["temperature"].length, value);
    return value;
}

bool COMService::getLeftLight(void)
{
    uint32_t value{0};
    extract(signal["left_turn_signal"].start, signal["left_turn_signal"].length, value);
    return value != 0;
}

bool COMService::getRightLight(void)
{
    uint32_t value{0};
    extract(signal["right_turn_signal"].start, signal["right_turn_signal"].length, value);
    return value != 0;
}
