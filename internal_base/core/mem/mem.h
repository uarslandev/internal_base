#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <Psapi.h>
#include <ProcessSnapshot.h>
#include <vector>

struct HookInfo {
    void* toHookAddress;
    std::vector<unsigned char> originalBytes;
    DWORD originalProtection;
    bool isHooked;

    HookInfo() : toHookAddress(nullptr), originalProtection(0), isHooked(false) {}
};


uintptr_t getModuleBaseAddress(const std::string& moduleName);
size_t getModuleSize(uintptr_t baseAddress, const std::string& moduleName);
std::string byteArrayToHexString(const uint8_t* data, size_t size);
//uintptr_t sigScan(uintptr_t baseAddress, size_t size, const std::string& signature, const std::string& mask);
std::vector<int> IdaToBytes(const std::string& pattern);
uintptr_t SigScan(uintptr_t module, size_t size, const std::string& pattern);
void PlaceJMP32(BYTE* address, DWORD destination, DWORD length);
void PlaceJMP64(BYTE* address, void* destination, DWORD length);
bool Hook64(void* toHook, void* hk_func, int len);
bool Unhook64(void* toUnhook, const std::vector<unsigned char>& originalBytes);