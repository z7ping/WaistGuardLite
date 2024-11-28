// RestWindow.h
#pragma once
#include "main.h"
#include <windows.h>

class RestWindow {
public:
    static bool Create(int duration);
    static void Close();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    static HWND s_hwnd;
    static int s_remainingSeconds;
    static UINT_PTR s_timer;
    static const wchar_t* s_tips[];
    static int s_currentTipIndex;

    static void UpdateCountdown();
    static void UpdateTips();
    static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static void DisableKeyboard();
    static void EnableKeyboard();
    static void RegisterWindowClass(HINSTANCE hInstance);
};

const wchar_t REST_WINDOW_CLASS[] = L"WaistGuardLiteRest";