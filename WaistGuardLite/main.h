// main.h
#pragma once
#include <windows.h>
#include <shellapi.h>
#include "resource.h"

// 常量定义
const wchar_t CLASS_NAME[] = L"WaistGuardLite";
const wchar_t WINDOW_TITLE[] = L"护腰神器 v1.0";

// 默认时长设置
#define DEFAULT_WORK_MINUTES  45   // 默认工作时长
#define DEFAULT_BREAK_MINUTES 5    // 默认休息时长

// 托盘菜单命令
#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_SHOW      1001
#define ID_TRAY_REST      1002
#define ID_TRAY_RESTART   1003
#define ID_TRAY_SETTINGS  1004
#define ID_TRAY_ABOUT     1005
#define ID_TRAY_EXIT      1006

// 全局状态
struct AppState {
    HWND hwnd;              // 主窗口句柄
    UINT_PTR workTimer;     // 工作定时器
    UINT_PTR displayTimer;  // 显示定时器
    SYSTEMTIME startTime;   // 开始时间
    bool isResting;         // 是否在休息
    bool isPreResting;      // 是否在预休息
    int workDuration;       // 工作时长（分钟）
    int breakDuration;      // 休息时长（分钟）
    int delayDuration;      // 延迟休息时长（分钟）
    bool autoStart;         // 开机自启动
    NOTIFYICONDATA nid;     // 托盘图标数据
};

// 全局变量
extern AppState g_appState;

// 窗口过程函数声明
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);