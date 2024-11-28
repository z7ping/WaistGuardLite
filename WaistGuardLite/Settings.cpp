// Settings.cpp
#include "Settings.h"
#include "MainWindow.h"
#include <strsafe.h>

HWND Settings::s_hwnd = NULL;
HWND Settings::s_workDurationEdit = NULL;
HWND Settings::s_breakDurationEdit = NULL;
HWND Settings::s_delayDurationEdit = NULL;
HWND Settings::s_autoStartCheck = NULL;
HWND Settings::s_tipsEdit = NULL;
const wchar_t Settings::CLASS_NAME[] = L"WaistGuardLiteSettings";

bool Settings::Create(HWND parentHwnd)
{
    if (s_hwnd != NULL)
        return false;

    // 注册窗口类
    RegisterWindowClass(GetModuleHandle(NULL));

    // 创建窗口
    s_hwnd = CreateWindowEx(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,  // 扩展样式
        CLASS_NAME,           // 窗口类名
        L"设置",             // 窗口标题
        WS_POPUP | WS_CAPTION | WS_SYSMENU,  // 窗口样式
        0, 0,                // 位置
        500, 450,           // 大小
        parentHwnd,          // 父窗口
        NULL,               // 菜单
        GetModuleHandle(NULL),  // 实例句柄
        NULL                // 附加数据
    );

    if (s_hwnd)
    {
        // 禁用父窗口
        if (parentHwnd)
        {
            EnableWindow(parentHwnd, FALSE);
        }

        // 居中窗口
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        RECT rect;
        GetWindowRect(s_hwnd, &rect);
        int windowWidth = rect.right - rect.left;
        int windowHeight = rect.bottom - rect.top;
        int x = (screenWidth - windowWidth) / 2;
        int y = (screenHeight - windowHeight) / 2;
        SetWindowPos(s_hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // 创建控件
        CreateControls(s_hwnd);

        // 显示窗口
        ShowWindow(s_hwnd, SW_SHOW);
        UpdateWindow(s_hwnd);
        return true;
    }

    return false;
}

void Settings::RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(APP_BG_COLOR);  // 使用统一背景色
    RegisterClass(&wc);
}

void Settings::CreateControls(HWND hwnd)
{
    // 统一控件尺寸和间距
    const int LEFT_MARGIN = 30;
    const int RIGHT_MARGIN = 30;
    const int WINDOW_WIDTH = 500;
    const int CONTENT_WIDTH = WINDOW_WIDTH - LEFT_MARGIN - RIGHT_MARGIN;
    const int CONTROL_SPACING = 35;  // 减小控件间距
    const int FIRST_CONTROL_Y = 30;  // 顶部间距
    const int LABEL_WIDTH = 150;     // 标签宽度
    const int EDIT_WIDTH = 100;      // 增加编辑框宽度
    const int CONTROL_HEIGHT = 25;
    const int LABEL_EDIT_SPACING = 10;
    const int QR_SIZE = 120;

    // 创建统一字体
    HFONT hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

    // 第一行：工作时长
    int currentY = FIRST_CONTROL_Y;
    HWND hLabel1 = CreateWindow(L"STATIC", L"工作时长(分钟):",
        WS_CHILD | WS_VISIBLE | SS_LEFT,  // 左对齐
        LEFT_MARGIN, currentY + 2, LABEL_WIDTH, CONTROL_HEIGHT,  // 微调垂直位置
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hLabel1, WM_SETFONT, (WPARAM)hFont, TRUE);

    s_workDurationEdit = CreateWindow(L"EDIT", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT,  // 左对齐
        LEFT_MARGIN + LABEL_WIDTH + LABEL_EDIT_SPACING, currentY, EDIT_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_WORK_DURATION, GetModuleHandle(NULL), NULL);
    SendMessage(s_workDurationEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 第二行：休息时长
    currentY += CONTROL_SPACING;
    HWND hLabel2 = CreateWindow(L"STATIC", L"休息时长(分钟):",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        LEFT_MARGIN, currentY + 2, LABEL_WIDTH, CONTROL_HEIGHT,  // 微调垂直位置
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hLabel2, WM_SETFONT, (WPARAM)hFont, TRUE);

    s_breakDurationEdit = CreateWindow(L"EDIT", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT,  // 左对齐
        LEFT_MARGIN + LABEL_WIDTH + LABEL_EDIT_SPACING, currentY, EDIT_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_BREAK_DURATION, GetModuleHandle(NULL), NULL);
    SendMessage(s_breakDurationEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 第三行：延迟休息时长
    currentY += CONTROL_SPACING;
    HWND hLabel3 = CreateWindow(L"STATIC", L"延迟休息时长(分钟):",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        LEFT_MARGIN, currentY + 2, LABEL_WIDTH, CONTROL_HEIGHT,  // 微调垂直位置
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hLabel3, WM_SETFONT, (WPARAM)hFont, TRUE);

    s_delayDurationEdit = CreateWindow(L"EDIT", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT,  // 左对齐
        LEFT_MARGIN + LABEL_WIDTH + LABEL_EDIT_SPACING, currentY, EDIT_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_DELAY_DURATION, GetModuleHandle(NULL), NULL);
    SendMessage(s_delayDurationEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 第四行：开机自启动
    currentY += CONTROL_SPACING;
    HWND hLabel4 = CreateWindow(L"STATIC", L"开机自启动:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        LEFT_MARGIN, currentY + 2, LABEL_WIDTH, CONTROL_HEIGHT,  // 微调垂直位置
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hLabel4, WM_SETFONT, (WPARAM)hFont, TRUE);

    s_autoStartCheck = CreateWindow(L"BUTTON", L"启用",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        LEFT_MARGIN + LABEL_WIDTH + LABEL_EDIT_SPACING, currentY + 2, 60, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_AUTO_START, GetModuleHandle(NULL), NULL);
    SendMessage(s_autoStartCheck, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 第五行：休息提示语
    currentY += CONTROL_SPACING;
    HWND hLabel5 = CreateWindow(L"STATIC", L"休息提示语:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        LEFT_MARGIN, currentY + 2, LABEL_WIDTH, CONTROL_HEIGHT,
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hLabel5, WM_SETFONT, (WPARAM)hFont, TRUE);

    currentY += CONTROL_HEIGHT + 5;  // 减小标签和文本框之间的间距
    s_tipsEdit = CreateWindow(L"EDIT", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_LEFT,
        LEFT_MARGIN, currentY, WINDOW_WIDTH - LEFT_MARGIN - RIGHT_MARGIN, 150,  // 使用窗口宽度减去左右边距
        hwnd, (HMENU)ID_TIPS_EDIT, GetModuleHandle(NULL), NULL);
    SendMessage(s_tipsEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 二维码部分
    const int QR_X = WINDOW_WIDTH - RIGHT_MARGIN - QR_SIZE;
    const int QR_Y = FIRST_CONTROL_Y;

    // 创建二维码边框
    HWND hQRBorder = CreateWindow(L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME,  // 添加边框样式
        QR_X - 1, QR_Y - 1, QR_SIZE + 2, QR_SIZE + 2,  // 边框比图片大2个像素
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    // 创建二维码图片控件
    HWND hQRCode = CreateWindow(L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP,
        QR_X, QR_Y, QR_SIZE, QR_SIZE,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    // 加载二维码图片
    HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDB_QRCODE),
        IMAGE_BITMAP,
        QR_SIZE, QR_SIZE,
        LR_DEFAULTCOLOR);

    // 设置二维码图片
    SendMessage(hQRCode, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

    // 在二维码下方添加文本说明
    HWND hQRText = CreateWindow(L"STATIC", L"微信扫码联系作者",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        QR_X, QR_Y + QR_SIZE + 5, QR_SIZE, 20,
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hQRText, WM_SETFONT, (WPARAM)hFont, TRUE);  // 使用普通字体

    // 按钮使用稍大的字体
    HFONT hBtnFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

    // 调整按钮位置，增加右边距
    const int BTN_WIDTH = 90;
    const int BTN_HEIGHT = 30;
    const int BTN_SPACING = 10;
    const int TOTAL_BTN_WIDTH = (BTN_WIDTH * 2) + BTN_SPACING;
    const int BTN_START_X = WINDOW_WIDTH - RIGHT_MARGIN - TOTAL_BTN_WIDTH;
    const int BTN_Y = currentY + 160;  // 按钮放在文本框下方

    HWND hSaveBtn = CreateWindow(L"BUTTON", L"保存",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        BTN_START_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT,
        hwnd, (HMENU)ID_SAVE_BUTTON, GetModuleHandle(NULL), NULL);
    SendMessage(hSaveBtn, WM_SETFONT, (WPARAM)hBtnFont, TRUE);

    HWND hCancelBtn = CreateWindow(L"BUTTON", L"取消",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        BTN_START_X + BTN_WIDTH + BTN_SPACING, BTN_Y, BTN_WIDTH, BTN_HEIGHT,
        hwnd, (HMENU)ID_CANCEL_BUTTON, GetModuleHandle(NULL), NULL);
    SendMessage(hCancelBtn, WM_SETFONT, (WPARAM)hBtnFont, TRUE);

    // 设置当前值
    wchar_t buffer[16];
    _itow_s(g_appState.workDuration, buffer, 10);
    SetWindowText(s_workDurationEdit, buffer);

    _itow_s(g_appState.breakDuration, buffer, 10);
    SetWindowText(s_breakDurationEdit, buffer);

    _itow_s(g_appState.delayDuration, buffer, 10);
    SetWindowText(s_delayDurationEdit, buffer);

    SendMessage(s_autoStartCheck, BM_SETCHECK,
        g_appState.autoStart ? BST_CHECKED : BST_UNCHECKED, 0);

    // 设置默认提示语
    SetWindowText(s_tipsEdit,
        L"护腰神器提醒您：长时间的疲劳容易导致错误和失误...\r\n\r\n"
        L"适当休息可以提高工作效率...\r\n\r\n"
        L"站起来活动一下，让身体保持活力...\r\n\r\n"
        L"记得多喝水，保护好腰椎...");

    // 注意：不要删除字体，因为控件还在使用它
    // 字体会在窗口销毁时自动清理
}

LRESULT CALLBACK Settings::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_SAVE_BUTTON:
            SaveAndClose(hwnd);
            return 0;

        case ID_CANCEL_BUTTON:
            DestroyWindow(hwnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        {
            HWND parentHwnd = GetParent(hwnd);
            if (parentHwnd)
            {
                EnableWindow(parentHwnd, TRUE);
                SetForegroundWindow(parentHwnd);
            }
            s_hwnd = NULL;
        }
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Settings::SaveAndClose(HWND hwnd)
{
    wchar_t buffer[1024];

    // 获取工作时长
    GetWindowText(s_workDurationEdit, buffer, 16);
    int workDuration = _wtoi(buffer);
    if (workDuration > 0)
    {
        g_appState.workDuration = workDuration;
    }

    // 获取休息时长
    GetWindowText(s_breakDurationEdit, buffer, 16);
    int breakDuration = _wtoi(buffer);
    if (breakDuration > 0)
    {
        g_appState.breakDuration = breakDuration;
    }

    // 获取延迟时长
    GetWindowText(s_delayDurationEdit, buffer, 16);
    int delayDuration = _wtoi(buffer);
    if (delayDuration > 0)
    {
        g_appState.delayDuration = delayDuration;
    }

    // 获取自启动设置
    bool autoStart = SendMessage(s_autoStartCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;
    if (autoStart != g_appState.autoStart)
    {
        g_appState.autoStart = autoStart;
        SetAutoStart(autoStart);
    }

    // 保存设置
    SaveSettings();

    // 重置定时器
    if (g_appState.workTimer)
    {
        KillTimer(g_appState.hwnd, g_appState.workTimer);
        KillTimer(g_appState.hwnd, g_appState.displayTimer);
        MainWindow::InitTimers();
    }

    // 启用父窗口
    HWND parentHwnd = GetParent(hwnd);
    if (parentHwnd)
    {
        EnableWindow(parentHwnd, TRUE);
        SetForegroundWindow(parentHwnd);
    }

    // 关闭窗口
    DestroyWindow(hwnd);
}

bool Settings::LoadSettings()
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\WaistGuardLite",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD value;
        DWORD size = sizeof(DWORD);

        if (RegQueryValueEx(hKey, L"WorkDuration", NULL, NULL,
            (LPBYTE)&value, &size) == ERROR_SUCCESS)
        {
            g_appState.workDuration = value;
        }

        if (RegQueryValueEx(hKey, L"BreakDuration", NULL, NULL,
            (LPBYTE)&value, &size) == ERROR_SUCCESS)
        {
            g_appState.breakDuration = value;
        }

        if (RegQueryValueEx(hKey, L"DelayDuration", NULL, NULL,
            (LPBYTE)&value, &size) == ERROR_SUCCESS)
        {
            g_appState.delayDuration = value;
        }

        RegCloseKey(hKey);
        return true;
    }
    return false;
}

bool Settings::SaveSettings()
{
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\WaistGuardLite",
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        DWORD value = g_appState.workDuration;
        RegSetValueEx(hKey, L"WorkDuration", 0, REG_DWORD,
            (LPBYTE)&value, sizeof(DWORD));

        value = g_appState.breakDuration;
        RegSetValueEx(hKey, L"BreakDuration", 0, REG_DWORD,
            (LPBYTE)&value, sizeof(DWORD));

        value = g_appState.delayDuration;
        RegSetValueEx(hKey, L"DelayDuration", 0, REG_DWORD,
            (LPBYTE)&value, sizeof(DWORD));

        RegCloseKey(hKey);
        return true;
    }
    return false;
}

bool Settings::SetAutoStart(bool enable)
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        if (enable)
        {
            wchar_t path[MAX_PATH];
            GetModuleFileName(NULL, path, MAX_PATH);
            RegSetValueEx(hKey, L"WaistGuardLite", 0, REG_SZ,
                (LPBYTE)path, static_cast<DWORD>((wcslen(path) + 1) * sizeof(wchar_t)));
        }
        else
        {
            RegDeleteValue(hKey, L"WaistGuardLite");
        }

        RegCloseKey(hKey);
        return true;
    }
    return false;
}