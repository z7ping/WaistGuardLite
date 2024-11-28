// Settings.h
#pragma once
#include "main.h"

class Settings {
public:
    static bool Create(HWND parentHwnd);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static bool LoadSettings();
    static bool SaveSettings();
    static bool SetAutoStart(bool enable);

private:
    static HWND s_hwnd;
    static HWND s_workDurationEdit;
    static HWND s_breakDurationEdit;
    static HWND s_delayDurationEdit;
    static HWND s_autoStartCheck;
    static HWND s_tipsEdit;
    static const wchar_t CLASS_NAME[];

    static void RegisterWindowClass(HINSTANCE hInstance);
    static void CreateControls(HWND hwnd);
    static void SaveAndClose(HWND hwnd);
};

// ؼ ID
#define ID_WORK_DURATION    1001
#define ID_BREAK_DURATION   1002
#define ID_DELAY_DURATION   1003
#define ID_AUTO_START       1004
#define ID_TIPS_EDIT        1005
#define ID_SAVE_BUTTON      1006
#define ID_CANCEL_BUTTON    1007