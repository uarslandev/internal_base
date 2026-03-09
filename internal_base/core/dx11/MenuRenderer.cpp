#include "MenuRenderer.h"
#include "hooks/hooks.h"

void MenuRenderer::Render(AppState& state, ESPConfig& espConfig, const ImGuiIO& io) {
    if (!state.showMenu) return;

    ImGui::Begin("Internal Base", &state.showMenu);

    RenderCheatMenu(state);
    RenderESPMenu(espConfig);
    RenderInfoSection(state, io);

    ImGui::End();
}

void MenuRenderer::RenderCheatMenu(AppState& state) {
    if (!ImGui::CollapsingHeader("Cheats", ImGuiTreeNodeFlags_DefaultOpen)) return;

    ImGui::Checkbox("God Mode", &state.godMode);
    ImGui::Checkbox("Unlimited Mana", &state.unlimitedMana);
    ImGui::Checkbox("Unlimited Stamina", &state.unlimitedStamina);

    ImGui::Separator();
    
    ImGui::SliderFloat("Test Slider", &state.sliderValue, 0.0f, 1.0f);
    ImGui::ColorEdit3("Clear Color", (float*)&state.clearColor);

    if (ImGui::Button("Test Button")) {
        state.counter++;
    }
    ImGui::SameLine();
    ImGui::Text("Counter: %d", state.counter);

    ImGui::Separator();
    
    ImGui::Text("Health: %d", state.playerHealth);
    ImGui::Text("Mana: %d", state.playerMana);
    ImGui::Text("Stamina: %d", state.playerStamina);
}

void MenuRenderer::RenderESPMenu(ESPConfig& espConfig) {
    if (!ImGui::CollapsingHeader("ESP Settings")) return;

    // Feature toggles
    bool nameEnabled = espConfig.IsFeatureEnabled(ESPFeature::Name);
    bool snaplineEnabled = espConfig.IsFeatureEnabled(ESPFeature::Snapline);
    bool box2DEnabled = espConfig.IsFeatureEnabled(ESPFeature::Box2D);
    bool box3DEnabled = espConfig.IsFeatureEnabled(ESPFeature::Box3D);
    bool skeletonEnabled = espConfig.IsFeatureEnabled(ESPFeature::Skeleton);
    bool healthBarEnabled = espConfig.IsFeatureEnabled(ESPFeature::HealthBar);
    bool distanceEnabled = espConfig.IsFeatureEnabled(ESPFeature::Distance);

    if (ImGui::Checkbox("Show Names", &nameEnabled)) {
        espConfig.ToggleFeature(ESPFeature::Name);
    }
    
    if (ImGui::Checkbox("Show Snaplines", &snaplineEnabled)) {
        espConfig.ToggleFeature(ESPFeature::Snapline);
    }
    
    if (ImGui::Checkbox("Show 2D Box", &box2DEnabled)) {
        espConfig.ToggleFeature(ESPFeature::Box2D);
    }
    
    if (ImGui::Checkbox("Show 3D Box", &box3DEnabled)) {
        espConfig.ToggleFeature(ESPFeature::Box3D);
    }
    
    if (ImGui::Checkbox("Show Skeleton", &skeletonEnabled)) {
        espConfig.ToggleFeature(ESPFeature::Skeleton);
    }
    
    if (ImGui::Checkbox("Show Health Bar", &healthBarEnabled)) {
        espConfig.ToggleFeature(ESPFeature::HealthBar);
    }
    
    if (ImGui::Checkbox("Show Distance", &distanceEnabled)) {
        espConfig.ToggleFeature(ESPFeature::Distance);
    }

    // Color customization
    ImGui::Separator();
    ImGui::Text("Color Customization");
    
    ImGui::ColorEdit4("Name Color", (float*)&espConfig.nameColor);
    ImGui::ColorEdit4("Snapline Color", (float*)&espConfig.snaplineColor);
    ImGui::ColorEdit4("Box Color", (float*)&espConfig.boxColor);
    ImGui::ColorEdit4("Skeleton Color", (float*)&espConfig.skeletonColor);
    
    // Size customization
    ImGui::Separator();
    ImGui::Text("Size Customization");
    
    ImGui::SliderFloat("Box Width", &espConfig.boxWidth, 20.0f, 100.0f);
    ImGui::SliderFloat("Box Height", &espConfig.boxHeight, 40.0f, 150.0f);
    ImGui::SliderFloat("Line Thickness", &espConfig.lineThickness, 0.5f, 5.0f);
}

void MenuRenderer::RenderInfoSection(AppState& state, const ImGuiIO& io) {
    if (!ImGui::CollapsingHeader("Information")) return;

    ImGui::Text("FPS: %.1f (%.3f ms/frame)", io.Framerate, 1000.0f / io.Framerate);
    
    ImGui::Separator();
    
    ImGui::InputScalar("ViewProj Address", ImGuiDataType_U64,
        &state.viewProjAddress, nullptr, nullptr, "%llX",
        ImGuiInputTextFlags_CharsHexadecimal);
    
    ImGui::Text("Tracked Entities: %d", GetTrackedEntityCount());
}