// PreRestWindow.h
#pragma once
#include "main.h"
#include "RestWindow.h"
#include "MainWindow.h"
#include <windows.h>
#include "resource.h"

class PreRestWindow {
public:
    static bool Create(bool isManual);
    static bool IsDelayed() { return s_isDelayed; }

private:
    static HWND s_hwnd;
    static int s_remainingSeconds;
    static UINT_PTR s_timer;
    static bool s_isManualTriggered;
    static bool s_isDelayed;
    static const wchar_t PRE_REST_WINDOW_CLASS[];

    static void RegisterWindowClass(HINSTANCE hInstance);
    static void CreateControls(HWND hwnd);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static void Close();
};