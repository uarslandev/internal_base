#include "dx.h"
#include "D3D9Manager.h"
#include "ImGuiManager.h"
#include "ESPRenderer.h"
#include "OverlayWindow.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "vars/vars.h"
#include "hooks/hooks.h"
#include <iostream>

namespace {
    constexpr LPCSTR GAME_NAME = "Skyrim Special Edition";
    constexpr float VIEW_MATRIX_ADDRESS = 0x7FF72116C3E8;
}

// Application state
struct AppState {
    bool showMenu = true;
    bool unlimitedMana = false;
    bool drawSnaplines = true;
    unsigned long long viewProjAddress = VIEW_MATRIX_ADDRESS;
    int playerHealth = 100;
    int playerMana = 100;
    int playerStamina = 100;
    float sliderValue = 0.0f;
    int counter = 0;
    ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
};

void RenderMenu(AppState& state, const ImGuiIO& io) {
    if (!state.showMenu) return;

    ImGui::Begin("Welcome to Internal Base!");

    ImGui::Text("Unlimited Mana");
    ImGui::Checkbox("Unlimited Mana", &state.unlimitedMana);

    ImGui::SliderFloat("float", &state.sliderValue, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", (float*)&state.clearColor);

    if (ImGui::Button("Button"))
        state.counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", state.counter);

    ImGui::Text("Health: %d", state.playerHealth);
    ImGui::Text("Mana: %d", state.playerMana);
    ImGui::Text("Stamina: %d", state.playerStamina);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / io.Framerate, io.Framerate);

    ImGui::Checkbox("Draw Snaplines", &state.drawSnaplines);
    ImGui::InputScalar("ViewProj Matrix Address", ImGuiDataType_U64,
        &state.viewProjAddress, nullptr, nullptr, "%llX",
        ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::Text("Tracked entities: %d", GetTrackedEntityCount());

    ImGui::End();
}

void ApplyGameCheats(AppState& state) {
    if (!localPlayerPtr) return;

    // Uncomment when implemented:
    // if (state.unlimitedMana) localPlayerPtr->Mana = 100.0f;
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

    // Initialize ESP renderer
    ESPRenderer espRenderer;

    // Application state
    AppState appState;

    // Main loop
    bool running = true;
    while (running) {
        // Exit check
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            break;
        }

        // Update overlay position
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

        // Begin frame
        imguiManager.NewFrame();

        // Get screen dimensions
        ImGuiIO& io = ImGui::GetIO();
        espRenderer.SetScreenDimensions((int)io.DisplaySize.x, (int)io.DisplaySize.y);
        espRenderer.SetViewMatrix((float*)appState.viewProjAddress);

        // Render menu
        RenderMenu(appState, io);

        // Render ESP
        if (appState.drawSnaplines) {
            espRenderer.RenderESP();
        }

        // Apply cheats
        ApplyGameCheats(appState);

        // End frame and render
        ImGui::EndFrame();

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