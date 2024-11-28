#pragma once
// src/MainWindow.h
#pragma once
#include "main.h"
#include "PreRestWindow.h"
#include "RestWindow.h"
#include <windows.h>
#include <shellapi.h>

class MainWindow {
public:
    static bool Create(HINSTANCE hInstance);
    static void InitTimers();
    static void UpdateWorkTime();
    static void ShowRestWindow();
    static void CreateTrayIcon();
    static void UpdateTrayIcon();
    static void ShowTrayMenu(HWND hwnd, POINT pt);
    static void HandleTrayCommand(HWND hwnd, WPARAM wParam);

private:
    static void StartTimer();
    static void StopTimer();
    static void RestartTimer();
    static VOID CALLBACK WorkTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static VOID CALLBACK DisplayTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
};