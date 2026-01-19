#include <Windows.h>
#include <string>
#include <iostream>
#include <Psapi.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cstdint>
#include "vars/vars.h"
#include <ProcessSnapshot.h>

uintptr_t getModuleBaseAddress(const std::string& moduleName) {
    HMODULE hModule = GetModuleHandleA(moduleName.c_str());
    if (hModule == nullptr) {
        return 0;
    }
    return reinterpret_cast<uintptr_t>(hModule);
}

size_t getModuleSize(uintptr_t baseAddress, const std::string& moduleName) {
    HMODULE hModule = GetModuleHandleA(moduleName.c_str());
    if (hModule) {
        MODULEINFO moduleInfo;
        if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO))) {
            return moduleInfo.SizeOfImage;
        }
    }
    return 0;
}

std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = (uint8_t)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

std::string byteArrayToHexString(const uint8_t* data, size_t size) {
    std::stringstream ss;
    for (size_t i = 0; i < size; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
    }
    return ss.str();
}

//uintptr_t sigScan(uintptr_t baseAddress, size_t size, const std::string& signature, const std::string& mask) {
//    if (signature.empty() || mask.empty() || signature.length() != mask.length()) {
//        return 0; // Invalid input
//    }
//
//    std::vector<uint8_t> signatureBytes = hexStringToBytes(signature);
//    const uint8_t* data = reinterpret_cast<const uint8_t*>(baseAddress);
//
//    for (size_t i = 0; i < size - signatureBytes.size(); ++i) {
//        bool found = true;
//        for (size_t j = 0; j < signatureBytes.size(); ++j) {
//            if (mask[j] != '?' && data[i + j] != signatureBytes[j]) {
//                found = false;
//                break;
//            }
//        }
//        if (found) {
//            return baseAddress + i;
//        }
//    }
//    return 0; // Not found
//}

std::vector<int> IdaToBytes(const std::string& pattern) {

    std::vector<int> bytes;
    std::stringstream ss(pattern);
    std::string word;
    while (ss >> word) {
        if (word == "?" || word == "??") {
            bytes.push_back(-1);
        }
        else {
            int byte = std::stoi(word, nullptr, 16);
            bytes.push_back(byte);
        }
    }

    return bytes;
}

uintptr_t SigScan(uintptr_t module, size_t size, const std::string& pattern) {
    if (module == 0 || size == 0 || pattern == "") return 0;
    
    std::vector<int> bytes = IdaToBytes(pattern);

    for (size_t i = 0; i <= size - bytes.size(); i++) {
        bool found = true;
        for (size_t j = 0; j < bytes.size(); j++) {
            if (bytes[j] != -1) {
                uint8_t byte = *(uint8_t*)(module + i + j);
                if (byte != (uint8_t)bytes[j]) {
                    found = false;
                    break;
                }
            }
        }
        if (found) {
            return module + i;
        }
    }

    return 0;
}

void PlaceJMP32(BYTE* address, DWORD destination, DWORD length) {
    DWORD oldProtect, backup, relAddr;

    VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtect);

    relAddr = (DWORD)(destination - (DWORD)address) - 5; //-5 because rel32 is 5 bytes long (1 byte for 0xE9 and 4 bytes for rel32) and that one is calculated from the next instructions address (address + 5)

    *address = 0xE9; //Relative near jump -> 0xE9 JMP rel32

    *((DWORD*)(address + 0x1)) = relAddr; //next one rel32 is reladdress

    for (DWORD x = 0x5; x < length; x++) {
        *(address + x) = 0x90;
    } //fill the rest of the bytes to do nothing 
    VirtualProtect(address, length, oldProtect, &backup);
}

void PlaceJMP64(BYTE* address, void* destination, DWORD length) {
    DWORD oldProtect;
    VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtect);

    // We need at least 14 bytes for a safe absolute jump in x64
    if (length < 14) {
        // You can choose to error out or pad manually
        VirtualProtect(address, length, oldProtect, &oldProtect);
        return;
    }

    // Absolute jump: mov rax, <destination>; jmp rax
    address[0] = 0x48;                        // REX.W prefix
    address[1] = 0xB8;                        // MOV RAX, imm64
    *((void**)(address + 2)) = destination;   // 8-byte immediate address
    address[10] = 0xFF;                       // JMP RAX
    address[11] = 0xE0;

    // NOP the rest
    for (DWORD i = 12; i < length; i++) {
        address[i] = 0x90;
    }

    DWORD temp;
    VirtualProtect(address, length, oldProtect, &temp);
}



bool Hook64(void* toHook, void* hk_func, int len) {
    if (len < 12) return false; 

    DWORD curProtection;
    VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

    memset(toHook, 0x90, len);

    unsigned char patch[] = {
        0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rax, [64-bit addr]
        0xFF, 0xE0                                                 // jmp rax
    };

    *(DWORD64*)&patch[2] = (DWORD64)hk_func;

    memcpy(toHook, patch, sizeof(patch));

    DWORD temp;
    VirtualProtect(toHook, len, curProtection, &temp);
    return true;
}

bool Unhook64(void* toUnhook, const std::vector<unsigned char>& originalBytes) {
    DWORD curProtection;
    VirtualProtect(toUnhook, originalBytes.size(), PAGE_EXECUTE_READWRITE, &curProtection);
    memcpy(toUnhook, originalBytes.data(), originalBytes.size());
    DWORD temp;
    VirtualProtect(toUnhook, originalBytes.size(), curProtection, &temp);
    return true;
}