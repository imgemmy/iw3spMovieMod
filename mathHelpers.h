#pragma once
#include <math.h>
#include <float.h>
#include <cmath>
#include <d3dx9.h>

# define M_PI           3.14159265358979323846f

inline float normalizeAngle(float x, float constraintMin, float constraintMax)
{
	return fmod(x, constraintMin) + (x < 0 ? constraintMax : 0);
}

struct Vector3
{
    float x, y, z;
    inline float& operator [] (int index) { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; return z; }
    const float& operator [] (int index) const { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; return FLT_MIN; }
    inline Vector3& operator + (Vector3 addition)
    {
        Vector3 result;
        result.x = this->x + addition.x;
        result.y = this->y + addition.y;
        result.z = this->z + addition.z;
        return result;
    }
    inline Vector3 operator*(float scalar) const 
    {
        return { x * scalar, y * scalar, z * scalar };
    }

    // Static method to convert D3DXVECTOR3 to Vector3
    static Vector3 fromD3DXVECTOR3(const D3DXVECTOR3& d3dVec) {
        return Vector3(d3dVec.x, d3dVec.y, d3dVec.z);
    }

    // Operator to convert Vector3 to D3DXVECTOR3
    operator D3DXVECTOR3() const {
        return D3DXVECTOR3(x, y, z);
    }

};

struct Vector2
{
    inline Vector2()
    {
        this->x = 0;
        this->y = 0;
    }

    inline Vector2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    float x, y;
    inline float& operator [] (int index) { if (index == 0) return x;  if (index == 1) return y; return y; }
    const float& operator [] (int index) const { if (index == 0) return x;  if (index == 1) return y; return FLT_MIN; }
};

inline float toRadians(float degrees)
{
    return degrees * (M_PI / 180.f);
}

inline float dot(Vector3 a, Vector3 b)  //calculates dot product of a and b
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float mag(Vector3 a)  //calculates magnitude of a
{
    return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

inline float angleBetweenVectors(Vector3 a, Vector3 b)
{
    return std::acos(dot(a, b) / (mag(a) * mag(b)));
}