// src/MainWindow.cpp
#include <stddef.h>
#include "main.h"
#include "MainWindow.h"
#include <strsafe.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

static_assert(offsetof(AppState, startTick) >= 0, "AppState must have startTick member");

// 定义静态成员变量
HWND MainWindow::hStatus = NULL;

void MainWindow::LogError(const wchar_t* message)
{
    wchar_t logPath[MAX_PATH];
    GetModuleFileName(NULL, logPath, MAX_PATH);
    PathRemoveFileSpec(logPath);
    PathAppend(logPath, L"error.log");
    
    FILE* file;
    if (_wfopen_s(&file, logPath, L"a") == 0) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        fwprintf(file, L"[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond,
            message);
        fclose(file);
    }
}

bool MainWindow::Create(HINSTANCE hInstance)
{
    try {
        // 注册窗口类
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WAISTGUARDLITE));
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(APP_BG_COLOR);
        if (!RegisterClass(&wc)) {
            LogError(L"Failed to register window class");
            return false;
        }

        // 创建窗口
        g_appState.hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            WINDOW_TITLE,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            400, 225,
            NULL,
            NULL,
            hInstance,
            NULL
        );

        if (!g_appState.hwnd) {
            LogError(L"Failed to create main window");
            return false;
        }

        // 初始化应用状态
        g_appState.workDuration = DEFAULT_WORK_MINUTES;
        g_appState.breakDuration = DEFAULT_BREAK_MINUTES;

        // 设置休息窗口的定时器回调
        RestWindow::SetTimerCallbacks(WorkTimerProc, DisplayTimerProc);

        // 在 Create 函数中初始化
        g_appState.startTick = GetTickCount64();

        return true;
    }
    catch (const std::exception& e) {
        char buffer[256];
        sprintf_s(buffer, "Exception: %s", e.what());
        LogError(L"Unexpected error occurred");
        return false;
    }
}

void MainWindow::InitTimers()
{
    TimerManager::StartTimer();
}

void MainWindow::CreateTrayIcon()
{
    // 添加更详细的程序信息
    ZeroMemory(&g_appState.nid, sizeof(NOTIFYICONDATA));
    g_appState.nid.cbSize = sizeof(NOTIFYICONDATA);
    g_appState.nid.hWnd = g_appState.hwnd;
    g_appState.nid.uID = 1;
    g_appState.nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;  // 添加 NIF_INFO
    g_appState.nid.uCallbackMessage = WM_TRAYICON;
    g_appState.nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_WAISTGUARDLITE));
    
    // 添加更详细的提示信息
    StringCchCopy(g_appState.nid.szTip, ARRAYSIZE(g_appState.nid.szTip), 
        L"护腰神器 - 保护您的健康\n点击查看详情");
    
    // 首次显示气泡提示
    StringCchCopy(g_appState.nid.szInfoTitle, ARRAYSIZE(g_appState.nid.szInfoTitle), 
        L"护腰神器已启动");
    StringCchCopy(g_appState.nid.szInfo, ARRAYSIZE(g_appState.nid.szInfo), 
        L"程序将在后台运行，定时提醒您休息\n右键点击托盘图标可查看更多选项");
    g_appState.nid.dwInfoFlags = NIIF_INFO;
    
    Shell_NotifyIcon(NIM_ADD, &g_appState.nid);
}

void MainWindow::UpdateTrayIcon()
{
    // 根据状态更新托盘图标提示文本
    if (g_appState.isResting)
    {
        StringCchCopy(g_appState.nid.szTip, ARRAYSIZE(g_appState.nid.szTip), L"正在休息...");
    }
    else if (g_appState.isPreResting)
    {
        StringCchCopy(g_appState.nid.szTip, ARRAYSIZE(g_appState.nid.szTip), L"即将开始休息...");
    }
    else
    {
        // 计算当前工作时长
        ULONGLONG currentTime = GetTickCount64();
        ULONGLONG elapsedTime = (currentTime - g_appState.startTick) / 1000;
        int minutes = elapsedTime / 60;
        int seconds = elapsedTime % 60;
        
        wchar_t tipText[128];
        StringCchPrintf(tipText, ARRAYSIZE(tipText), 
            L"已工作：%02d:%02d", minutes, seconds);
        StringCchCopy(g_appState.nid.szTip, ARRAYSIZE(g_appState.nid.szTip), tipText);
    }
    Shell_NotifyIcon(NIM_MODIFY, &g_appState.nid);
}

void MainWindow::UpdateWorkTime()
{
    if (!g_appState.hwnd || g_appState.isResting) return;

    static ULONGLONG lastUpdateTime = 0;
    ULONGLONG currentTime = GetTickCount64();
    
    // 计算时间
    ULONGLONG elapsedTime = (currentTime - g_appState.startTick) / 1000;
    int elapsedMinutes = (int)(elapsedTime / 60);
    int remainingMinutes = g_appState.workDuration - elapsedMinutes;
    int seconds = (int)(elapsedTime % 60);
    
    if (remainingMinutes < 0) remainingMinutes = 0;
    
    // 每100ms更新一次UI
    if (currentTime - lastUpdateTime >= 100)
    {
        // 更新窗口标题
        wchar_t title[256];
        StringCchPrintf(title, ARRAYSIZE(title), 
            L"护腰神器 - By：程序员七平");
        SetWindowText(g_appState.hwnd, title);
        
        // 更新状态栏
        wchar_t status[256];
        StringCchPrintf(status, ARRAYSIZE(status),
            L" 工作时长：%d分钟  休息时长：%d分钟  下次休息还有：%d分钟",
            g_appState.workDuration, g_appState.breakDuration, remainingMinutes);
        
        SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)status);

        // 更新托盘图标
        UpdateTrayIcon();

        // 强制重绘主窗口
        InvalidateRect(g_appState.hwnd, NULL, TRUE);

        lastUpdateTime = currentTime;
    }
    
    // 提前提醒功能
    if (remainingMinutes == 5 && !g_appState.isPreResting) {
        g_appState.nid.dwInfoFlags = NIIF_INFO;
        StringCchCopy(g_appState.nid.szInfoTitle, ARRAYSIZE(g_appState.nid.szInfoTitle),
            L"休息提醒");
        StringCchCopy(g_appState.nid.szInfo, ARRAYSIZE(g_appState.nid.szInfo),
            L"还有5分钟就要休息了，请注意安排工作进度");
        Shell_NotifyIcon(NIM_MODIFY, &g_appState.nid);
    }
}

VOID CALLBACK MainWindow::DisplayTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    UpdateWorkTime();
}

VOID CALLBACK MainWindow::WorkTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (!g_appState.isResting && !g_appState.isPreResting)
    {
        ShowRestWindow(false);
    }
}

bool MainWindow::CheckSystemState()
{
    // 使用 WM_POWERBROADCAST 和 WM_DISPLAYCHANGE 消息来监听系统状态变化
    static bool isFullScreen = false;
    static bool isOnBattery = false;

    // 检查全屏状态
    HWND foregroundWindow = GetForegroundWindow();
    if (foregroundWindow) {
        MONITORINFO mi = { sizeof(MONITORINFO) };
        HMONITOR hMonitor = MonitorFromWindow(foregroundWindow, MONITOR_DEFAULTTOPRIMARY);
        if (GetMonitorInfo(hMonitor, &mi)) {
            RECT windowRect;
            GetWindowRect(foregroundWindow, &windowRect);
            
            // 比较窗口是否占据整个屏幕
            isFullScreen = (windowRect.left == mi.rcMonitor.left &&
                          windowRect.top == mi.rcMonitor.top &&
                          windowRect.right == mi.rcMonitor.right &&
                          windowRect.bottom == mi.rcMonitor.bottom);
        }
    }

    // 检查电源状态
    SYSTEM_POWER_STATUS powerStatus;
    if (GetSystemPowerStatus(&powerStatus)) {
        isOnBattery = (powerStatus.ACLineStatus == 0);
    }

    // 根据状态调整行为
    if (isFullScreen) {
        // 如果是全屏状态，推迟提醒
        SetTimer(g_appState.hwnd, 3, 5 * 60 * 1000, DelayedRestTimerProc);
        return false; // 不显示休息提醒
    }

    if (isOnBattery) {
        // 如果使用电池，可以调整提醒间隔
        // 这里可以根据需要调整工作时长
        g_appState.workDuration = (g_appState.workDuration * 3) / 2; // 延长50%
    }

    return true; // 可以显示休息提醒
}

void MainWindow::ShowRestWindow(bool isManualTrigger)
{
    if (!isManualTrigger && !CheckSystemState()) {
        // 推迟提醒
        SetTimer(g_appState.hwnd, 3, 5 * 60 * 1000, DelayedRestTimerProc);
        return;
    }
    
    if (!g_appState.isResting && !g_appState.isPreResting)
    {
        // 停止当前计时器
        KillTimer(g_appState.hwnd, g_appState.workTimer);
        KillTimer(g_appState.hwnd, g_appState.displayTimer);

        // 显示预休息窗口
        g_appState.isPreResting = true;
        if (PreRestWindow::Create(isManualTrigger))
        {
            // 预休息窗口会行处理后续逻辑
            // 不需要在这里处理休息窗口的显示
            
            // 更新托盘图标状态
            StringCchCopy(g_appState.nid.szTip, ARRAYSIZE(g_appState.nid.szTip), L"即将开始休息...");
            UpdateTrayIcon();
        }
        else
        {
            // 如果预休息窗口创建失败，重置状态
            g_appState.isPreResting = false;
            RestartTimer();
        }
    }
}

void MainWindow::ShowTrayMenu(HWND hwnd, POINT pt)
{
    HMENU hMenu = CreatePopupMenu();
    
    // 计算当前工作时长
    ULONGLONG currentTime = GetTickCount64();
    ULONGLONG elapsedTime = (currentTime - g_appState.startTick) / 1000;  // 转换为秒
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;
    
    // 添加工作时长信息到菜单
    wchar_t timeInfo[64];
    swprintf_s(timeInfo, L"已工作：%02d:%02d", minutes, seconds);
    InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING | MF_GRAYED, 0, timeInfo);
    
    // 添加其他菜单项
    InsertMenu(hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hMenu, 2, MF_BYPOSITION | MF_STRING, ID_TRAY_SHOW, L"显示主窗口");
    InsertMenu(hMenu, 3, MF_BYPOSITION | MF_STRING | (g_appState.isResting ? MF_GRAYED : 0), 
        ID_TRAY_REST, L"立即休息");
    InsertMenu(hMenu, 4, MF_BYPOSITION | MF_STRING, ID_TRAY_RESTART, L"重新计时");
    InsertMenu(hMenu, 5, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hMenu, 6, MF_BYPOSITION | MF_STRING, ID_TRAY_SETTINGS, L"设置");
    InsertMenu(hMenu, 7, MF_BYPOSITION | MF_STRING, ID_TRAY_ABOUT, L"关于");
    InsertMenu(hMenu, 8, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hMenu, 9, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, L"退出");

    // 设置默认菜单项
    SetMenuDefaultItem(hMenu, ID_TRAY_SHOW, FALSE);

    // 确保窗口处于前台
    SetForegroundWindow(hwnd);

    // 显示菜单
    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_RIGHTBUTTON,
        pt.x, pt.y, 0, hwnd, NULL);

    // 清理
    PostMessage(hwnd, WM_NULL, 0, 0);
    DestroyMenu(hMenu);
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hStatus;  // 将变量声明移到 switch 外部
    RECT rect;
    POINT pt;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        // 创建托盘图标
        ZeroMemory(&g_appState.nid, sizeof(NOTIFYICONDATA));
        g_appState.nid.cbSize = sizeof(NOTIFYICONDATA);
        g_appState.nid.hWnd = hwnd;
        g_appState.nid.uID = 1;
        g_appState.nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        g_appState.nid.uCallbackMessage = WM_TRAYICON;
        g_appState.nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_WAISTGUARDLITE));
        StringCchCopy(g_appState.nid.szTip, ARRAYSIZE(g_appState.nid.szTip), WINDOW_TITLE);
        Shell_NotifyIcon(NIM_ADD, &g_appState.nid);

        // 初始化定时器
        g_appState.startTick = GetTickCount64();
        g_appState.workTimer = SetTimer(hwnd, 1, g_appState.workDuration * 60 * 1000, WorkTimerProc);
        g_appState.displayTimer = SetTimer(hwnd, 2, 100, DisplayTimerProc);  // 使用100ms更新频率

        // 初始化通用控件
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_BAR_CLASSES;
        InitCommonControlsEx(&icex);

        // 创建状态栏
        hStatus = CreateWindowEx(
            0,
            STATUSCLASSNAME,
            NULL,
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            hwnd,
            (HMENU)IDC_STATUSBAR,
            GetModuleHandle(NULL),
            NULL);

        if (hStatus)
        {
            // 设置状态栏文本
            const wchar_t* tipText = L"双击托盘图标显示主窗口 | 右键托盘图标可设置、立即休息等";
            SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)tipText);
        }
        return 0;
    }

    case WM_COMMAND:
        // 处理菜单命令
        HandleTrayCommand(hwnd, wParam);
        return 0;

    case WM_TRAYICON:
        switch (lParam)
        {
        case WM_RBUTTONUP:
        case WM_CONTEXTMENU:
            GetCursorPos(&pt);
            ShowTrayMenu(hwnd, pt);
            return 0;

        case WM_LBUTTONDBLCLK:
            ShowWindow(hwnd, IsWindowVisible(hwnd) ? SW_HIDE : SW_RESTORE);
            if (IsWindowVisible(hwnd))
            {
                SetForegroundWindow(hwnd);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }

        GetClientRect(hwnd, &rect);
        hStatus = GetDlgItem(hwnd, IDC_STATUSBAR);
        if (hStatus)
        {
            SetWindowPos(hStatus, HWND_BOTTOM, 0, rect.bottom - 25, rect.right, 25, SWP_NOACTIVATE);
        }
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 设置文本颜色和背景模式
        SetTextColor(hdc, RGB(51, 51, 51));
        SetBkMode(hdc, TRANSPARENT);

        // 计算当前工作时长
        ULONGLONG currentTime = GetTickCount64();
        ULONGLONG elapsedTime = (currentTime - g_appState.startTick) / 1000;
        int minutes = elapsedTime / 60;
        int seconds = elapsedTime % 60;

        // 创建字体
        HFONT hFont = CreateFont(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // 获取窗口客户区大小并调整绘制区域
        RECT rect;
        GetClientRect(hwnd, &rect);
        rect.bottom -= 25;  // 为状态栏留出空间
        rect.top += 10;     // 上方留出一些空间

        // 绘制工作时长
        wchar_t timeText[64];
        swprintf_s(timeText, L"已工作时长：%02d:%02d", minutes, seconds);
        DrawText(hdc, timeText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // 清理
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_GETMINMAXINFO:
    {
        // 设置窗口最小尺寸
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = 400;  // 最小宽度从500改为400
        mmi->ptMinTrackSize.y = 225;  // 最小高度保持不变
        return 0;
    }

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &g_appState.nid);
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);  // 点击闭按钮时隐藏窗口而不是退出
        return 0;

    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH hBrush = CreateSolidBrush(APP_BG_COLOR);
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
        return TRUE;
    }

    case WM_POWERBROADCAST:
        switch (wParam)
        {
            case PBT_APMPOWERSTATUSCHANGE:
                // 电源状态改变
                CheckSystemState();
                return TRUE;

            case PBT_APMRESUMEAUTOMATIC:
                // 从睡眠状态恢复
                RestartTimer();
                return TRUE;
        }
        break;

    case WM_DISPLAYCHANGE:
        // 显示设置改变（分辨率、颜色深度等）
        CheckSystemState();
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void MainWindow::HandleTrayCommand(HWND hwnd, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case ID_TRAY_SHOW:
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        break;

    case ID_TRAY_REST:
        if (!g_appState.isResting && !g_appState.isPreResting)
        {
            // 停止当前计时器
            KillTimer(g_appState.hwnd, g_appState.workTimer);
            KillTimer(g_appState.hwnd, g_appState.displayTimer);
            
            // 显示预休息窗口，标记为手动触发
            g_appState.isPreResting = true;
            if (PreRestWindow::Create(true))  // true 表示手动触发
            {
                // 预休息窗口会自行处理后续逻辑
                // 不需要在这里处理休息窗口的显示
            }
        }
        break;

    case ID_TRAY_RESTART:
        RestartTimer();
        break;

    case ID_TRAY_SETTINGS:
        Settings::Create(hwnd);
        break;

    case ID_TRAY_ABOUT:
        About::Create(hwnd);
        break;

    case ID_TRAY_EXIT:
        DestroyWindow(hwnd);
        break;
    }
}

void MainWindow::RestartTimer()
{
    TimerManager::RestartTimer();
}

void MainWindow::ShowAboutInfo()
{
    // 显示简单的 Windows 消息框
    wchar_t aboutText[512];
    swprintf_s(aboutText, 
        L"WaistGuard Lite\n"
        L"版本: %d.%d.%d.%d\n"
        L"公司: %s\n"
        L"描述: %s",
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_REVISION,
        VERSION_BUILD,
        L"程序员七平",
        L"WaistGuard Lite - 腰部健康保护应用");
    
    MessageBox(NULL, aboutText, L"关于", MB_OK | MB_ICONINFORMATION);
}

VOID CALLBACK MainWindow::DelayedRestTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    KillTimer(hwnd, idEvent);
    ShowRestWindow(false);
}