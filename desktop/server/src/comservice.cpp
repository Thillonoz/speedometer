#include "comservice.h"
#include <climits>

void COMService::insert(const uint32_t _start, const uint32_t _length, uint32_t _value)
{
  mtx.lock();
  int cursor = _start % CHAR_BIT;
  int index = _start / CHAR_BIT;

  for (size_t i = 0; i < _length; i++)
  {
    uint8_t bit = (uint8_t)((_value >> i) & 1);

    if (bit == 0)
    {
      buffer[index] &= ~(1 << cursor);
    }
    else
    {
      buffer[index] |= (1 << cursor);
    }

    cursor++;
    if (cursor == CHAR_BIT)
    {
      cursor = 0;
      index++;
    }
  }
  mtx.unlock();
};

void COMService::insertSpeed(uint32_t _value)
{
  insert(SIGNALS["speed"].start, SIGNALS["speed"].length, _value);
};

void COMService::insertTemperature(int32_t _value)
{
  insert(SIGNALS["temperature"].start, SIGNALS["temperature"].length, _value);
};

void COMService::insertBatteryLevel(uint32_t _value)
{
  insert(SIGNALS["battery"].start, SIGNALS["battery"].length, _value);
};

void COMService::insertLeftLight(bool _value)
{
  insert(SIGNALS["left_turn_signal"].start, SIGNALS["left_turn_signal"].length, _value);
};

void COMService::insertRightLight(bool _value)
{
  insert(SIGNALS["right_turn_signal"].start, SIGNALS["right_turn_signal"].length, _value);
};