#pragma once
#include <windows.h>

class TimerManager {
public:
    static void RestartTimer();
    static void StartTimer();
    static void StopTimer();
}; 