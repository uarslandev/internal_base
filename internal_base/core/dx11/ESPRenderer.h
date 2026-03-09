#pragma once
#include "imgui/imgui.h"
#include "math/math.h"
#include "reclass/reclass.h"

class ESPRenderer {
public:
    ESPRenderer();

    void SetViewMatrix(float* matrix) { m_viewMatrix = matrix; }
    void SetScreenDimensions(int width, int height);
    void RenderESP();

private:
    bool TryReadEntityCoords(Player* ent, float& x, float& y, float& z);

    float* m_viewMatrix;
    int m_screenWidth;
    int m_screenHeight;
};