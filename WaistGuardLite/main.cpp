// main.cpp
#include "main.h"
#include "PreRestWindow.h"
#include "Settings.h"
#include "About.h"
#include "RestWindow.h"
#include "MainWindow.h"
#include <strsafe.h>

// 全局变量定义
AppState g_appState = { 0 };

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // 初始化默认值
    g_appState.workDuration = DEFAULT_WORK_MINUTES;
    g_appState.breakDuration = DEFAULT_BREAK_MINUTES;
    g_appState.delayDuration = DEFAULT_DELAY_MINUTES;
    g_appState.startTick = GetTickCount64();  // 使用 GetTickCount64 初始化
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
        350, 225,                      // 高度增加25像素，为状态栏留空间
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
    // 转发给 MainWindow 的窗口过程函数处理
    return MainWindow::WindowProc(hwnd, uMsg, wParam, lParam);
}