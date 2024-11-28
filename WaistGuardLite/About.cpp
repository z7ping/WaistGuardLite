// About.cpp
#include "About.h"
#include "resource.h"
#include <strsafe.h>

HWND About::s_hwnd = NULL;
const wchar_t About::CLASS_NAME[] = L"WaistGuardLiteAbout";

bool About::Create(HWND parentHwnd)
{
    if (s_hwnd != NULL)
        return false;

    // 注册窗口类
    RegisterWindowClass(GetModuleHandle(NULL));

    // 创建窗口
    s_hwnd = CreateWindowEx(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,  // 扩展样式
        CLASS_NAME,           // 窗口类名
        L"关于",             // 窗口标题
        WS_POPUP | WS_CAPTION | WS_SYSMENU,  // 窗口样式
        0, 0,                // 位置
        400, 300,           // 大小
        parentHwnd,          // 父窗口
        NULL,               // 菜单
        GetModuleHandle(NULL),  // 实例句柄
        NULL                // 附加数据
    );

    if (s_hwnd)
    {
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

void About::RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);
}

void About::CreateControls(HWND hwnd)
{
    // 创建图标
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_WAISTGUARDLITE));
    if (hIcon)
    {
        HWND hIconCtrl = CreateWindow(L"STATIC", NULL,
            WS_CHILD | WS_VISIBLE | SS_ICON,
            20, 20, 32, 32,
            hwnd, NULL, GetModuleHandle(NULL), NULL);
        SendMessage(hIconCtrl, STM_SETICON, (WPARAM)hIcon, 0);
    }

    // 创建文本
    CreateWindow(L"STATIC", L"护腰神器 v1.0",
        WS_CHILD | WS_VISIBLE,
        70, 20, 200, 20,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    CreateWindow(L"STATIC", L"为您的健康保驾护航！",
        WS_CHILD | WS_VISIBLE,
        70, 50, 200, 20,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    CreateWindow(L"STATIC", L"Powered by 程序员七平",
        WS_CHILD | WS_VISIBLE,
        70, 80, 200, 20,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    CreateWindow(L"STATIC", L"联系方式：扫码关注微信公众号：",
        WS_CHILD | WS_VISIBLE,
        20, 120, 360, 20,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    // 创建二维码图片
    HWND hQRCode = CreateWindow(L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
        140, 150, 120, 120,  // 居中显示 // 位置和大小
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    // 加载二维码图片
    HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDB_QRCODE),  // 需要在资源文件中定义
        IMAGE_BITMAP,
        120, 120,  // 期望的大小
        LR_DEFAULTCOLOR);

    if (hBitmap)
    {
        SendMessage(hQRCode, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
    }
}

LRESULT CALLBACK About::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
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