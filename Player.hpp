#pragma once
#include <string>
#include "Offsets.hpp"
#include "LocalPlayer.hpp"
#include "DMALibrary/Memory/Memory.h"
#include "HitboxType.hpp"
#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "Matrix.hpp"

struct Player {
    LocalPlayer* Myself;

    int Index;
    uint64_t BasePointer = 0;

    uint64_t ModelPointer = 0;

    uint64_t BonePointer = 0;

    char NameBuffer[8] = { 0 };
    std::string Name;
    int Team;

    int GlowEnable;
    int GlowThroughWall;
    int HighlightID;

    bool IsDead;
    bool IsKnocked;

    Vector3D LocalOrigin;
    Vector3D AbsoluteVelocity;

    int Health;

    int LastTimeAimedAt;
    int LastTimeAimedAtPrevious;
    bool IsAimedAt;

    float LastVisibleCheckTime;
    int LastVisibleTime;
    int LastTimeVisiblePrevious;
    bool IsVisible;

    bool IsLocal;
    bool IsAlly;
    bool IsHostile;

    float DistanceToLocalPlayer;
    float Distance2DToLocalPlayer;

    bool IsLockedOn;

    Player(int PlayerIndex, LocalPlayer* Me) {
        this->Index = PlayerIndex;
        this->Myself = Me;
    }

    uint64_t GetMilliseconds()
    {
        LARGE_INTEGER PerformanceFrequency;
        QueryPerformanceFrequency(&PerformanceFrequency);
        LARGE_INTEGER CurrentPerformanceCount;
        QueryPerformanceCounter(&CurrentPerformanceCount);
        return (CurrentPerformanceCount.QuadPart/*microseconds*/ * 1000/*milliseconds*/) / PerformanceFrequency.QuadPart/*microseconds*/;
    }

    bool VisCheck() {
        static float LastVisibleCheckTime = 0.0f; // Ensure this is initialized appropriately
        static bool lastVisibleState = false; // Store the last visibility state
        float VisibleCheckTime = GetMilliseconds();

        if (VisibleCheckTime >= LastVisibleCheckTime + 5.0f) {
            // Only update visibility state if enough time has passed
            lastVisibleState = false; // Default to false at the start of each check

            if (LastVisibleTime > LastTimeVisiblePrevious) {
                lastVisibleState = true;
            }
            else if (LastVisibleTime < 0.0f && LastTimeVisiblePrevious > 0.0f) {
                lastVisibleState = true;
            }
            LastTimeVisiblePrevious = LastVisibleTime; // Update after checking
            LastVisibleCheckTime = VisibleCheckTime; // Update the check time
        }

        // Return the last known visibility state if not enough time has passed for a new check
        return lastVisibleState;
    }

    void Read() {
        if (BasePointer == 0) return;
        if (!IsPlayer() && !IsDummy()) { BasePointer = 0; return; }

        IsAimedAt = LastTimeAimedAtPrevious < LastTimeAimedAt;
        LastTimeAimedAtPrevious = LastTimeAimedAt;

        IsVisible = IsDummy() || IsAimedAt || VisCheck();

        if (Myself->IsValid()) {
            IsLocal = Myself->BasePointer == BasePointer;
            IsAlly = Myself->Team == Team;
            IsHostile = !IsAlly;
            DistanceToLocalPlayer = Myself->LocalOrigin.Distance(LocalOrigin);
            Distance2DToLocalPlayer = Myself->LocalOrigin.To2D().Distance(LocalOrigin.To2D());
        }
    }

    bool IsValid() {
        return BasePointer != 0 && Health > 0 && (IsPlayer() || IsDummy());
    }

    bool IsCombatReady() {
        if (!IsValid())return false;
        if (IsDummy()) return true;
        if (IsDead) return false;
        if (IsKnocked) return false;
        return true;
    }

    bool IsPlayer() {
        return Name == "player";
    }

    bool IsDummy() {
        return Team == 97;
    }

    // Bones //
    int GetBoneFromHitbox(HitboxType HitBox) const {
        if (!mem.IsValidPointer(ModelPointer))
            return -1;

        uint64_t StudioHDR = mem.Read<uint64_t>(ModelPointer + 0x8, true);
        if (!mem.IsValidPointer(StudioHDR + 0x34))
            return -1;

        auto HitboxCache = mem.Read<uint16_t>(StudioHDR + 0x34, true);
        auto HitboxArray = StudioHDR + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));
        if (!mem.IsValidPointer(HitboxArray + 0x4))
            return -1;

        auto IndexCache = mem.Read<uint16_t>(HitboxArray + 0x4, true);
        auto HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));
        auto BonePtr = HitboxIndex + HitboxArray + (static_cast<int>(HitBox) * 0x20);
        if (!mem.IsValidPointer(BonePtr))
            return -1;

        return mem.Read<uint16_t>(BonePtr, true);
    }

    Vector3D GetBonePosition(HitboxType HitBox) const {
        Vector3D Offset = Vector3D(0.0f, 0.0f, 0.0f);

        int Bone = GetBoneFromHitbox(HitBox);
        if (Bone < 0 || Bone > 255)
            return LocalOrigin.Add(Offset);

        uint64_t TempBonePointer = BonePointer;  // Create a temporary non-const variable
        TempBonePointer += (Bone * sizeof(Matrix3x4));
        if (!mem.IsValidPointer(TempBonePointer))
            return LocalOrigin.Add(Offset);

        Matrix3x4 BoneMatrix = mem.Read<Matrix3x4>(TempBonePointer);
        Vector3D BonePosition = BoneMatrix.GetPosition();

        if (!BonePosition.IsValid())
            return LocalOrigin.Add(Vector3D(0, 0, 0));

        BonePosition += LocalOrigin;
        return BonePosition;
    }
};