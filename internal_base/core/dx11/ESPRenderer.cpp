#include "ESPRenderer.h"
#include "hooks/hooks.h"
#include <cmath>
#include <string>

ESPRenderer::ESPRenderer()
    : m_viewMatrix(nullptr)
    , m_screenWidth(0)
    , m_screenHeight(0) {
    
    // Disable all features by default to prevent crashes
    m_config.enabledFeatures = 0;
}

void ESPRenderer::SetScreenDimensions(int width, int height) {
    m_screenWidth = width;
    m_screenHeight = height;
}

bool ESPRenderer::TryReadEntityCoords(Player* ent, float& x, float& y, float& z) {
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

bool ESPRenderer::IsValidViewMatrix(float* matrix) {
    if (!matrix) return false;
    
    __try {
        // Try to read first value to check if memory is accessible
        volatile float test = matrix[0];
        (void)test;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

float ESPRenderer::CalculateDistance(const Vector3& pos1, const Vector3& pos2) {
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float dz = pos2.z - pos1.z;
    return std::sqrtf(dx * dx + dy * dy + dz * dz);
}

void ESPRenderer::DrawName(ImDrawList* drawList, const Vector2& screenPos, const char* name) {
    ImVec2 textSize = ImGui::CalcTextSize(name);
    ImVec2 textPos(screenPos.x - textSize.x * 0.5f, screenPos.y - 20.0f);
    
    // Draw text shadow for better visibility
    drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), 
        IM_COL32(0, 0, 0, 255), name);
    drawList->AddText(textPos, m_config.nameColor, name);
}

void ESPRenderer::DrawSnapline(ImDrawList* drawList, const Vector2& screenPos) {
    // Draw line from bottom center of screen to entity
    ImVec2 start(m_screenWidth * 0.5f, static_cast<float>(m_screenHeight));
    ImVec2 end(screenPos.x, screenPos.y);
    drawList->AddLine(start, end, m_config.snaplineColor, m_config.lineThickness);
}

void ESPRenderer::Draw2DBox(ImDrawList* drawList, const Vector2& screenPos) {
    float halfWidth = m_config.boxWidth * 0.5f;
    float halfHeight = m_config.boxHeight * 0.5f;
    
    ImVec2 topLeft(screenPos.x - halfWidth, screenPos.y - halfHeight);
    ImVec2 bottomRight(screenPos.x + halfWidth, screenPos.y + halfHeight);
    
    drawList->AddRect(topLeft, bottomRight, m_config.boxColor, 0.0f, 0, m_config.lineThickness);
}

void ESPRenderer::Draw3DBox(ImDrawList* drawList, const Vector3& worldPos) {
    if (!m_viewMatrix || !IsValidViewMatrix(m_viewMatrix)) return;

    // Define 3D box dimensions (adjust based on your game's scale)
    const float width = 1.0f;
    const float height = 2.0f;
    const float depth = 1.0f;

    // 8 corners of the box
    Vector3 corners[8] = {
        { worldPos.x - width, worldPos.y - height, worldPos.z - depth }, // Bottom
        { worldPos.x + width, worldPos.y - height, worldPos.z - depth },
        { worldPos.x + width, worldPos.y - height, worldPos.z + depth },
        { worldPos.x - width, worldPos.y - height, worldPos.z + depth },
        { worldPos.x - width, worldPos.y + height, worldPos.z - depth }, // Top
        { worldPos.x + width, worldPos.y + height, worldPos.z - depth },
        { worldPos.x + width, worldPos.y + height, worldPos.z + depth },
        { worldPos.x - width, worldPos.y + height, worldPos.z + depth }
    };

    Vector2 screenCorners[8];
    bool allVisible = true;

    // Convert all corners to screen space
    for (int i = 0; i < 8; i++) {
        if (!WorldToScreen(corners[i], screenCorners[i], m_viewMatrix, m_screenWidth, m_screenHeight)) {
            allVisible = false;
            break;
        }
    }

    if (!allVisible) return;

    // Draw bottom rectangle
    drawList->AddLine(ImVec2(screenCorners[0].x, screenCorners[0].y), 
        ImVec2(screenCorners[1].x, screenCorners[1].y), m_config.boxColor, m_config.lineThickness);
    drawList->AddLine(ImVec2(screenCorners[1].x, screenCorners[1].y), 
        ImVec2(screenCorners[2].x, screenCorners[2].y), m_config.boxColor, m_config.lineThickness);
    drawList->AddLine(ImVec2(screenCorners[2].x, screenCorners[2].y), 
        ImVec2(screenCorners[3].x, screenCorners[3].y), m_config.boxColor, m_config.lineThickness);
    drawList->AddLine(ImVec2(screenCorners[3].x, screenCorners[3].y), 
        ImVec2(screenCorners[0].x, screenCorners[0].y), m_config.boxColor, m_config.lineThickness);

    // Draw top rectangle
    drawList->AddLine(ImVec2(screenCorners[4].x, screenCorners[4].y), 
        ImVec2(screenCorners[5].x, screenCorners[5].y), m_config.boxColor, m_config.lineThickness);
    drawList->AddLine(ImVec2(screenCorners[5].x, screenCorners[5].y), 
        ImVec2(screenCorners[6].x, screenCorners[6].y), m_config.boxColor, m_config.lineThickness);
    drawList->AddLine(ImVec2(screenCorners[6].x, screenCorners[6].y), 
        ImVec2(screenCorners[7].x, screenCorners[7].y), m_config.boxColor, m_config.lineThickness);
    drawList->AddLine(ImVec2(screenCorners[7].x, screenCorners[7].y), 
        ImVec2(screenCorners[4].x, screenCorners[4].y), m_config.boxColor, m_config.lineThickness);

    // Draw vertical lines connecting top and bottom
    for (int i = 0; i < 4; i++) {
        drawList->AddLine(ImVec2(screenCorners[i].x, screenCorners[i].y),
            ImVec2(screenCorners[i + 4].x, screenCorners[i + 4].y), m_config.boxColor, m_config.lineThickness);
    }
}

void ESPRenderer::DrawSkeleton(ImDrawList* drawList, const Vector3& worldPos) {
    if (!m_viewMatrix || !IsValidViewMatrix(m_viewMatrix)) return;

    // Simplified skeleton (adjust bone positions based on your game's skeleton structure)
    // This is a placeholder - you'll need actual bone positions from the game
    Vector3 bones[] = {
        { worldPos.x, worldPos.y + 1.7f, worldPos.z },        // Head
        { worldPos.x, worldPos.y + 1.2f, worldPos.z },        // Neck
        { worldPos.x, worldPos.y + 0.8f, worldPos.z },        // Chest
        { worldPos.x - 0.3f, worldPos.y + 1.0f, worldPos.z }, // Left shoulder
        { worldPos.x + 0.3f, worldPos.y + 1.0f, worldPos.z }, // Right shoulder
        { worldPos.x - 0.3f, worldPos.y + 0.4f, worldPos.z }, // Left hand
        { worldPos.x + 0.3f, worldPos.y + 0.4f, worldPos.z }, // Right hand
        { worldPos.x, worldPos.y, worldPos.z },               // Pelvis
        { worldPos.x - 0.2f, worldPos.y - 0.8f, worldPos.z }, // Left foot
        { worldPos.x + 0.2f, worldPos.y - 0.8f, worldPos.z }  // Right foot
    };

    // Define skeleton connections (bone pairs)
    int connections[][2] = {
        {0, 1}, {1, 2}, {2, 7},      // Spine
        {1, 3}, {3, 5},              // Left arm
        {1, 4}, {4, 6},              // Right arm
        {7, 8}, {7, 9}               // Legs
    };

    // Convert bones to screen space and draw connections
    Vector2 screenBones[10];
    bool boneVisible[10];

    for (int i = 0; i < 10; i++) {
        boneVisible[i] = WorldToScreen(bones[i], screenBones[i], m_viewMatrix, m_screenWidth, m_screenHeight);
    }

    for (const auto& connection : connections) {
        int bone1 = connection[0];
        int bone2 = connection[1];

        if (boneVisible[bone1] && boneVisible[bone2]) {
            drawList->AddLine(
                ImVec2(screenBones[bone1].x, screenBones[bone1].y),
                ImVec2(screenBones[bone2].x, screenBones[bone2].y),
                m_config.skeletonColor, m_config.lineThickness);
        }
    }
}

void ESPRenderer::DrawHealthBar(ImDrawList* drawList, const Vector2& screenPos, float healthPercent) {
    const float barWidth = 50.0f;
    const float barHeight = 5.0f;
    const float barX = screenPos.x - barWidth * 0.5f;
    const float barY = screenPos.y - m_config.boxHeight * 0.5f - 30.0f;

    // Background
    drawList->AddRectFilled(
        ImVec2(barX, barY),
        ImVec2(barX + barWidth, barY + barHeight),
        IM_COL32(50, 50, 50, 200));

    // Health fill
    float fillWidth = barWidth * (healthPercent / 100.0f);
    ImU32 healthColor = IM_COL32(
        static_cast<int>(255 * (1.0f - healthPercent / 100.0f)),
        static_cast<int>(255 * (healthPercent / 100.0f)),
        0, 255);

    drawList->AddRectFilled(
        ImVec2(barX, barY),
        ImVec2(barX + fillWidth, barY + barHeight),
        healthColor);

    // Border
    drawList->AddRect(
        ImVec2(barX, barY),
        ImVec2(barX + barWidth, barY + barHeight),
        IM_COL32(255, 255, 255, 255), 0.0f, 0, 1.0f);
}

void ESPRenderer::DrawDistance(ImDrawList* drawList, const Vector2& screenPos, float distance) {
    char distanceText[32];
    snprintf(distanceText, sizeof(distanceText), "%.1fm", distance);
    
    ImVec2 textSize = ImGui::CalcTextSize(distanceText);
    ImVec2 textPos(screenPos.x - textSize.x * 0.5f, screenPos.y + 5.0f);
    
    drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), 
        IM_COL32(0, 0, 0, 255), distanceText);
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), distanceText);
}

void ESPRenderer::RenderESP() {
    // Don't render if no features are enabled
    if (m_config.enabledFeatures == 0) {
        return;
    }

    if (!m_viewMatrix || m_screenWidth == 0 || m_screenHeight == 0) {
        return;
    }

    // Validate view matrix before using it
    if (!IsValidViewMatrix(m_viewMatrix)) {
        return;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    for (int i = 0; i < 255; i++) {
        Player* ent = GetEntityByIndex(i);
        if (!ent) continue;

        float x, y, z;
        if (!TryReadEntityCoords(ent, x, y, z)) continue;

        Vector3 worldPos(x, y, z);
        Vector2 screenPos;

        if (!WorldToScreen(worldPos, screenPos, m_viewMatrix, m_screenWidth, m_screenHeight)) {
            continue;
        }

        // Calculate distance (you'll need local player position)
        // For now using placeholder
        float distance = 0.0f; // Replace with actual distance calculation

        // Draw 3D box (drawn first so it's behind other elements)
        if (m_config.IsFeatureEnabled(ESPFeature::Box3D)) {
            Draw3DBox(drawList, worldPos);
        }

        // Draw skeleton
        if (m_config.IsFeatureEnabled(ESPFeature::Skeleton)) {
            DrawSkeleton(drawList, worldPos);
        }

        // Draw 2D box
        if (m_config.IsFeatureEnabled(ESPFeature::Box2D)) {
            Draw2DBox(drawList, screenPos);
        }

        // Draw snapline
        if (m_config.IsFeatureEnabled(ESPFeature::Snapline)) {
            DrawSnapline(drawList, screenPos);
        }

        // Draw health bar
        if (m_config.IsFeatureEnabled(ESPFeature::HealthBar)) {
            DrawHealthBar(drawList, screenPos, 75.0f); // Replace with actual health
        }

        // Draw name
        if (m_config.IsFeatureEnabled(ESPFeature::Name)) {
            DrawName(drawList, screenPos, "Player");
        }

        // Draw distance
        if (m_config.IsFeatureEnabled(ESPFeature::Distance)) {
            DrawDistance(drawList, screenPos, distance);
        }
    }
}