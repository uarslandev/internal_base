#include "ImGuiManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"

ImGuiManager::ImGuiManager()
    : m_initialized(false) {
}

ImGuiManager::~ImGuiManager() {
    Shutdown();
}

bool ImGuiManager::Initialize(HWND hwnd, LPDIRECT3DDEVICE9 device, float dpiScale) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    SetupStyle(dpiScale);

    if (!ImGui_ImplWin32_Init(hwnd)) {
        return false;
    }
    if (!ImGui_ImplDX9_Init(device)) {
        ImGui_ImplWin32_Shutdown();
        return false;
    }

    m_initialized = true;
    return true;
}

void ImGuiManager::SetupStyle(float dpiScale) {
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(dpiScale);
    style.FontScaleDpi = dpiScale;
}

void ImGuiManager::Shutdown() {
    if (m_initialized) {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        m_initialized = false;
    }
}

void ImGuiManager::NewFrame() {
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render() {
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::InvalidateDeviceObjects() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void ImGuiManager::CreateDeviceObjects() {
    ImGui_ImplDX9_CreateDeviceObjects();
}