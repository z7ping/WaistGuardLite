// RestWindow.h
#pragma once
#include "main.h"
#include "TimerManager.h"
#include <windows.h>

// 前向声明
class MainWindow;

// 控件 ID
#define ID_SKIP_REST 1001

class RestWindow {
public:
    // 定义回调函数类型为公共类型
    typedef VOID (CALLBACK* TimerCallback)(HWND, UINT, UINT_PTR, DWORD);
    
    // 公共静态方法
    static bool Create(int duration);
    static void Close();
    static void SetTimerCallbacks(TimerCallback workProc, TimerCallback displayProc);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    // 私有静态成员变量
    static HWND s_hwnd;
    static int s_remainingSeconds;
    static UINT_PTR s_timer;
    static const wchar_t* s_tips[];
    static int s_currentTipIndex;
    static TimerCallback s_workTimerProc;
    static TimerCallback s_displayTimerProc;

    // 私有静态方法
    static void UpdateCountdown();
    static void UpdateTips();
    static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static void DisableKeyboard();
    static void EnableKeyboard();
    static void RegisterWindowClass(HINSTANCE hInstance);
};

// 窗口类名
extern const wchar_t REST_WINDOW_CLASS[];