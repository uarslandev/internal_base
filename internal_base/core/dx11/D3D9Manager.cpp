#include "D3D9Manager.h"
#include "imgui/imgui_impl_dx9.h"
#include <iostream>

D3D9Manager::D3D9Manager()
    : m_pD3D(nullptr)
    , m_pd3dDevice(nullptr)
    , m_deviceLost(false)
    , m_resizeWidth(0)
    , m_resizeHeight(0) {
    ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
}

D3D9Manager::~D3D9Manager() {
    Cleanup();
}

bool D3D9Manager::Initialize(HWND hWnd) {
    return CreateDevice(hWnd);
}

bool D3D9Manager::CreateDevice(HWND hWnd) {
    if ((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr) {
        return false;
    }

    ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
    m_d3dpp.Windowed = TRUE;
    m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    m_d3dpp.EnableAutoDepthStencil = TRUE;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_pd3dDevice) < 0) {
        return false;
    }

    return true;
}

void D3D9Manager::Cleanup() {
    if (m_pd3dDevice) {
        m_pd3dDevice->Release();
        m_pd3dDevice = nullptr;
    }
    if (m_pD3D) {
        m_pD3D->Release();
        m_pD3D = nullptr;
    }
}

void D3D9Manager::ResetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = m_pd3dDevice->Reset(&m_d3dpp);
    if (hr == D3DERR_INVALIDCALL) {
        std::cerr << "Device reset failed with D3DERR_INVALIDCALL" << std::endl;
    }
    ImGui_ImplDX9_CreateDeviceObjects();
}

void D3D9Manager::HandleDeviceLoss() {
    if (!m_deviceLost) return;

    HRESULT hr = m_pd3dDevice->TestCooperativeLevel();
    if (hr == D3DERR_DEVICELOST) {
        Sleep(10);
        return;
    }
    if (hr == D3DERR_DEVICENOTRESET) {
        Reset();
    }
    m_deviceLost = false;
}

void D3D9Manager::HandleResize(UINT width, UINT height) {
    if (width != 0 && height != 0) {
        m_d3dpp.BackBufferWidth = width;
        m_d3dpp.BackBufferHeight = height;
        Reset();
    }
}

void D3D9Manager::Reset() {
    ResetDevice();
}