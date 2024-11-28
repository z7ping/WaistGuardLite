// RestWindow.cpp
#include "RestWindow.h"
#include "MainWindow.h"
#include <strsafe.h>

// 定义静态成员变量
HWND RestWindow::s_hwnd = NULL;
int RestWindow::s_remainingSeconds = 0;
UINT_PTR RestWindow::s_timer = 0;
int RestWindow::s_currentTipIndex = 0;
RestWindow::TimerCallback RestWindow::s_workTimerProc = NULL;
RestWindow::TimerCallback RestWindow::s_displayTimerProc = NULL;

// 定义窗口类名
const wchar_t REST_WINDOW_CLASS[] = L"WaistGuardLiteRest";

// 休息提示语
const wchar_t* RestWindow::s_tips[] = {
    L"护腰神器提醒您：长时间的疲劳容易导致错误和失误...",
    L"护腰神器提醒您：适当休息可以提高工作效率...",
    L"护腰神器提醒您：站起来活动一下，让身体保持活力...",
    L"护腰神器提醒您：记得多喝水，保护好腰椎..."
};

bool RestWindow::Create(int duration)
{
    if (s_hwnd != NULL)
        return false;

    // 注册窗口类
    RegisterWindowClass(GetModuleHandle(NULL));

    // 获取屏幕尺寸
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // 创建屏窗口
    s_hwnd = CreateWindowEx(
        WS_EX_TOPMOST,           // 总在最前
        REST_WINDOW_CLASS,       // 窗口类名
        L"休息时间",             // 窗口标题
        WS_POPUP,               // 无边框窗口
        0, 0,                   // 位置
        screenWidth,            // 宽度
        screenHeight,           // 高度
        NULL, NULL,             // 父窗口和菜单
        GetModuleHandle(NULL),  // 实例句柄
        NULL                    // 附加数据
    );

    // 创建跳过按钮
    const int BTN_WIDTH = 120;
    const int BTN_HEIGHT = 35;
    const int BTN_MARGIN_BOTTOM = 150;

    HWND hSkipBtn = CreateWindow(L"BUTTON", L"跳过此次休息",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        (screenWidth - BTN_WIDTH) / 2,  // 居中放置
        screenHeight - BTN_HEIGHT - BTN_MARGIN_BOTTOM,  // 从底部向上150像素
        BTN_WIDTH, BTN_HEIGHT,
        s_hwnd, (HMENU)ID_SKIP_REST, GetModuleHandle(NULL), NULL);

    // 设置按钮字体
    HFONT hBtnFont = CreateFont(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");
    SendMessage(hSkipBtn, WM_SETFONT, (WPARAM)hBtnFont, TRUE);


    if (s_hwnd)
    {
        // 初始化
        s_remainingSeconds = duration * 60;
        s_currentTipIndex = 0;
        
        // 禁用键盘
        DisableKeyboard();
        
        // 创建定时器
        s_timer = SetTimer(s_hwnd, 1, 1000, TimerProc);

        // 显示窗口
        ShowWindow(s_hwnd, SW_SHOW);
        UpdateWindow(s_hwnd);

        return true;
    }
    return false;
}

void RestWindow::RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = REST_WINDOW_CLASS;
    wc.hbrBackground = CreateSolidBrush(APP_BG_COLOR);
    RegisterClass(&wc);
}

LRESULT CALLBACK RestWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 设置文本颜色和背景模式
        SetTextColor(hdc, RGB(236, 65, 65));  // 网易云红色
        SetBkMode(hdc, TRANSPARENT);

        // 创建标题字体
        HFONT hTitleFont = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

        // 创建倒计时字体
        HFONT hCountdownFont = CreateFont(96, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

        // 创建提示文本字体
        HFONT hTipsFont = CreateFont(28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

        // 获取窗口客户区大小
        RECT rect;
        GetClientRect(hwnd, &rect);

        // 绘制标题
        HFONT hOldFont = (HFONT)SelectObject(hdc, hTitleFont);
        RECT titleRect = rect;
        titleRect.top = rect.bottom / 3;
        DrawText(hdc, L"护腰神器提醒您：休息结束时间", -1, &titleRect, DT_CENTER | DT_SINGLELINE);

        // 绘制倒计时
        SelectObject(hdc, hCountdownFont);
        RECT countdownRect = rect;
        countdownRect.top = titleRect.top + 50;
        wchar_t countdownText[32];
        swprintf_s(countdownText, L"%02d:%02d", s_remainingSeconds / 60, s_remainingSeconds % 60);
        DrawText(hdc, countdownText, -1, &countdownRect, DT_CENTER | DT_SINGLELINE);

        // 绘制提示文本
        SetTextColor(hdc, RGB(51, 51, 51));  // 深灰色
        SelectObject(hdc, hTipsFont);
        RECT tipsRect = rect;
        tipsRect.top = countdownRect.top + 100;
        tipsRect.left = rect.right / 4;
        tipsRect.right = rect.right * 3 / 4;
        DrawText(hdc, s_tips[s_currentTipIndex], -1, &tipsRect, DT_CENTER | DT_WORDBREAK);

        // 清理
        SelectObject(hdc, hOldFont);
        DeleteObject(hTitleFont);
        DeleteObject(hCountdownFont);
        DeleteObject(hTipsFont);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        return 0;  // 忽略所有键盘输入

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_SKIP_REST)
        {
            Close();  // 直接调用 Close 函数处理所有清理工作
        }
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

VOID CALLBACK RestWindow::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    s_remainingSeconds--;
    if (s_remainingSeconds <= 0)
    {
        Close();  // 只在 Close 中处理重置逻辑
        return;
    }

    if (s_remainingSeconds % 5 == 0)  // 每5秒更新一次提示语
    {
        s_currentTipIndex = (s_currentTipIndex + 1) % (sizeof(s_tips) / sizeof(s_tips[0]));
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

void RestWindow::Close()
{
    if (s_hwnd)
    {
        // 停止计时器
        KillTimer(s_hwnd, s_timer);

        // 启用键盘
        EnableKeyboard();

        // 销毁窗口
        DestroyWindow(s_hwnd);
        s_hwnd = NULL;

        // 重置状态
        g_appState.isResting = false;
        g_appState.isPreResting = false;

        // 使用 TimerManager 重置计时器
        TimerManager::RestartTimer();

        // 更新托盘图标状态
        StringCchCopy(g_appState.nid.szTip, ARRAYSIZE(g_appState.nid.szTip), WINDOW_TITLE);
        Shell_NotifyIcon(NIM_MODIFY, &g_appState.nid);
    }
}

void RestWindow::DisableKeyboard()
{
    // 使用更温和的方式：设置窗口置顶和全屏
    SetWindowPos(s_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void RestWindow::EnableKeyboard()
{
    // 不需要做任何事情
}

void RestWindow::SetTimerCallbacks(TimerCallback workProc, TimerCallback displayProc)
{
    s_workTimerProc = workProc;
    s_displayTimerProc = displayProc;
}