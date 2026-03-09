#pragma once
#include <Windows.h>
#include <d3d9.h>

class ImGuiManager {
public:
    ImGuiManager();
    ~ImGuiManager();

    bool Initialize(HWND hwnd, LPDIRECT3DDEVICE9 device, float dpiScale);
    void Shutdown();
    void NewFrame();
    void Render();
    
    void InvalidateDeviceObjects();
    void CreateDeviceObjects();

private:
    void SetupStyle(float dpiScale);
    
    bool m_initialized;
};