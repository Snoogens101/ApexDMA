#pragma once
#include <cmath>
#include <stdexcept>

struct Vector2D {
    float x, y;

    Vector2D() : x(0), y(0) {}

    Vector2D(float x_val, float y_val) : x(x_val), y(y_val) {}

    float operator[](int i) const;
	float& operator[](int i);

    Vector2D Subtract(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    Vector2D Add(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D Divide(const Vector2D& other) const {
        return Vector2D(x / other.x, y / other.y);
    }

    Vector2D Divide(float scalar) const {
        return Vector2D(x / scalar, y / scalar);
    }

    float DotProduct(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    float Magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    float Distance(const Vector2D& other) const {
        Vector2D diff = Subtract(other);
        return diff.Magnitude();
    }

    Vector2D Multiply(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D Normalized() const {
        Vector2D result;
        float length = std::sqrt(x * x + y * y);
        if (length != 0) {
            result.x = x / length;
            result.y = y / length;
        }
        return result;
    }

    Vector2D MultipliedByScalar(float scalar) const {
        Vector2D result;
        result.x = x * scalar;
        result.y = y * scalar;
        return result;
    }

    Vector2D Clamp() const {
        //pitch doesnt have a full rotation so just set it to max value if its more than that
        float clampedX = x;
        if (clampedX < -89) clampedX = -89;
        if (clampedX > 89) clampedX = 89;
        //yaw has a full rotation so we might want to move it to the oposite side from negative to positive or vice versa
        float clampedY = y;
        if (clampedY < -180) clampedY += 360;
        if (clampedY > 180) clampedY -= 360;
        //create the vector
        if (clampedX > 89 || clampedX < -89) throw std::invalid_argument("SHIT CLAMPING OF PITCH. CHECK YOUR CODE");
        if (clampedY > 180 || clampedY < -180) throw std::invalid_argument("SHIT CLAMPING OF YAW. CHECK YOUR CODE");
        return Vector2D(clampedX, clampedY);
    }

    bool IsZeroVector() {
        return x == 0 && y == 0;
    }

    bool operator==(const Vector2D& other) const {
        float epsilon = 1e-5;
        return (std::abs(x - other.x) < epsilon)
            && (std::abs(y - other.y) < epsilon);
    }

    bool operator!=(const Vector2D& other) const {
        return !(*this == other);
    }
};
