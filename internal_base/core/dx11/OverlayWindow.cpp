#include "OverlayWindow.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

OverlayWindow::OverlayWindow()
    : m_hwnd(nullptr)
    , m_visible(false) {
    ZeroMemory(&m_wc, sizeof(m_wc));
}

OverlayWindow::~OverlayWindow() {
    Destroy();
}

bool OverlayWindow::Create(float dpiScale) {
    m_wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, WindowProc, 0L, 0L,
             GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
             L"ImGui Overlay", nullptr };
    
    ::RegisterClassExW(&m_wc);

    m_hwnd = ::CreateWindowW(m_wc.lpszClassName, L"Overlay",
        WS_POPUP, 100, 100,
        (int)(1280 * dpiScale), (int)(800 * dpiScale),
        nullptr, nullptr, m_wc.hInstance, nullptr);

    if (!m_hwnd) return false;

    SetWindowLong(m_hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE);
    SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), 255, LWA_COLORKEY);
    SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    ::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(m_hwnd);

    return true;
}

void OverlayWindow::Destroy() {
    if (m_hwnd) {
        ::DestroyWindow(m_hwnd);
        ::UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
        m_hwnd = nullptr;
    }
}

void OverlayWindow::UpdatePosition(LPCSTR gameWindowName) {
    RECT gameRect = GetGameWindowRect(gameWindowName);
    MoveWindow(m_hwnd, gameRect.left, gameRect.top,
        gameRect.right - gameRect.left, gameRect.bottom - gameRect.top, true);
}

void OverlayWindow::ToggleVisibility() {
    if (GetAsyncKeyState(VK_INSERT) & 1) {
        m_visible = !m_visible;
        LONG ex = GetWindowLong(m_hwnd, GWL_EXSTYLE);

        if (m_visible) {
            ex &= ~WS_EX_TRANSPARENT;
            SetWindowLong(m_hwnd, GWL_EXSTYLE, ex);
            ShowWindow(m_hwnd, SW_SHOW);
            SetForegroundWindow(m_hwnd);
            SetFocus(m_hwnd);
        }
        else {
            ex |= WS_EX_TRANSPARENT;
            SetWindowLong(m_hwnd, GWL_EXSTYLE, ex);
            ShowWindow(m_hwnd, SW_HIDE);
        }
    }
}

RECT OverlayWindow::GetGameWindowRect(LPCSTR className) {
    RECT rect = { 0 };
    HWND hwnd = FindWindowA(nullptr, className);
    if (hwnd) {
        ::GetWindowRect(hwnd, &rect);
    }
    return rect;
}

LRESULT WINAPI OverlayWindow::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}