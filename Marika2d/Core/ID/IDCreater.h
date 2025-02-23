#pragma once

#include "Common/Singleton/Singleton.h"

#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>

namespace Mrk
{
    union ID
    {
        uint64_t total64;
        struct
        {
            uint32_t low32;
            uint32_t high32;
        };
    };

    class IDGenerater : public Singleton< IDGenerater>
    {
        MRK_SINGLETON(IDGenerater)
    public:
        static ID Generate();
    private:
        IDGenerater();
        static int64_t GetCurrentTimeStamp();
        static const int   sequenceBits = 12;
        static const int   timeStampLeftShift = 12;
        static const int   sequenceMask = -1 ^ (-1 << sequenceBits);
        int         sequence = 0;
        int64_t     lastTimeStamp = -1L;
        std::mutex  mutex;
    };
}
