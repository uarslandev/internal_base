#include <Windows.h>
#include <iostream>
#include "vars/vars.h"
#include "mem/mem.h"

DWORD64 jmpBackAddress = moduleBase + 17;
DWORD64 entityList = 0;

//void __declspec(naked) hk_coords() {
//
//    __asm {
//        movss xmm5, [rax + 0x54] // Original instruction 1
//        movss xmm4, [rax + 0x58] // Original instruction 2
//        movss xmm3, [rax + 0x5C] // Original instruction 3
//        mov g_raxAddress, rax // Store the RAX value in the global variable
//
//        jmp[jmpBackAddress]
//    }
//}
void __declspec(naked) hk_coords() {

    __asm {
        mov [rbx + 0x54], eax
        mov eax, [r14 + 4h]
        mov [rbx + 0x58], eax
        mov eax, [r14 + 8h]
        mov [rbx + 0x5C], eax
        mov entityList, rbx 

        jmp[jmpBackAddress]
    }
}

//89 43 54 41 8B 46 04 89 43 58 41 8B 46 08 89 43 5C
DWORD WINAPI InitiateHooks(HMODULE hModule) {
    //hook coords
    uintptr_t entityList = sigScan(
        moduleBase,
        sizeof(moduleBase),
        "894354418B4604894358418B460889435C",
        "xxxxxxxxxxxxxxxxx"
    );

	Hook64((void*)(moduleBase + entityList), hk_coords, 17);
    return 0;
}