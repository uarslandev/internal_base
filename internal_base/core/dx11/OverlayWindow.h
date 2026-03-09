#pragma once
#include <Windows.h>

class OverlayWindow {
public:
    OverlayWindow();
    ~OverlayWindow();

    bool Create(float dpiScale);
    void Destroy();
    
    HWND GetHandle() const { return m_hwnd; }
    bool IsVisible() const { return m_visible; }
    
    void UpdatePosition(LPCSTR gameWindowName);
    void ToggleVisibility();

private:
    static LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    RECT GetGameWindowRect(LPCSTR className);

    HWND m_hwnd;
    WNDCLASSEXW m_wc;
    bool m_visible;
};