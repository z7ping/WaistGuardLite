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
        400, 400,           // 高度从450改为400，因为去掉了关闭按钮
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
    // 定义边距和间距
    const int LEFT_MARGIN = 30;
    const int TOP_MARGIN = 25;
    const int CONTENT_SPACING = 15;  // 内容之间的垂直间距
    const int WINDOW_WIDTH = 400;
    const int CONTENT_WIDTH = WINDOW_WIDTH - (LEFT_MARGIN * 2);

    // 创建标题字体（稍大一点）
    HFONT hTitleFont = CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

    // 创建普通文本字体
    HFONT hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

    int currentY = TOP_MARGIN;

    // 创建图标
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_WAISTGUARDLITE));
    if (hIcon)
    {
        HWND hIconCtrl = CreateWindow(L"STATIC", NULL,
            WS_CHILD | WS_VISIBLE | SS_ICON,
            LEFT_MARGIN, currentY, 32, 32,
            hwnd, NULL, GetModuleHandle(NULL), NULL);
        SendMessage(hIconCtrl, STM_SETICON, (WPARAM)hIcon, 0);
    }

    // 标题使用大一点的字体
    HWND hTitle = CreateWindow(L"STATIC", L"护腰神器 v1.0",
        WS_CHILD | WS_VISIBLE,
        LEFT_MARGIN + 40, currentY + 5, 200, 25,  // 图标右侧5像素对齐
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);

    currentY += 45;  // 标题区域高度

    // 标语
    HWND hSlogan = CreateWindow(L"STATIC", L"为您的健康保驾护航！",
        WS_CHILD | WS_VISIBLE,
        LEFT_MARGIN, currentY, CONTENT_WIDTH, 25,
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hSlogan, WM_SETFONT, (WPARAM)hFont, TRUE);

    currentY += 35;  // 标语后间距大一点

    // 作者信息
    HWND hAuthor = CreateWindow(L"STATIC", L"Powered by 程序员七平",
        WS_CHILD | WS_VISIBLE,
        LEFT_MARGIN, currentY, CONTENT_WIDTH, 25,
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hAuthor, WM_SETFONT, (WPARAM)hFont, TRUE);

    currentY += 40;  // 作者信息后间距大一点

    // 联系方式
    HWND hContact = CreateWindow(L"STATIC", L"联系方式：扫码关注微信公众号",
        WS_CHILD | WS_VISIBLE | SS_CENTER,  // 居中对齐
        LEFT_MARGIN, currentY, CONTENT_WIDTH, 25,
        hwnd, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hContact, WM_SETFONT, (WPARAM)hFont, TRUE);

    currentY += 30;

    // 二维码居中显示
    const int QR_SIZE = 160;
    const int QR_X = (WINDOW_WIDTH - QR_SIZE) / 2;
    HWND hQRCode = CreateWindow(L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
        QR_X, currentY, QR_SIZE, QR_SIZE,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    // 加载二维码图片
    HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDB_QRCODE),
        IMAGE_BITMAP,
        QR_SIZE, QR_SIZE,
        LR_DEFAULTCOLOR);

    if (hBitmap)
    {
        SendMessage(hQRCode, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
    }

    currentY += QR_SIZE + 30;  // 二维码下方留出足够空间
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