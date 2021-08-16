#include "window.hpp"

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

Window::Window(int width, int height, const std::wstring& appName)
    : m_appName(appName)
    , m_hinstance(GetModuleHandle(nullptr))
    , m_width(width)
    , m_height(height)
{
    WNDCLASSEX wc =
    {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = m_hinstance,
        .hIcon = LoadIcon(nullptr, IDI_WINLOGO),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)),
        .lpszMenuName = nullptr,
        .lpszClassName = m_appName.c_str(),
        .hIconSm = wc.hIcon,
    };

    RegisterClassEx(&wc);

    RECT rect = { 0,0,m_width,m_height };

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    const int adjustedWidth = rect.right - rect.left;
    const int adjustedHeight = rect.bottom - rect.top;

    const int posX = (GetSystemMetrics(SM_CXSCREEN) - adjustedWidth) / 2;
    const int posY = (GetSystemMetrics(SM_CYSCREEN) - adjustedHeight) / 2;

    m_hwnd = CreateWindowEx(
        WS_EX_APPWINDOW, m_appName.c_str(), m_appName.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        posX, posY, adjustedWidth, adjustedHeight,
        nullptr, nullptr, m_hinstance, nullptr);

    ShowWindow(m_hwnd, SW_SHOW);

    SetForegroundWindow(m_hwnd);

    SetFocus(m_hwnd);
}

Window::~Window()
{
    UnregisterClassW(m_appName.c_str(), m_hinstance);
}

bool Window::update()
{
    MSG msg = {};

    if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (msg.message == WM_QUIT)
        {
            return false;
        }
    }

    return true;
}
