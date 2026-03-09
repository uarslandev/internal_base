#include "AppState.h"
#include "vars/vars.h"

void AppState::Reset() {
    showMenu = true;
    unlimitedMana = false;
    godMode = false;
    unlimitedStamina = false;
    playerHealth = 100;
    playerMana = 100;
    playerStamina = 100;
    sliderValue = 0.0f;
    counter = 0;
    clearColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
}

void AppState::ApplyCheats() {
    if (!localPlayerPtr) return;

    // Apply cheats here when Player class has the fields
    // if (unlimitedMana) localPlayerPtr->Mana = 100.0f;
    // if (godMode) localPlayerPtr->Health = 100.0f;
    // if (unlimitedStamina) localPlayerPtr->Stamina = 100.0f;
}