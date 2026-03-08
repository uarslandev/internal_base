#pragma once

class Vector2 {
public:
	float x, y;
	Vector2() : x(0), y(0) {}
	Vector2(float x, float y) : x(x), y(y) {}
};

class Vector3 {
public:
	float x, y, z;
	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

// Convert 3D world coordinates to 2D screen coordinates
// Returns true if the point is visible (in front of camera)
bool WorldToScreen(const Vector3& worldPos, Vector2& screenPos, const float* viewMatrix, int screenWidth, int screenHeight);