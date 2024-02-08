#pragma once
#include <cmath>
#include "Vector3D.hpp"
#include "QAngle.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif


class Resolver {
public:
    static QAngle CalculateAngle(Vector3D from, Vector3D to) {
        Vector3D newDirection = to.Subtract(from);
        newDirection.Normalize();
        
        float pitch = -asinf(newDirection.z) * (180 / M_PI);
        float yaw = atan2f(newDirection.y, newDirection.x) * (180 / M_PI);

        return QAngle(pitch, yaw);
    }

    static Vector3D GetTargetPosition(const Vector3D& targetPosition, Vector3D targetVelocity, float time) {
        return targetPosition.Add((targetVelocity.Multiply(time)));
    }

    static float GetTimeToTarget(Vector3D startPosition, Vector3D endPosition, float bulletSpeed) {
        float distance = (endPosition.Subtract(startPosition)).Magnitude();
        return distance / bulletSpeed;
    }

    static float GetBasicBulletDrop(Vector3D startPosition, Vector3D endPosition, float bulletSpeed, float bulletDropRate) {
        float time = GetTimeToTarget(startPosition, endPosition, bulletSpeed);
        bulletDropRate *= 400.0f;
        return 0.5f * bulletDropRate * time * time;
    }

    static Vector3D GetTargetPosition(Vector3D startPosition, Vector3D endPosition, Vector3D targetVelocity, float bulletSpeed) {
        float time = GetTimeToTarget(startPosition, endPosition, bulletSpeed);
        return GetTargetPosition(endPosition, targetVelocity, time);
    }

    static float GetTimeToTarget(const Vector3D start, const Vector3D end, const float speed, const float gravity) {
        float horizontalDistance = start.Distance2D(end);
        float heightDifference = end.z - start.z;
        return (speed * sqrtf(2 * sinf(M_PI / 4) / gravity)) + (sqrtf(2 * sinf(M_PI / 4) * (sinf(M_PI / 4) - 2 * heightDifference) / gravity));
    }
};
