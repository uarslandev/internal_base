#include <Windows.h>
#include <iostream>
#include "vars/vars.h"
#include "mem/mem.h"

//DWORD64 jmpBackAddress = 0x7FF766F3A9FB + 0x13; // 0x7FF71130AA0E
DWORD64 jmpBackAddress; // 
DWORD64 entityList = 0;

void __declspec(naked) hk_coords() {
    __asm {
        movss xmm2, [rbx + 54]
        movss xmm0, [rbx + 58]
        subss xmm0,[rdi+58]

        mov entityList, rbx

        jmp[jmpBackAddress]
    }
}

DWORD WINAPI InitiateHooks(HMODULE hModule) {
    //hook coords
    std::string entityListPattern = "F3 0F 10 53 ?? F3 0F 10 43 ?? F3 0F 5C 47 ??";
    uintptr_t entityAddress = SigScan(moduleBase, sizeof(moduleBase), entityListPattern);
    std::vector<int> entityListBytes = IdaToBytes(entityListPattern);

	jmpBackAddress = entityAddress + 15;

    Hook64((void*)(entityAddress), hk_coords, 15);

    //while (true) {
    //    //std::cout << std::hex << entityList << std::endl;
    //    std::cout << std::hex << entityAddress << std::endl;
    //}
    return 0;
}

DWORD WINAPI RestoreHooks(HMODULE hModule) {
    std::vector<unsigned char> originalBytes = {
        0xF3, 0xF, 0x10, 0x53, 0x54,        // mov eax, [r14]
        0xF3, 0xF, 0x10, 0x43, 0x58,        // mov eax, [r14]
        0xF3, 0xF, 0x5C, 0x47, 0x58        // mov eax, [r14]
    };
    std::string entityListPattern = "F3 0F 10 53 ?? F3 0F 10 43 ?? F3 0F 5C 47 ??";
    uintptr_t entityAddress = SigScan(moduleBase, sizeof(moduleBase), entityListPattern);
    std::vector<int> entityListBytes = IdaToBytes(entityListPattern);

	Unhook64((void*)(entityAddress), originalBytes);

    return 0;
}