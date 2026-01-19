#include <Windows.h>
#include <iostream>
#include "vars/vars.h"
#include "mem/mem.h"

DWORD64 jmpBackAddress = 0x7FF71130A9FB + 13;
DWORD64 entityList = 0;

void __declspec(naked) hk_coords() {
    __asm {
        mov eax, [r14]
        mov[rbx + 0x54], eax
        mov eax, [r14 + 0x4]
        mov[rbx + 0x58], eax

        mov entityList, rbx

        jmp[jmpBackAddress]
    }
}

DWORD WINAPI InitiateHooks(HMODULE hModule) {
    //hook coords
    std::string entityListPattern = "41 8B 06 89 43 54 41 8B 46 04 89 43 58 41 8B 46 08 89 43 5C 48 8B 43 68";
    uintptr_t entityAddress = SigScan(moduleBase, sizeof(moduleBase), entityListPattern);
    std::vector<int> entityListBytes = IdaToBytes(entityListPattern);

	//jmpBackAddress = entityAddress + 13;

    Hook64((void*)(entityAddress), hk_coords, 13);
    while (true) {
        std::cout << std::hex << entityList << std::endl;
    }
    return 0;
}

DWORD WINAPI RestoreHooks(HMODULE hModule) {
    std::vector<unsigned char> originalBytes = {
        0x41, 0x8B, 0x06,        // mov eax, [r14]
        0x89, 0x43, 0x54,        // mov [rbx+54h], eax
        0x41, 0x8B, 0x46, 0x04,  // mov eax, [r14+4]
        0x89, 0x43, 0x58        // mov [rbx+58h], eax
    };

    return 0;
}