#pragma once
#include <windows.h>
#include "reclass/reclass.h"

// Global module information
extern DWORD64 moduleBase;
extern DWORD64 localPlayerBaseAddress;
extern DWORD64 hookedLocalPlayerBaseAddress;
extern Player* localPlayerPtr;

namespace GameVars {
    // Initialization
    void Initialize();
    
    // Module base getters
    inline DWORD64 GetModuleBase() { return moduleBase; }
    inline void SetModuleBase(DWORD64 base) { moduleBase = base; }
    
    // Player getters
    inline Player* GetLocalPlayer() { return localPlayerPtr; }
    inline void SetLocalPlayer(Player* player) { localPlayerPtr = player; }
    
    inline DWORD64 GetLocalPlayerBaseAddress() { return localPlayerBaseAddress; }
    inline void SetLocalPlayerBaseAddress(DWORD64 address) { localPlayerBaseAddress = address; }
}