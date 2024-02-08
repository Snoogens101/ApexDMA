#pragma once
#include "Vector2D.hpp"

struct Vector3D {
    float x, y, z;

    Vector3D() : x(0), y(0), z(0) {}

    Vector3D(float x_val, float y_val, float z_val) : x(x_val), y(y_val), z(z_val) {}

    Vector3D Subtract(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }

    Vector3D Add(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    float DotProduct(const Vector3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    float Magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float Magnitude2D() const {
        return std::sqrt(x * x + y * y);
    }

    float Distance(const Vector3D& other) const {
        Vector3D diff = Subtract(other);
        return diff.Magnitude();
    }

    bool IsZeroVector() {
        return x == 0 && y == 0 && z == 0;
    }

    bool IsValid() {
        if(std::isnan(x) || std::isinf(x) || std::isnan(y) || std::isinf(y) || std::isnan(z) || std::isinf(z)) {
            return false;
        }
        return true;
    }

    Vector3D& Normalize() {
        float len = Magnitude();
        if (len > 0) {
            x /= len;
            y /= len;
            z /= len;
        }
        return *this;
    }

    Vector3D Multiply(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector2D To2D() const {
        return Vector2D(x, y);
    }

    float Distance2D(const Vector3D& other) const {
        return (other.Subtract(*this)).Magnitude2D();
    };

    Vector3D& operator+=(const Vector3D& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3D& operator-=(const Vector3D& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

    bool operator==(const Vector3D& other) const {
        float epsilon = 1e-5;
        return (std::abs(x - other.x) < epsilon)
            && (std::abs(y - other.y) < epsilon)
            && (std::abs(z - other.z) < epsilon);
    }

    bool operator!=(const Vector3D& other) const {
        return !(*this == other);
    }

    Vector3D ModifyZ(float zOffset) {
        return Vector3D{ x, y, z + zOffset };
    }
};