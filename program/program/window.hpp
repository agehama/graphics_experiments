#pragma once

#include <string>

#include <Windows.h>

class Window
{
public:

    Window() = default;

    Window(int width, int height, const std::wstring& appName = L"");

    ~Window();

    HWND hwnd() { return m_hwnd; }

    int width()const { return m_width; }

    int height()const { return m_height; }

    bool update();

private:

    std::wstring m_appName = L"";

    HINSTANCE m_hinstance = nullptr;

    HWND m_hwnd = nullptr;

    int m_width = 0;

    int m_height = 0;
};
