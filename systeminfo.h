#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H
#include <windows.h>

class SystemInfo
{
public:
    SystemInfo();
    double getcpuUsage();
    double getRamUsage();
private:
    ULONGLONG prevIdle,prevTotal;
    bool firstCall;

};

#endif // SYSTEMINFO_H
