#include "dx.h"
#include "D3D9Manager.h"
#include "ImGuiManager.h"
#include "ESPRenderer.h"
#include "OverlayWindow.h"
#include "MenuRenderer.h"
#include "app/AppState.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "vars/vars.h"
#include "hooks/hooks.h"
#include <iostream>

namespace {
    constexpr LPCSTR GAME_NAME = "Skyrim Special Edition";
    constexpr uintptr_t DEFAULT_VIEW_MATRIX_ADDRESS = 0x7FF72116C3E8;
}

DWORD WINAPI GUI(HMODULE hModule, int, char**) {
    // Initialize DPI awareness
    ImGui_ImplWin32_EnableDpiAwareness();
    float dpiScale = ImGui_ImplWin32_GetDpiScaleForMonitor(
        ::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Create overlay window
    OverlayWindow overlayWindow;
    if (!overlayWindow.Create(dpiScale)) {
        std::cerr << "Failed to create overlay window" << std::endl;
        return 1;
    }

    // Initialize DirectX
    D3D9Manager d3dManager;
    if (!d3dManager.Initialize(overlayWindow.GetHandle())) {
        std::cerr << "Failed to initialize DirectX" << std::endl;
        overlayWindow.Destroy();
        return 1;
    }

    // Initialize ImGui
    ImGuiManager imguiManager;
    if (!imguiManager.Initialize(overlayWindow.GetHandle(), d3dManager.GetDevice(), dpiScale)) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        d3dManager.Cleanup();
        overlayWindow.Destroy();
        return 1;
    }

    // Initialize renderers
    ESPRenderer espRenderer;
    MenuRenderer menuRenderer;

    // Application state
    AppState appState;
    appState.viewProjAddress = DEFAULT_VIEW_MATRIX_ADDRESS;

    // Main loop
    bool running = true;
    while (running) {
        // Exit check
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            break;
        }

        // Update overlay position and visibility
        overlayWindow.UpdatePosition(GAME_NAME);
        overlayWindow.ToggleVisibility();

        // Handle Windows messages
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                running = false;
            }
        }
        if (!running) break;

        // Handle device loss
        d3dManager.HandleDeviceLoss();

        // Begin ImGui frame
        imguiManager.NewFrame();

        // Get screen dimensions
        ImGuiIO& io = ImGui::GetIO();
        
        // Update ESP renderer
        espRenderer.SetScreenDimensions(static_cast<int>(io.DisplaySize.x), 
                                       static_cast<int>(io.DisplaySize.y));
        espRenderer.SetViewMatrix(reinterpret_cast<float*>(appState.viewProjAddress));

        // Render menu
        menuRenderer.Render(appState, espRenderer.GetConfig(), io);

        // Render ESP
        espRenderer.RenderESP();

        // Apply cheats
        appState.ApplyCheats();

        // End frame and render
        ImGui::EndFrame();

        // DirectX rendering
        LPDIRECT3DDEVICE9 device = d3dManager.GetDevice();
        device->SetRenderState(D3DRS_ZENABLE, FALSE);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

        D3DCOLOR clearColorDx = D3DCOLOR_RGBA(0, 0, 0, 0);
        device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clearColorDx, 1.0f, 0);

        if (device->BeginScene() >= 0) {
            imguiManager.Render();
            device->EndScene();
        }

        HRESULT result = device->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST) {
            d3dManager.SetDeviceLost(true);
        }

        Sleep(10);
    }

    // Cleanup
    imguiManager.Shutdown();
    d3dManager.Cleanup();
    overlayWindow.Destroy();

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}