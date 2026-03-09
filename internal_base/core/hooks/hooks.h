#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "reclass/reclass.h"

class HooksManager {
public:
    HooksManager();
    ~HooksManager();

    // Initialization and cleanup
    bool Initialize(const std::string& pattern);
    void Restore();
    
    // Entity access
    Player* GetEntityByIndex(int index) const;
    int GetTrackedEntityCount() const;
    void UpdateEntityList();
    void PrintEntityList() const;
    void CleanStaleEntities();
    
    // Direct array access (if needed)
    Player** GetEntityArray() { return m_entities; }
    const Player* const* GetEntityArray() const { return m_entities; }
    
    bool IsInitialized() const { return m_initialized; }

private:
    bool TryReadEntityCoords(Player* ent, float& x, float& y, float& z) const;
    bool WaitForHookActivation(int maxAttempts, int delayMs);
    
    static constexpr int MAX_ENTITIES = 255;
    
    Player* m_entities[MAX_ENTITIES];
    std::vector<unsigned char> m_originalBytes;
    uintptr_t m_hookedAddress;
    bool m_initialized;
};

// Global hooks manager instance
extern HooksManager* g_hooksManager;

// Legacy compatibility functions
Player* GetEntityByIndex(int index);
int GetTrackedEntityCount();
void UpdateEntityList();
void PrintEntityList();
