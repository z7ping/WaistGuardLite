// Settings.cpp
#include "Settings.h"
#include <strsafe.h>

HWND Settings::s_hwnd = NULL;
HWND Settings::s_workDurationEdit = NULL;
HWND Settings::s_breakDurationEdit = NULL;
HWND Settings::s_autoStartCheck = NULL;
HWND Settings::s_tipsEdit = NULL;
const wchar_t Settings::CLASS_NAME[] = L"WaistGuardLiteSettings";

bool Settings::Create(HWND parentHwnd)
{
    if (s_hwnd != NULL)
        return false;

    // ע�ᴰ����
    RegisterWindowClass(GetModuleHandle(NULL));

    // ��������
    s_hwnd = CreateWindowEx(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,  // ��չ��ʽ
        CLASS_NAME,           // ��������
        L"����",             // ���ڱ���
        WS_POPUP | WS_CAPTION | WS_SYSMENU,  // ������ʽ
        0, 0,                // λ��
        500, 400,           // ��С
        parentHwnd,          // ������
        NULL,               // �˵�
        GetModuleHandle(NULL),  // ʵ�����
        NULL                // ��������
    );

    if (s_hwnd)
    {
        // ���д���
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        RECT rect;
        GetWindowRect(s_hwnd, &rect);
        int windowWidth = rect.right - rect.left;
        int windowHeight = rect.bottom - rect.top;
        int x = (screenWidth - windowWidth) / 2;
        int y = (screenHeight - windowHeight) / 2;
        SetWindowPos(s_hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // �����ؼ�
        CreateControls(s_hwnd);

        // ��ʾ����
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
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);
}

void Settings::CreateControls(HWND hwnd)
{
    // ������ǩ�������
    CreateWindow(L"STATIC", L"����ʱ��(����):",
        WS_CHILD | WS_VISIBLE,
        20, 20, 120, 20,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    s_workDurationEdit = CreateWindow(L"EDIT", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        150, 20, 60, 20,
        hwnd, (HMENU)ID_WORK_DURATION, GetModuleHandle(NULL), NULL);

    CreateWindow(L"STATIC", L"��Ϣʱ��(����):",
        WS_CHILD | WS_VISIBLE,
        20, 50, 120, 20,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    s_breakDurationEdit = CreateWindow(L"EDIT", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
        150, 50, 60, 20,
        hwnd, (HMENU)ID_BREAK_DURATION, GetModuleHandle(NULL), NULL);

    s_autoStartCheck = CreateWindow(L"BUTTON", L"����������",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        20, 80, 100, 20,
        hwnd, (HMENU)ID_AUTO_START, GetModuleHandle(NULL), NULL);

    // ������ʾ��༭��
    CreateWindow(L"STATIC", L"��Ϣ��ʾ��:",
        WS_CHILD | WS_VISIBLE,
        20, 110, 100, 20,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    s_tipsEdit = CreateWindow(L"EDIT", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        20, 140, 460, 180,
        hwnd, (HMENU)ID_TIPS_EDIT, GetModuleHandle(NULL), NULL);

    // ������ť
    CreateWindow(L"BUTTON", L"����",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        300, 330, 80, 25,
        hwnd, (HMENU)ID_SAVE_BUTTON, GetModuleHandle(NULL), NULL);

    CreateWindow(L"BUTTON", L"ȡ��",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        400, 330, 80, 25,
        hwnd, (HMENU)ID_CANCEL_BUTTON, GetModuleHandle(NULL), NULL);

    // ���õ�ǰֵ
    wchar_t buffer[16];
    _itow_s(g_appState.workDuration, buffer, 10);
    SetWindowText(s_workDurationEdit, buffer);

    _itow_s(g_appState.breakDuration, buffer, 10);
    SetWindowText(s_breakDurationEdit, buffer);

    SendMessage(s_autoStartCheck, BM_SETCHECK,
        g_appState.autoStart ? BST_CHECKED : BST_UNCHECKED, 0);

    // ������ʾ��
    SetWindowText(s_tipsEdit, L"����������������\r\n\r\n"
        L"��ʱ���ƣ�����׵��´����ʧ��...\r\n\r\n"
        L"�ʵ���Ϣ������߹���Ч��...\r\n\r\n"
        L"վ�����һ�£������屣�ֻ���...\r\n\r\n"
        L"�ǵö��ˮ����������׵...");
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
        s_hwnd = NULL;
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Settings::SaveAndClose(HWND hwnd)
{
    wchar_t buffer[1024];

    // ��ȡ����ʱ��
    GetWindowText(s_workDurationEdit, buffer, 16);
    int workDuration = _wtoi(buffer);
    if (workDuration > 0)
    {
        g_appState.workDuration = workDuration;
    }

    // ��ȡ��Ϣʱ��
    GetWindowText(s_breakDurationEdit, buffer, 16);
    int breakDuration = _wtoi(buffer);
    if (breakDuration > 0)
    {
        g_appState.breakDuration = breakDuration;
    }

    // ��ȡ����������
    bool autoStart = SendMessage(s_autoStartCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;
    if (autoStart != g_appState.autoStart)
    {
        g_appState.autoStart = autoStart;
        SetAutoStart(autoStart);
    }

    // ��������
    SaveSettings();

    // �رմ���
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