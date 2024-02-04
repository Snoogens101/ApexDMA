#pragma once
#include "Offsets.hpp"
#include "LocalPlayer.hpp"
#include "Player.hpp"
#include "Offsets.hpp"
#include "DMALibrary/Memory/Memory.h"
#include "Vector3D.hpp"
#include "Vector2D.hpp"
#include "Matrix.hpp"

struct Camera {
    Vector2D ScreenSize = Vector2D(0, 0);
    Vector2D ScreenCenter = Vector2D(ScreenSize.x / 2, ScreenSize.y / 2);
    float FOV;
    ViewMatrix GameViewMatrix;
    uint64_t RenderPointer;

    void Initialize() {
        ScreenCenter = Vector2D(ScreenSize.x / 2, ScreenSize.y / 2);
    }

    const Vector2D& GetResolution() {
        return ScreenSize;
    }

    const Vector2D& GetCenter() {
		return ScreenCenter;
	}

    void Update() {
        uint64_t MatrixPtr = mem.Read<uint64_t>(RenderPointer + OFF_VIEWMATRIX, true);
        GameViewMatrix = mem.Read<ViewMatrix>(MatrixPtr);
    }

    bool WorldToScreen(Vector3D WorldPosition, Vector2D& ScreenPosition) const {
        Vector3D transformed = GameViewMatrix.Transform(WorldPosition);
        if (transformed.z < 0.001) {
            return false;
        }

        transformed.x *= 1.0 / transformed.z;
        transformed.y *= 1.0 / transformed.z;

        ScreenPosition = Vector2D(ScreenSize.x / 2.0f + transformed.x * (ScreenSize.x / 2.0f), ScreenSize.y / 2.0f - transformed.y * (ScreenSize.y / 2.0f));
        return true;
    }
};