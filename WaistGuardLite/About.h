// About.h
#pragma once
#include "main.h"

class About {
public:
    static bool Create(HWND parentHwnd);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    static HWND s_hwnd;
    static const wchar_t CLASS_NAME[];

    static void RegisterWindowClass(HINSTANCE hInstance);
    static void CreateControls(HWND hwnd);
};