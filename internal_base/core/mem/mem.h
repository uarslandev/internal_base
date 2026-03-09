#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <cstdint>

class MemoryManager {
public:
    // Module operations
    static uintptr_t GetModuleBaseAddress(const std::string& moduleName);
    static size_t GetModuleSize(uintptr_t baseAddress, const std::string& moduleName);
    
    // Pattern scanning
    static uintptr_t SigScan(uintptr_t baseAddress, size_t size, const std::string& pattern);
    static std::vector<uint8_t> IdaToBytes(const std::string& pattern);
    
    // Hooking operations
    static bool Hook64(void* toHook, void* hkFunc, int len);
    static bool Unhook64(void* toUnhook, const std::vector<unsigned char>& originalBytes);
    
private:
    MemoryManager() = delete; // Static class, no instantiation
    
    static std::vector<uint8_t> HexStringToBytes(const std::string& hex);
    static std::string ByteArrayToHexString(const uint8_t* data, size_t size);
};

// Legacy function aliases for compatibility
inline uintptr_t getModuleBaseAddress(const std::string& moduleName) {
    return MemoryManager::GetModuleBaseAddress(moduleName);
}

inline size_t getModuleSize(uintptr_t baseAddress, const std::string& moduleName) {
    return MemoryManager::GetModuleSize(baseAddress, moduleName);
}

inline uintptr_t SigScan(uintptr_t baseAddress, size_t size, const std::string& pattern) {
    return MemoryManager::SigScan(baseAddress, size, pattern);
}

inline std::vector<uint8_t> IdaToBytes(const std::string& pattern) {
    return MemoryManager::IdaToBytes(pattern);
}

inline bool Hook64(void* toHook, void* hkFunc, int len) {
    return MemoryManager::Hook64(toHook, hkFunc, len);
}

inline bool Unhook64(void* toUnhook, const std::vector<unsigned char>& originalBytes) {
    return MemoryManager::Unhook64(toUnhook, originalBytes);
}