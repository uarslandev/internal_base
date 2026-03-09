#include <Windows.h>
#include <iostream>
#include "core/hooks/hooks.h"
#include "core/dx11/dx.h"
#include "core/vars/vars.h"
#include "core/mem/mem.h"

namespace {
    constexpr const char* ENTITY_PATTERN = "F3 0F 10 53 ?? F3 0F 10 43 ?? F3 0F 5C 47 ??";
    constexpr const char* MODULE_NAME = "SkyrimSE.exe";
}

DWORD WINAPI InitializeInternal(HMODULE hModule) {
    // Allocate console for debugging
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Internal Base - Initialization" << std::endl;
    std::cout << "========================================" << std::endl;

    // Initialize game variables
    GameVars::Initialize();

    // Get module base
    moduleBase = MemoryManager::GetModuleBaseAddress(MODULE_NAME);
    if (moduleBase == 0) {
        std::cerr << "[ERROR] Failed to get module base address" << std::endl;
        return 1;
    }
    std::cout << "[INFO] Module base: 0x" << std::hex << moduleBase << std::dec << std::endl;

    // Initialize hooks manager
    g_hooksManager = new HooksManager();
    if (!g_hooksManager->Initialize(ENTITY_PATTERN)) {
        std::cerr << "[ERROR] Failed to initialize hooks" << std::endl;
        delete g_hooksManager;
        g_hooksManager = nullptr;
        return 1;
    }

    // Start GUI thread
    HANDLE guiThread = CreateThread(nullptr, 0, 
        reinterpret_cast<LPTHREAD_START_ROUTINE>(GUI), 
        hModule, 0, nullptr);
    
    if (guiThread) {
        CloseHandle(guiThread);
        std::cout << "[INFO] GUI thread started successfully" << std::endl;
    } else {
        std::cerr << "[ERROR] Failed to start GUI thread" << std::endl;
    }

    std::cout << "========================================" << std::endl;
    return 0;
}

DWORD WINAPI Cleanup(HMODULE hModule) {
    std::cout << "========================================" << std::endl;
    std::cout << "  Internal Base - Cleanup" << std::endl;
    std::cout << "========================================" << std::endl;

    // Restore hooks
    if (g_hooksManager) {
        g_hooksManager->Restore();
        delete g_hooksManager;
        g_hooksManager = nullptr;
        std::cout << "[INFO] Hooks restored" << std::endl;
    }

    // Close console
    FILE* fp;
    if (freopen_s(&fp, "CONOUT$", "w", stdout) == 0 && fp != nullptr) {
        fclose(fp);
    }
    FreeConsole();

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, 
            reinterpret_cast<LPTHREAD_START_ROUTINE>(InitializeInternal), 
            hModule, 0, nullptr);
        break;
        
    case DLL_PROCESS_DETACH:
        Cleanup(hModule);
        break;
    }
    return TRUE;
}