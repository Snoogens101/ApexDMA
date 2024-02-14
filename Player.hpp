#pragma once
#include <string>
#include "Offsets.hpp"
#include "LocalPlayer.hpp"
#include "Level.hpp"
#include "DMALibrary/Memory/Memory.h"
#include "HitboxType.hpp"
#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "Matrix.hpp"

struct Player {
    LocalPlayer* Myself;
    Level* Map;

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

    float ViewYaw;

    int LastTimeAimedAt;
    int LastTimeAimedAtPrevious;
    bool IsAimedAt;

    uint64_t LastVisibleCheckTime = 0;
    int LastVisibleTime;
    int LastTimeVisiblePrevious = 0;
    bool LastVisibleState = false;
    int VisCheckCount = 0;
    const int VisCheckThreshold = 10;
    bool IsVisible;

    bool IsLocal;
    bool IsAlly;
    bool IsHostile;

    float DistanceToLocalPlayer;
    float Distance2DToLocalPlayer;

    bool IsLockedOn;

    uint64_t Valid = 0;

    Player(int PlayerIndex, LocalPlayer* Me, Level* Map) {
        this->Index = PlayerIndex;
        this->Myself = Me;
        this->Map = Map;
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
        uint64_t VisibleCheckTime = GetMilliseconds();

        if (VisibleCheckTime >= LastVisibleCheckTime + 10) {
            LastVisibleState = false;
            if (LastVisibleTime > LastTimeVisiblePrevious) {
                LastVisibleState = true;
                VisCheckCount = 0;
            }
            else if (LastVisibleTime < 0 && LastTimeVisiblePrevious > 0) {
                LastVisibleState = true;
                VisCheckCount = 0;
            }
            else if(LastVisibleTime == LastTimeVisiblePrevious) {
                VisCheckCount++;
                if (VisCheckCount < VisCheckThreshold) {
                    LastVisibleState = true;
                }
            }
            LastTimeVisiblePrevious = LastVisibleTime;
            LastVisibleCheckTime = VisibleCheckTime;
        }

        return LastVisibleState;
    }

    void ValidCheck() {
        if (Valid) {
            if (Valid > 0x7FFFFFFEFFFF || Valid < 0x7FF700000000) {
				BasePointer = 0;
				Valid = 0;
			}
        }
    }

    void Read() {
        if (!mem.IsValidPointer(BasePointer)) { BasePointer = 0; return; }
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
        return mem.IsValidPointer(BasePointer) && !IsDead && Health > 0 && (IsPlayer() || IsDummy() && Map->IsFiringRange);
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