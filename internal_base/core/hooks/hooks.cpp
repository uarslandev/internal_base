#include <Windows.h>
#include <iostream>
#include "vars/vars.h"
#include "mem/mem.h"

DWORD64 jmpBackAddress;
DWORD64 entityList;
Player* ents[255]{};

// Store original bytes when hooking
std::vector<unsigned char> savedOriginalBytes;
uintptr_t hookedAddress = 0;

volatile LONG hookHits = 0;

void __declspec(naked) hk_coords() {
    __asm {
        movss xmm2, [rbx + 0x54]
        movss xmm0, [rbx + 0x58]
        subss xmm0, [rdi + 0x58]

        mov entityList, rbx

        mov eax, hookHits
        inc eax
        mov hookHits, eax

        jmp [jmpBackAddress]
    }
}

static bool TryReadEntityCoords(Player* ent, float& x, float& y, float& z) {
    __try {
        x = ent->coordinates.x;
        y = ent->coordinates.y;
        z = ent->coordinates.z;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

void UpdateEntityList() {
    if (entityList <= 0x10000) return;

    Player* current = reinterpret_cast<Player*>(entityList);

    float x, y, z;
    if (!TryReadEntityCoords(current, x, y, z)) return;

    // already tracked?
    for (int i = 0; i < 255; i++) {
        if (ents[i] == current) return;
    }

    // insert into first free slot
    for (int i = 0; i < 255; i++) {
        if (ents[i] == nullptr) {
            ents[i] = current;
            return;
        }
    }
}

void PrintEntityList() {
    for (int i = 0; i < 255; i++) {
        Player* ent = ents[i];
        if (!ent) continue;

        float x, y, z;
        if (!TryReadEntityCoords(ent, x, y, z)) {
            ents[i] = nullptr; // clean stale pointer
            continue;
        }

        std::cout << "[" << i << "] 0x" << std::hex << (uintptr_t)ent << std::dec
            << " -> X: " << x << " Y: " << y << " Z: " << z << std::endl;
    }
}

DWORD WINAPI InitiateHooks(HMODULE hModule) {
    std::string entityListPattern = "F3 0F 10 53 ?? F3 0F 10 43 ?? F3 0F 5C 47 ??";

    size_t moduleSize = getModuleSize(moduleBase, "SkyrimSE.exe");
    uintptr_t entityAddress = SigScan(moduleBase, moduleSize, entityListPattern);
    int length = (IdaToBytes(entityListPattern)).size();

    if (entityAddress == 0) {
        std::cout << "SigScan failed." << std::endl;
        return 0;
    }

    hookedAddress = entityAddress;
    savedOriginalBytes.resize(length);
    memcpy(savedOriginalBytes.data(), (void*)entityAddress, length);

    jmpBackAddress = entityAddress + length;
    Hook64((void*)(entityAddress), hk_coords, length);

    std::cout << "Hook placed at: 0x" << std::hex << entityAddress << std::dec << std::endl;

	for (int i = 0; i < 400 && hookHits == 0; i++) {
		Sleep(5);
	}

	std::cout << "hookHits: " << hookHits << std::endl;
	std::cout << "captured rbx(entityList var): 0x" << std::hex << entityList << std::dec << std::endl;
	UpdateEntityList();
    return 0;
}

DWORD WINAPI RestoreHooks(HMODULE hModule) {
    if (hookedAddress != 0 && !savedOriginalBytes.empty()) {
        Unhook64((void*)hookedAddress, savedOriginalBytes);
    }
    return 0;
}

Player* GetEntityByIndex(int index) {
    if (index < 0 || index >= 255) return nullptr;
    return ents[index];
}

int GetTrackedEntityCount() {
    int count = 0;
    for (int i = 0; i < 255; i++) {
        if (ents[i] != nullptr) count++;
    }
    return count;
}