#include <iostream>
#include <io.h>
#include <fcntl.h>

#include "WindowSetup.h"

// Handles everything that happens within the window if and when an instance is created
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Switch case that checks if the window instance is closed or not
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool SetWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window)
{
    // Create a console to write couts such as if a file exists 
    AllocConsole();
    HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    int hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
    FILE* fp = _fdopen(hConsole, "w");
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    // Register window class
    const wchar_t CLASS_NAME[] = L"3DProject Win Class";

    WNDCLASS winClass = {};

    winClass.lpfnWndProc = WindowProc; // Window procedure pointer
    winClass.hInstance = instance; // Set instance. Value is taken from wWinMain
    winClass.lpszClassName = CLASS_NAME; // String identifier

    RegisterClass(&winClass);

    window = CreateWindowEx(0, CLASS_NAME, L"3D Project", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, instance, nullptr);

    if (window == nullptr)
    {
        std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
        return false;
    }

    ShowWindow(window, nCmdShow);
    return true;
}
