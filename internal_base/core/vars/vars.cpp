#include "vars.h"

// Global variable definitions
DWORD64 moduleBase = 0;
DWORD64 localPlayerBaseAddress = 0;
DWORD64 hookedLocalPlayerBaseAddress = 0;
Player* localPlayerPtr = nullptr;

namespace GameVars {
    void Initialize() {
        moduleBase = 0;
        localPlayerBaseAddress = 0;
        hookedLocalPlayerBaseAddress = 0;
        localPlayerPtr = nullptr;
    }
}