#pragma once
#include "imgui/imgui.h"
#include "app/AppState.h"
#include "ESPRenderer.h"

class MenuRenderer {
public:
    MenuRenderer() = default;
    ~MenuRenderer() = default;

    void Render(AppState& state, ESPConfig& espConfig, const ImGuiIO& io);

private:
    void RenderCheatMenu(AppState& state);
    void RenderESPMenu(ESPConfig& espConfig);
    void RenderInfoSection(AppState& state, const ImGuiIO& io);
};