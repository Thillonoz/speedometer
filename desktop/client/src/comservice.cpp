#include "comservice.h"

bool COMService::getStatus() const
{
    return status;
}

uint32_t COMService::extractBits(int startBit, int bitLength) const
{
    std::lock_guard lock(mtx);
    uint32_t raw = 0;
    for (size_t i = 0; i < buffer.size(); ++i)
    {
        raw |= buffer[i] << (8 * i);
    }
    raw >>= startBit;
    return raw & ((1u << bitLength) - 1);
}

uint32_t COMService::extractSignal(const std::string &name) const
{
    const auto *info = Setting::getSignalInfo(name);
    if (!info)
        return 0;
    return extractBits(info->meta.startBit, info->meta.scale);
}
