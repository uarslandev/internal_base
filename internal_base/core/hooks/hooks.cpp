#include <Windows.h>
#include <iostream>
#include "vars/vars.h"

DWORD64 jmpBackAddress = moduleBase + 0x74452C + 15;
DWORD64 g_raxAddress = 0;

void __declspec(naked) hk_coords() {

    __asm {
        movss xmm5, [rax + 0x54] // Original instruction 1
        movss xmm4, [rax + 0x58] // Original instruction 2
        movss xmm3, [rax + 0x5C] // Original instruction 3
        mov g_raxAddress, rax // Store the RAX value in the global variable

        jmp[jmpBackAddress]
    }
}