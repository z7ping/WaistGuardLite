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
#include <shlwapi.h>
#include <exception>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

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
    static void LogError(const wchar_t* message);
    static bool CheckSystemState();
    static VOID CALLBACK DelayedRestTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static void ShowAboutInfo();

private:
    static void StartTimer();
    static void StopTimer();
    static HWND hStatus;  // 添加状态栏句柄
    static void HandlePowerBroadcast(WPARAM wParam);
    static void HandleDisplayChange();
    static bool IsFullScreenAppRunning();
    static bool IsOnBatteryPower();
};