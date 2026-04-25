#include "systeminfo.h"
double SystemInfo::getcpuUsage()
{
    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    ULARGE_INTEGER curIdle, curKernel, curUser;

    curIdle.LowPart  = idleTime.dwLowDateTime;
    curIdle.HighPart = idleTime.dwHighDateTime;

    curKernel.LowPart  = kernelTime.dwLowDateTime;
    curKernel.HighPart = kernelTime.dwHighDateTime;

    curUser.LowPart  = userTime.dwLowDateTime;
    curUser.HighPart = userTime.dwHighDateTime;

    ULONGLONG currentIdle  = curIdle.QuadPart;
    ULONGLONG currentTotal = curKernel.QuadPart + curUser.QuadPart;

    if (firstCall) {
        prevIdle = currentIdle;
        prevTotal = currentTotal;
        firstCall = false;
        return 0.0;
    }

    ULONGLONG totaldiff = currentTotal - prevTotal;
    if (totaldiff == 0) return 0.0;

    ULONGLONG idlediff = currentIdle - prevIdle;

    // ✅ FINAL FIX
    double usage = (double)(totaldiff - idlediff) / totaldiff;

    prevIdle = currentIdle;
    prevTotal = currentTotal;

    return usage * 100;
}

double SystemInfo::getRamUsage()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    GlobalMemoryStatusEx(&memInfo);

    return memInfo.dwMemoryLoad;
}
SystemInfo::SystemInfo()
{
    prevIdle = 0;
    prevTotal = 0;
    firstCall = true;
}
