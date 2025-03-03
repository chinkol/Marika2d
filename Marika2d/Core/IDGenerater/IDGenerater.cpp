#include "IDGenerater.h"

Mrk::ID Mrk::IDGenerater::Generate()
{
    std::lock_guard<std::mutex> lock(Instance().mutex);
    int64_t timeStamp = GetCurrentTimeStamp();
    if (timeStamp == Instance().lastTimeStamp)
    {
        Instance().sequence = (Instance().sequence + 1) & sequenceMask;
        if (Instance().sequence == 0)
        {
            while (timeStamp <= Instance().lastTimeStamp)
            {
                timeStamp = GetCurrentTimeStamp();
            }
        }
    }
    else
    {
        Instance().sequence = 0;
    }
    Instance().lastTimeStamp = timeStamp;
    Mrk::ID id{ (uint64_t)(timeStamp << timeStampLeftShift) | Instance().sequence };
    return id;
}

Mrk::IDGenerater::IDGenerater()
{
}

int64_t Mrk::IDGenerater::GetCurrentTimeStamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
