// main.cpp
#include "main.h"
#include "PreRestWindow.h"
#include "Settings.h"
#include "About.h"
#include "RestWindow.h"
#include <strsafe.h>

// 全局变量定义
AppState g_appState = { 0 };

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // 初始化默认值
    g_appState.workDuration = 45;
    g_appState.breakDuration = 5;
    Settings::LoadSettings();  // 加载保存的设置

    // 注册窗口类
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WAISTGUARDLITE));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // 创建窗口
    g_appState.hwnd = CreateWindowEx(
        0,                              // 扩展窗口样式
        CLASS_NAME,                     // 窗口类名
        WINDOW_TITLE,                   // 窗口标题
        WS_OVERLAPPEDWINDOW,           // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT,  // 位置
        350, 200,                      // 大小
        NULL,                          // 父窗口
        NULL,                          // 菜单
        hInstance,                     // 实例句柄
        NULL                           // 附加数据
    );

    if (g_appState.hwnd == NULL)
    {
        return 0;
    }

    // 居中窗口
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    RECT rect;
    GetWindowRect(g_appState.hwnd, &rect);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;
    SetWindowPos(g_appState.hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    ShowWindow(g_appState.hwnd, nCmdShow);
    UpdateWindow(g_appState.hwnd);

    // 消息循环
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
        GetSystemTime(&g_appState.startTime);
        g_appState.workTimer = SetTimer(hwnd, 1, g_appState.workDuration * 60 * 1000, NULL);
        g_appState.displayTimer = SetTimer(hwnd, 2, 1000, NULL);

        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 设置文本颜色和背景模式
        SetTextColor(hdc, RGB(51, 51, 51));
        SetBkMode(hdc, TRANSPARENT);

        // 创建字体
        HFONT hFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // 获取窗口客户区大小
        RECT rect;
        GetClientRect(hwnd, &rect);

        // 计算工作时长
        SYSTEMTIME currentTime;
        GetSystemTime(&currentTime);
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

        // 绘制文本
        wchar_t text[64];
        swprintf_s(text, L"已工作时长：%02d:%02d", minutes, seconds);
        DrawText(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // 清理
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_TIMER:
    {
        switch (wParam)
        {
        case 1:  // 工作定时器
            if (!g_appState.isResting && !g_appState.isPreResting)
            {
                PreRestWindow::Create(false);  // 显示预休息窗口
            }
            break;

        case 2:  // 显示定时器
            InvalidateRect(hwnd, NULL, TRUE);  // 刷新显示
            break;
        }
        return 0;
    }

    case WM_TRAYICON:
    {
        if (lParam == WM_RBUTTONUP)
        {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, ID_TRAY_SHOW, L"显示主窗口");
            AppendMenu(hMenu, MF_STRING, ID_TRAY_REST, L"立即休息");
            AppendMenu(hMenu, MF_STRING, ID_TRAY_RESTART, L"重新计时");
            AppendMenu(hMenu, MF_STRING, ID_TRAY_SETTINGS, L"设置");
            AppendMenu(hMenu, MF_STRING, ID_TRAY_ABOUT, L"关于");
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"退出");

            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
        }
        else if (lParam == WM_LBUTTONDBLCLK)
        {
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
        }
        return 0;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_TRAY_SHOW:
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
            break;

        case ID_TRAY_REST:
            PreRestWindow::Create(true);  // 手动触发休息
            break;

        case ID_TRAY_RESTART:
            GetSystemTime(&g_appState.startTime);
            g_appState.isResting = false;
            g_appState.isPreResting = false;
            InvalidateRect(hwnd, NULL, TRUE);
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
        return 0;
    }

    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);  // 点击关闭按钮时隐藏窗口而不是退出
        return 0;

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &g_appState.nid);  // 删除托盘图标
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}