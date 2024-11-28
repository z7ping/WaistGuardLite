#pragma once
// src/MainWindow.h
#pragma once
#include "main.h"
#include "PreRestWindow.h"
#include "RestWindow.h"
#include "Settings.h"
#include "About.h"
#include "TimerManager.h"
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>  // 添加这行，用于状态栏控件
#pragma comment(lib, "comctl32.lib")  // 链接comctl32库

class MainWindow {
public:
    static bool Create(HINSTANCE hInstance);
    static void InitTimers();
    static void UpdateWorkTime();
    static void ShowRestWindow(bool isManualTrigger = false);
    static void CreateTrayIcon();
    static void UpdateTrayIcon();
    static void ShowTrayMenu(HWND hwnd, POINT pt);
    static void HandleTrayCommand(HWND hwnd, WPARAM wParam);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static VOID CALLBACK WorkTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static VOID CALLBACK DisplayTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static void RestartTimer();

private:
    static void StartTimer();
    static void StopTimer();
};