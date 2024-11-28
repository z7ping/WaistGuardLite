// src/MainWindow.cpp
#include "MainWindow.h"
#include <strsafe.h>

bool MainWindow::Create(HINSTANCE hInstance)
{
    // 初始化应用状态
    g_appState.workDuration = DEFAULT_WORK_MINUTES;
    g_appState.breakDuration = DEFAULT_BREAK_MINUTES;
    GetSystemTime(&g_appState.startTime);

    // 创建托盘图标
    CreateTrayIcon();

    // 初始化定时器
    InitTimers();

    return true;
}

void MainWindow::InitTimers()
{
    // 创建工作定时器
    g_appState.workTimer = SetTimer(
        g_appState.hwnd,
        1,
        g_appState.workDuration * 60 * 1000,  // 转换为毫秒
        WorkTimerProc
    );

    // 创建显示定时器
    g_appState.displayTimer = SetTimer(
        g_appState.hwnd,
        2,
        1000,  // 1秒更新一次
        DisplayTimerProc
    );
}

void MainWindow::CreateTrayIcon()
{
    ZeroMemory(&g_appState.nid, sizeof(NOTIFYICONDATA));
    g_appState.nid.cbSize = sizeof(NOTIFYICONDATA);
    g_appState.nid.hWnd = g_appState.hwnd;
    g_appState.nid.uID = 1;
    g_appState.nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_appState.nid.uCallbackMessage = WM_TRAYICON;
    g_appState.nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_WAISTGUARDLITE));
    StringCchCopy(g_appState.nid.szTip, ARRAYSIZE(g_appState.nid.szTip), WINDOW_TITLE);
    Shell_NotifyIcon(NIM_ADD, &g_appState.nid);
}

void MainWindow::UpdateWorkTime()
{
    SYSTEMTIME currentTime;
    GetSystemTime(&currentTime);

    // 计算工作时长
    FILETIME ft1, ft2;
    SystemTimeToFileTime(&g_appState.startTime, &ft1);
    SystemTimeToFileTime(&currentTime, &ft2);

    ULARGE_INTEGER u1, u2;
    u1.LowPart = ft1.dwLowDateTime;
    u1.HighPart = ft1.dwHighDateTime;
    u2.LowPart = ft2.dwLowDateTime;
    u2.HighPart = ft2.dwHighDateTime;

    ULONGLONG diff = (u2.QuadPart - u1.QuadPart) / 10000000;  // 转换为秒
    int minutes = (int)(diff / 60);
    int seconds = (int)(diff % 60);

    // 更新显示
    wchar_t text[64];
    StringCchPrintf(text, ARRAYSIZE(text), L"已工作时长：%d分钟%d秒", minutes, seconds);
    SetWindowText(g_appState.hwnd, text);
}

VOID CALLBACK MainWindow::WorkTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (!g_appState.isResting)
    {
        ShowRestWindow();
    }
}

VOID CALLBACK MainWindow::DisplayTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    UpdateWorkTime();
}
void MainWindow::ShowRestWindow()
{
    if (!g_appState.isResting && !g_appState.isPreResting)
    {
        // 先显示预休息窗口
        g_appState.isPreResting = true;
        if (PreRestWindow::Create(false))  // false 表示自动触发
        {
            // 如果用户没有选择延迟
            if (!PreRestWindow::IsDelayed())
            {
                // 显示全屏休息窗口
                g_appState.isResting = true;
                if (RestWindow::Create(g_appState.breakDuration))
                {
                    // 重置计时
                    GetSystemTime(&g_appState.startTime);
                }
            }
            else
            {
                // 用户选择延迟，3分钟后重新提醒
                g_appState.workTimer = SetTimer(
                    g_appState.hwnd,
                    1,  // 使用原来的工作定时器
                    3 * 60 * 1000,  // 3分钟
                    WorkTimerProc
                );
            }
        }
        g_appState.isPreResting = false;
    }
}

void MainWindow::ShowTrayMenu(HWND hwnd, POINT pt)
{
    HMENU hMenu = CreatePopupMenu();
    InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRAY_SHOW, L"显示主窗口");
    InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, IDM_TRAY_REST, L"立即休息");
    InsertMenu(hMenu, 2, MF_BYPOSITION | MF_STRING, IDM_TRAY_RESTART, L"重新计时");
    InsertMenu(hMenu, 3, MF_BYPOSITION | MF_STRING, IDM_TRAY_SETTINGS, L"设置");
    InsertMenu(hMenu, 4, MF_BYPOSITION | MF_STRING, IDM_TRAY_ABOUT, L"关于");
    InsertMenu(hMenu, 5, MF_BYPOSITION | MF_STRING, IDM_TRAY_EXIT, L"退出");

    SetForegroundWindow(hwnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);
}