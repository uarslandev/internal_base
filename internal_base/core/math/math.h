#pragma once
#include <cmath>

class Vector2 {
public:
    float x, y;
    
    // Constructors
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    
    // Utility methods
    float Length() const;
    float LengthSquared() const;
    float DistanceTo(const Vector2& other) const;
    Vector2 Normalized() const;
    
    // Operators
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    
    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(float scalar);
    Vector2& operator/=(float scalar);
    
    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;
    
    // Static utility
    static float Dot(const Vector2& a, const Vector2& b);
};

class Vector3 {
public:
    float x, y, z;
    
    // Constructors
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    // Utility methods
    float Length() const;
    float LengthSquared() const;
    float DistanceTo(const Vector3& other) const;
    Vector3 Normalized() const;
    
    // Operators
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    
    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);
    
    bool operator==(const Vector3& other) const;
    bool operator!=(const Vector3& other) const;
    
    // Static utility
    static float Dot(const Vector3& a, const Vector3& b);
    static Vector3 Cross(const Vector3& a, const Vector3& b);
};

// World to screen transformation
namespace MathUtils {
    bool WorldToScreen(const Vector3& worldPos, Vector2& screenPos, 
                      const float* viewMatrix, int screenWidth, int screenHeight);
}