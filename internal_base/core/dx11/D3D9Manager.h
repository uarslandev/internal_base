#pragma once
#include <d3d9.h>
#include <Windows.h>

class D3D9Manager {
public:
    D3D9Manager();
    ~D3D9Manager();

    bool Initialize(HWND hWnd);
    void Cleanup();
    void Reset();
    
    LPDIRECT3DDEVICE9 GetDevice() const { return m_pd3dDevice; }
    bool IsDeviceLost() const { return m_deviceLost; }
    void SetDeviceLost(bool lost) { m_deviceLost = lost; }
    
    void HandleDeviceLoss();
    void HandleResize(UINT width, UINT height);

private:
    bool CreateDevice(HWND hWnd);
    void ResetDevice();

    LPDIRECT3D9 m_pD3D;
    LPDIRECT3DDEVICE9 m_pd3dDevice;
    D3DPRESENT_PARAMETERS m_d3dpp;
    bool m_deviceLost;
    UINT m_resizeWidth;
    UINT m_resizeHeight;
};