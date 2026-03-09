#include "ESPRenderer.h"
#include "hooks/hooks.h"

ESPRenderer::ESPRenderer()
    : m_viewMatrix(nullptr)
    , m_screenWidth(0)
    , m_screenHeight(0) {
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

void ESPRenderer::RenderESP() {
    if (!m_viewMatrix || m_screenWidth == 0 || m_screenHeight == 0) {
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

        if (WorldToScreen(worldPos, screenPos, m_viewMatrix, m_screenWidth, m_screenHeight)) {
            drawList->AddText(ImVec2(screenPos.x, screenPos.y),
                IM_COL32(255, 255, 255, 255),
                "Player");
        }
    }
}