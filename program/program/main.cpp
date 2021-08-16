#include "logger.hpp"
#include "window.hpp"
#include "dx.hpp"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    ClearLog();

    Window window(1600, 900, L"program");

    Dx dx;
    if (!dx.init(window.hwnd(), window.width(), window.height()))
    {
        return 1;
    }

    while (window.update())
    {
        dx.frameBegin();
        dx.frameEnd();
    }

    return 0;
}
