// PreRestWindow.h
#pragma once
#include "main.h"

class PreRestWindow {
public:
    static bool Create(bool isManual = false);
    static void Close();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static bool IsDelayed() { return s_isDelayed; }

private:
    static HWND s_hwnd;
    static int s_remainingSeconds;
    static UINT_PTR s_timer;
    static bool s_isManualTriggered;
    static bool s_isDelayed;

    static void UpdateCountdown();
    static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static void RegisterWindowClass(HINSTANCE hInstance);
    static void CreateDelayButton(HWND hwnd);
};

const wchar_t PRE_REST_WINDOW_CLASS[] = L"WaistGuardLitePreRest";