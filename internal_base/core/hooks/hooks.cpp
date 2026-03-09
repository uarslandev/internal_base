#include "hooks.h"
#include "vars/vars.h"
#include "mem/mem.h"
#include <iostream>

// These must have C linkage to match the assembly EXTERN declarations
extern "C" {
    DWORD64 jmpBackAddress = 0;
    DWORD64 entityList = 0;
    volatile LONG hookHits = 0;
}

// Declare the external assembly function
extern "C" void hk_coords();

// Global instance
HooksManager* g_hooksManager = nullptr;

HooksManager::HooksManager()
    : m_entities{}
    , m_hookedAddress(0)
    , m_initialized(false) {
}

HooksManager::~HooksManager() {
    Restore();
}

bool HooksManager::Initialize(const std::string& pattern) {
    if (m_initialized) {
        std::cerr << "[HooksManager] Already initialized" << std::endl;
        return false;
    }

    std::cout << "[HooksManager] Initializing with pattern: " << pattern << std::endl;

    // Get module size and scan for pattern
    size_t moduleSize = MemoryManager::GetModuleSize(moduleBase, "SkyrimSE.exe");
    uintptr_t entityAddress = MemoryManager::SigScan(moduleBase, moduleSize, pattern);
    int length = static_cast<int>(MemoryManager::IdaToBytes(pattern).size());

    if (entityAddress == 0) {
        std::cerr << "[HooksManager] SigScan failed" << std::endl;
        return false;
    }

    // Save original bytes
    m_hookedAddress = entityAddress;
    m_originalBytes.resize(length);
    memcpy(m_originalBytes.data(), reinterpret_cast<void*>(entityAddress), length);

    // Setup jump back address
    jmpBackAddress = entityAddress + length;
    
    // Place hook
    if (!MemoryManager::Hook64(reinterpret_cast<void*>(entityAddress), hk_coords, length)) {
        std::cerr << "[HooksManager] Hook64 failed" << std::endl;
        return false;
    }

    std::cout << "[HooksManager] Hook placed at: 0x" << std::hex << entityAddress << std::dec << std::endl;

    // Wait for hook to be hit
    if (!WaitForHookActivation(400, 5)) {
        std::cerr << "[HooksManager] Warning: Hook not activated within timeout" << std::endl;
    }

    std::cout << "[HooksManager] Hook hits: " << hookHits << std::endl;
    std::cout << "[HooksManager] Captured entityList: 0x" << std::hex << entityList << std::dec << std::endl;
    
    UpdateEntityList();
    m_initialized = true;
    
    std::cout << "[HooksManager] Initialization complete" << std::endl;
    return true;
}

void HooksManager::Restore() {
    if (!m_initialized) return;

    if (m_hookedAddress != 0 && !m_originalBytes.empty()) {
        std::cout << "[HooksManager] Restoring original bytes" << std::endl;
        MemoryManager::Unhook64(reinterpret_cast<void*>(m_hookedAddress), m_originalBytes);
    }
    
    m_initialized = false;
}

bool HooksManager::WaitForHookActivation(int maxAttempts, int delayMs) {
    for (int i = 0; i < maxAttempts && hookHits == 0; i++) {
        Sleep(delayMs);
    }
    return hookHits > 0;
}

bool HooksManager::TryReadEntityCoords(Player* ent, float& x, float& y, float& z) const {
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

void HooksManager::UpdateEntityList() {
    if (entityList <= 0x10000) {
        return;
    }

    Player* current = reinterpret_cast<Player*>(entityList);

    // Validate entity coordinates
    float x, y, z;
    if (!TryReadEntityCoords(current, x, y, z)) {
        return;
    }

    // Check if already tracked
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (m_entities[i] == current) {
            return;
        }
    }

    // Insert into first free slot
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (m_entities[i] == nullptr) {
            m_entities[i] = current;
            std::cout << "[HooksManager] Tracked new entity at index " << i 
                      << " (0x" << std::hex << reinterpret_cast<uintptr_t>(current) << std::dec << ")" << std::endl;
            return;
        }
    }
    
    std::cerr << "[HooksManager] Warning: Entity list is full" << std::endl;
}

void HooksManager::PrintEntityList() const {
    std::cout << "[HooksManager] === Entity List ===" << std::endl;
    
    int count = 0;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        Player* ent = m_entities[i];
        if (!ent) continue;

        float x, y, z;
        if (!TryReadEntityCoords(ent, x, y, z)) {
            std::cout << "[" << i << "] 0x" << std::hex << reinterpret_cast<uintptr_t>(ent) 
                      << std::dec << " -> INVALID" << std::endl;
            continue;
        }

        std::cout << "[" << i << "] 0x" << std::hex << reinterpret_cast<uintptr_t>(ent) << std::dec
                  << " -> X: " << x << " Y: " << y << " Z: " << z << std::endl;
        count++;
    }
    
    std::cout << "[HooksManager] Total valid entities: " << count << std::endl;
}

void HooksManager::CleanStaleEntities() {
    int cleanedCount = 0;
    
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!m_entities[i]) continue;
        
        float x, y, z;
        if (!TryReadEntityCoords(m_entities[i], x, y, z)) {
            m_entities[i] = nullptr;
            cleanedCount++;
        }
    }
    
    if (cleanedCount > 0) {
        std::cout << "[HooksManager] Cleaned " << cleanedCount << " stale entities" << std::endl;
    }
}

Player* HooksManager::GetEntityByIndex(int index) const {
    if (index < 0 || index >= MAX_ENTITIES) {
        return nullptr;
    }
    return m_entities[index];
}

int HooksManager::GetTrackedEntityCount() const {
    int count = 0;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (m_entities[i] != nullptr) {
            count++;
        }
    }
    return count;
}

// Legacy compatibility functions
Player* GetEntityByIndex(int index) {
    return g_hooksManager ? g_hooksManager->GetEntityByIndex(index) : nullptr;
}

int GetTrackedEntityCount() {
    return g_hooksManager ? g_hooksManager->GetTrackedEntityCount() : 0;
}

void UpdateEntityList() {
    if (g_hooksManager) {
        g_hooksManager->UpdateEntityList();
    }
}

void PrintEntityList() {
    if (g_hooksManager) {
        g_hooksManager->PrintEntityList();
    }
}