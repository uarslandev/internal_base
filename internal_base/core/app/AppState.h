#pragma once
#include "imgui/imgui.h"
#include <cstdint>

struct AppState {
    // Menu state
    bool showMenu = true;
    
    // Cheat settings
    bool unlimitedMana = false;
    bool godMode = false;
    bool unlimitedStamina = false;
    
    // Player stats (for display)
    int playerHealth = 100;
    int playerMana = 100;
    int playerStamina = 100;
    
    // View matrix
    uintptr_t viewProjAddress = 0;
    
    // UI settings
    float sliderValue = 0.0f;
    int counter = 0;
    ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Methods
    void Reset();
    void ApplyCheats();
};