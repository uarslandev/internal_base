#pragma once
#include "imgui/imgui.h"
#include "math/math.h"
#include "reclass/reclass.h"

enum class ESPFeature {
    Name = 1 << 0,
    Snapline = 1 << 1,
    Box2D = 1 << 2,
    Box3D = 1 << 3,
    Skeleton = 1 << 4,
    HealthBar = 1 << 5,
    Distance = 1 << 6
};

struct ESPConfig {
    int enabledFeatures = 0;
    ImU32 nameColor = IM_COL32(255, 255, 255, 255);
    ImU32 snaplineColor = IM_COL32(255, 0, 0, 255);
    ImU32 boxColor = IM_COL32(0, 255, 0, 255);
    ImU32 skeletonColor = IM_COL32(255, 255, 0, 255);
    float boxWidth = 50.0f;
    float boxHeight = 80.0f;
    float lineThickness = 1.5f;

    bool IsFeatureEnabled(ESPFeature feature) const {
        return (enabledFeatures & static_cast<int>(feature)) != 0;
    }

    void EnableFeature(ESPFeature feature) {
        enabledFeatures |= static_cast<int>(feature);
    }

    void DisableFeature(ESPFeature feature) {
        enabledFeatures &= ~static_cast<int>(feature);
    }

    void ToggleFeature(ESPFeature feature) {
        enabledFeatures ^= static_cast<int>(feature);
    }
};

class ESPRenderer {
public:
    ESPRenderer();

    void SetViewMatrix(float* matrix) { m_viewMatrix = matrix; }
    void SetScreenDimensions(int width, int height);
    void RenderESP();
    
    ESPConfig& GetConfig() { return m_config; }
    const ESPConfig& GetConfig() const { return m_config; }

private:
    bool TryReadEntityCoords(Player* ent, float& x, float& y, float& z);
    bool IsValidViewMatrix(float* matrix);
    float CalculateDistance(const Vector3& pos1, const Vector3& pos2);
    
    // ESP drawing functions
    void DrawName(ImDrawList* drawList, const Vector2& screenPos, const char* name);
    void DrawSnapline(ImDrawList* drawList, const Vector2& screenPos);
    void Draw2DBox(ImDrawList* drawList, const Vector2& screenPos);
    void Draw3DBox(ImDrawList* drawList, const Vector3& worldPos);
    void DrawSkeleton(ImDrawList* drawList, const Vector3& worldPos);
    void DrawHealthBar(ImDrawList* drawList, const Vector2& screenPos, float healthPercent);
    void DrawDistance(ImDrawList* drawList, const Vector2& screenPos, float distance);

    float* m_viewMatrix;
    int m_screenWidth;
    int m_screenHeight;
    ESPConfig m_config;
};