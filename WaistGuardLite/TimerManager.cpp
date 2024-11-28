#include "TimerManager.h"
#include "main.h"
#include "MainWindow.h"

void TimerManager::StartTimer()
{
    // 创建工作定时器
    g_appState.workTimer = SetTimer(
        g_appState.hwnd,
        1,
        g_appState.workDuration * 60 * 1000,
        MainWindow::WorkTimerProc
    );

    // 创建显示定时器 - 使用更高的更新频率
    g_appState.displayTimer = SetTimer(
        g_appState.hwnd,
        2,
        100,  // 100ms 间隔，提高更新频率
        MainWindow::DisplayTimerProc
    );
}

void TimerManager::StopTimer()
{
    if (g_appState.workTimer)
    {
        KillTimer(g_appState.hwnd, g_appState.workTimer);
    }
    if (g_appState.displayTimer)
    {
        KillTimer(g_appState.hwnd, g_appState.displayTimer);
    }
}

void TimerManager::RestartTimer()
{
    StopTimer();
    g_appState.startTick = GetTickCount64();
    g_appState.isResting = false;
    g_appState.isPreResting = false;
    StartTimer();
    InvalidateRect(g_appState.hwnd, NULL, TRUE);
    UpdateWindow(g_appState.hwnd);
}
  