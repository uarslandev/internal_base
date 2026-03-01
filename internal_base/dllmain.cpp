#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include "dx11/dx.h"
#include "hooks/hooks.h"
#include "vars/vars.h"
#include "mem/mem.h"

DWORD WINAPI ConsoleThread(HMODULE hModule) {
    // 1. Allocate the console window
    if (AllocConsole()) {
        FILE* fDummy;

        // Reopen stdout, stderr, and stdin to the new console
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONIN$", "r", stdin);

        std::cout << "Internal Base loaded!" << std::endl;

         InitiateHooks(hModule); // Re-enable when hooks are ready
    }

    // Main loop
    while (true)
    {
        if (GetAsyncKeyState(VK_END))
            break;

        Sleep(10); // avoid 100% CPU
    }

    // Cleanup and Unload
     RestoreHooks(hModule); // Re-enable when hooks are ready

    // Proper stream cleanup
    if (stdout) fclose(stdout);
    if (stderr) fclose(stderr);
    if (stdin)  fclose(stdin);

    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //MessageBoxA(nullptr, "DLL_PROCESS_ATTACH called", "Debug", MB_OK);
        // Launch the console/main logic thread
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ConsoleThread, hModule, 0, nullptr));

        // Debug check to see if DllMain is even being hit

        // CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)GUI, hModule, 0, nullptr));
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}