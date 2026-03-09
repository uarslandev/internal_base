#include "math.h"
#include <cmath>

// ========== Vector2 Implementation ==========

float Vector2::Length() const {
    return std::sqrtf(x * x + y * y);
}

float Vector2::LengthSquared() const {
    return x * x + y * y;
}

float Vector2::DistanceTo(const Vector2& other) const {
    float dx = other.x - x;
    float dy = other.y - y;
    return std::sqrtf(dx * dx + dy * dy);
}

Vector2 Vector2::Normalized() const {
    float len = Length();
    if (len > 0.0f) {
        return Vector2(x / len, y / len);
    }
    return Vector2(0.0f, 0.0f);
}

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const {
    return Vector2(x / scalar, y / scalar);
}

Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2& Vector2::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

bool Vector2::operator==(const Vector2& other) const {
    return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2& other) const {
    return !(*this == other);
}

float Vector2::Dot(const Vector2& a, const Vector2& b) {
    return a.x * b.x + a.y * b.y;
}

// ========== Vector3 Implementation ==========

float Vector3::Length() const {
    return std::sqrtf(x * x + y * y + z * z);
}

float Vector3::LengthSquared() const {
    return x * x + y * y + z * z;
}

float Vector3::DistanceTo(const Vector3& other) const {
    float dx = other.x - x;
    float dy = other.y - y;
    float dz = other.z - z;
    return std::sqrtf(dx * dx + dy * dy + dz * dz);
}

Vector3 Vector3::Normalized() const {
    float len = Length();
    if (len > 0.0f) {
        return Vector3(x / len, y / len, z / len);
    }
    return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3& Vector3::operator+=(const Vector3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

bool Vector3::operator==(const Vector3& other) const {
    return x == other.x && y == other.y && z == other.z;
}

bool Vector3::operator!=(const Vector3& other) const {
    return !(*this == other);
}

float Vector3::Dot(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vector3::Cross(const Vector3& a, const Vector3& b) {
    return Vector3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

// ========== WorldToScreen Implementation ==========

namespace MathUtils {
    bool WorldToScreen(const Vector3& worldPos, Vector2& screenPos,
                      const float* viewMatrix, int screenWidth, int screenHeight) {
        if (!viewMatrix) return false;

        // Transform world coordinates to clip space
        float clipX = viewMatrix[0] * worldPos.x + viewMatrix[1] * worldPos.y +
                      viewMatrix[2] * worldPos.z + viewMatrix[3];
        float clipY = viewMatrix[4] * worldPos.x + viewMatrix[5] * worldPos.y +
                      viewMatrix[6] * worldPos.z + viewMatrix[7];
        float clipZ = viewMatrix[8] * worldPos.x + viewMatrix[9] * worldPos.y +
                      viewMatrix[10] * worldPos.z + viewMatrix[11];
        float clipW = viewMatrix[12] * worldPos.x + viewMatrix[13] * worldPos.y +
                      viewMatrix[14] * worldPos.z + viewMatrix[15];

        // Check if point is behind camera
        if (clipW < 0.1f) {
            return false;
        }

        // Perspective divide - convert to NDC
        float ndcX = clipX / clipW;
        float ndcY = clipY / clipW;
        float ndcZ = clipZ / clipW;

        // Check if point is outside view frustum
        if (ndcX < -1.0f || ndcX > 1.0f || ndcY < -1.0f || ndcY > 1.0f ||
            ndcZ < 0.0f || ndcZ > 1.0f) {
            return false;
        }

        // Convert NDC to screen space
        screenPos.x = (ndcX + 1.0f) * 0.5f * screenWidth;
        screenPos.y = (1.0f - ndcY) * 0.5f * screenHeight;

        return true;
    }
}

// Legacy compatibility function
bool WorldToScreen(const Vector3& worldPos, Vector2& screenPos,
                  const float* viewMatrix, int screenWidth, int screenHeight) {
    return MathUtils::WorldToScreen(worldPos, screenPos, viewMatrix, screenWidth, screenHeight);
}