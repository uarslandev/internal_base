#include "math.h"

bool WorldToScreen(const Vector3& worldPos, Vector2& screenPos, const float* viewMatrix, int screenWidth, int screenHeight) {
    // ViewProjection matrix is 4x4, stored in row-major order
    // Transform world coordinates to clip space
    float clipX = viewMatrix[0] * worldPos.x + viewMatrix[1] * worldPos.y + viewMatrix[2] * worldPos.z + viewMatrix[3];
    float clipY = viewMatrix[4] * worldPos.x + viewMatrix[5] * worldPos.y + viewMatrix[6] * worldPos.z + viewMatrix[7];
    float clipZ = viewMatrix[8] * worldPos.x + viewMatrix[9] * worldPos.y + viewMatrix[10] * worldPos.z + viewMatrix[11];
    float clipW = viewMatrix[12] * worldPos.x + viewMatrix[13] * worldPos.y + viewMatrix[14] * worldPos.z + viewMatrix[15];

    // Check if point is behind camera (w <= 0 means behind or at camera)
    if (clipW < 0.1f) {
        return false;
    }

    // Perspective divide - convert clip space to NDC (Normalized Device Coordinates)
    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;
    float ndcZ = clipZ / clipW;

    // Check if point is outside view frustum (-1 to 1 range in NDC)
    if (ndcX < -1.0f || ndcX > 1.0f || ndcY < -1.0f || ndcY > 1.0f || ndcZ < 0.0f || ndcZ > 1.0f) {
        return false;
    }

    // Convert NDC to screen space
    // NDC: -1 to 1 range -> Screen: 0 to width/height range
    // Note: Y is inverted (NDC +1 is top, screen 0 is top)
    screenPos.x = (ndcX + 1.0f) * 0.5f * screenWidth;
    screenPos.y = (1.0f - ndcY) * 0.5f * screenHeight;

    return true;
}