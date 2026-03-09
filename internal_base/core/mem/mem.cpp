#include "mem.h"
#include <iostream>
#include <Psapi.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

uintptr_t MemoryManager::GetModuleBaseAddress(const std::string& moduleName) {
    HMODULE hModule = GetModuleHandleA(moduleName.c_str());
    return hModule ? reinterpret_cast<uintptr_t>(hModule) : 0;
}

size_t MemoryManager::GetModuleSize(uintptr_t baseAddress, const std::string& moduleName) {
    HMODULE hModule = GetModuleHandleA(moduleName.c_str());
    if (!hModule) return 0;

    MODULEINFO moduleInfo;
    if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO))) {
        return moduleInfo.SizeOfImage;
    }
    return 0;
}

std::vector<uint8_t> MemoryManager::HexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    bytes.reserve(hex.length() / 2);
    
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        bytes.push_back(static_cast<uint8_t>(std::stoul(byteString, nullptr, 16)));
    }
    return bytes;
}

std::string MemoryManager::ByteArrayToHexString(const uint8_t* data, size_t size) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (size_t i = 0; i < size; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
        if (i < size - 1) ss << " ";
    }
    return ss.str();
}

std::vector<uint8_t> MemoryManager::IdaToBytes(const std::string& pattern) {
    std::vector<uint8_t> bytes;
    std::istringstream stream(pattern);
    std::string byteStr;

    while (stream >> byteStr) {
        if (byteStr == "??") {
            bytes.push_back(0x00); // Wildcard
        } else {
            bytes.push_back(static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16)));
        }
    }
    return bytes;
}

uintptr_t MemoryManager::SigScan(uintptr_t baseAddress, size_t size, const std::string& pattern) {
    std::vector<uint8_t> patternBytes = IdaToBytes(pattern);
    const size_t patternSize = patternBytes.size();

    if (patternSize == 0) return 0;

    uint8_t* scanStart = reinterpret_cast<uint8_t*>(baseAddress);
    uint8_t* scanEnd = scanStart + size - patternSize;

    for (uint8_t* current = scanStart; current <= scanEnd; ++current) {
        bool found = true;
        
        for (size_t i = 0; i < patternSize; ++i) {
            if (patternBytes[i] != 0x00 && current[i] != patternBytes[i]) {
                found = false;
                break;
            }
        }
        
        if (found) {
            return reinterpret_cast<uintptr_t>(current);
        }
    }
    return 0;
}

bool MemoryManager::Hook64(void* toHook, void* hkFunc, int len) {
    if (len < 12) {
        std::cerr << "Hook length must be at least 12 bytes" << std::endl;
        return false;
    }

    DWORD oldProtect;
    if (!VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        std::cerr << "Failed to change memory protection" << std::endl;
        return false;
    }

    // NOP out the bytes
    memset(toHook, 0x90, len);

    // Create absolute jump
    unsigned char patch[] = {
        0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rax, [addr]
        0xFF, 0xE0                                                  // jmp rax
    };

    *reinterpret_cast<DWORD64*>(&patch[2]) = reinterpret_cast<DWORD64>(hkFunc);
    memcpy(toHook, patch, sizeof(patch));

    DWORD temp;
    VirtualProtect(toHook, len, oldProtect, &temp);
    return true;
}

bool MemoryManager::Unhook64(void* toUnhook, const std::vector<unsigned char>& originalBytes) {
    if (originalBytes.empty()) {
        std::cerr << "Original bytes vector is empty" << std::endl;
        return false;
    }

    DWORD oldProtect;
    if (!VirtualProtect(toUnhook, originalBytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        std::cerr << "Failed to change memory protection" << std::endl;
        return false;
    }

    memcpy(toUnhook, originalBytes.data(), originalBytes.size());
    
    DWORD temp;
    VirtualProtect(toUnhook, originalBytes.size(), oldProtect, &temp);
    return true;
}