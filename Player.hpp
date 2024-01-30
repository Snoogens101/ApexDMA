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
    long long BasePointer;

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
    int MaxHealth;
    int Shield;
    int MaxShield;

    int LastTimeAimedAt;
    int LastTimeAimedAtPrevious;
    bool IsAimedAt;

    int LastVisibleTime;
    int LastTimeVisiblePrevious;
    bool IsVisible;

    bool IsLocal;
    bool IsAlly;
    bool IsHostile;

    float DistanceToLocalPlayer;
    float Distance2DToLocalPlayer;

    float ViewYaw;

    bool IsLockedOn;

    Player(int PlayerIndex, LocalPlayer* Me) {
        this->Index = PlayerIndex;
        this->Myself = Me;
    }

    void Read() {
        BasePointer = mem.Read<long long>(OFF_BASE + OFF_ENTITY_LIST + ((Index + 1) << 5));
        if (BasePointer == 0) return;

        char buffer[8] = { 0 };
        bool success = mem.Read(BasePointer + OFF_NAME, &buffer, 8);
        if (!success) {
			Name = "Unknown";
		}
        else {
			Name = std::string(buffer);
		}
        Team = mem.Read<int>(BasePointer + OFF_TEAM_NUMBER);

        if (!IsPlayer() && !IsDummy()) { BasePointer = 0; return; }
        IsDead = (IsDummy()) ? false : mem.Read<short>(BasePointer + OFF_LIFE_STATE) > 0;
        IsKnocked = (IsDummy()) ? false : mem.Read<short>(BasePointer + OFF_BLEEDOUT_STATE) > 0;

        LocalOrigin = mem.Read<Vector3D>(BasePointer + OFF_LOCAL_ORIGIN);
        AbsoluteVelocity = mem.Read<Vector3D>(BasePointer + OFF_ABSVELOCITY);

        GlowEnable = mem.Read<int>(BasePointer + OFF_GLOW_ENABLE);
        GlowThroughWall = mem.Read<int>(BasePointer + OFF_GLOW_THROUGH_WALL);
        HighlightID = mem.Read<int>(BasePointer + OFF_GLOW_HIGHLIGHT_ID + 1);

        LastTimeAimedAt = mem.Read<int>(BasePointer + OFF_LAST_AIMEDAT_TIME);
        IsAimedAt = LastTimeAimedAtPrevious < LastTimeAimedAt;
        LastTimeAimedAtPrevious = LastTimeAimedAt;

        LastVisibleTime = mem.Read<int>(BasePointer + OFF_LAST_VISIBLE_TIME);
        IsVisible = IsDummy() || IsAimedAt || LastTimeVisiblePrevious < LastVisibleTime;
        LastTimeVisiblePrevious = LastVisibleTime;

        Health = mem.Read<int>(BasePointer + OFF_HEALTH);
        MaxHealth = mem.Read<int>(BasePointer + OFF_MAXHEALTH);
        Shield = mem.Read<int>(BasePointer + OFF_SHIELD);
        MaxShield = mem.Read<int>(BasePointer + OFF_MAXSHIELD);

        if (Myself->IsValid()) {
            IsLocal = Myself->BasePointer == BasePointer;
            IsAlly = Myself->Team == Team;
            IsHostile = !IsAlly;
            DistanceToLocalPlayer = Myself->LocalOrigin.Distance(LocalOrigin);
            Distance2DToLocalPlayer = Myself->LocalOrigin.To2D().Distance(LocalOrigin.To2D());
        }
    }

    std::string GetPlayerName() {
        uintptr_t NameIndex = mem.Read<uintptr_t>(BasePointer + OFF_NAME_INDEX);
        uintptr_t NameOffset = mem.Read<uintptr_t>(OFF_BASE + OFF_NAME_LIST + ((NameIndex - 1) << 4));

        char buffer[64] = { 0 };
        bool success = mem.Read(NameOffset, &buffer, 64);

        if (!success) {
            return "Unknown";
        }

        return std::string(buffer);
    }


    float GetViewYaw() {
        if (!IsDummy() || IsPlayer()) {
            return mem.Read<float>(BasePointer + OFF_YAW);
        }
        return 0.0f;
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
        long ModelPointer = mem.Read<long long>(BasePointer + OFF_STUDIOHDR);
        if (!mem.IsValidPointer(ModelPointer))
            return -1;

        long StudioHDR = mem.Read<long long>(ModelPointer + 0x8);
        if (!mem.IsValidPointer(StudioHDR + 0x34))
            return -1;

        auto HitboxCache = mem.Read<uint16_t>(StudioHDR + 0x34);
        auto HitboxArray = StudioHDR + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));
        if (!mem.IsValidPointer(HitboxArray + 0x4))
            return -1;

        auto IndexCache = mem.Read<uint16_t>(HitboxArray + 0x4);
        auto HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));
        auto BonePointer = HitboxIndex + HitboxArray + (static_cast<int>(HitBox) * 0x20);
        if (!mem.IsValidPointer(BonePointer))
            return -1;

        return mem.Read<uint16_t>(BonePointer);
    }

    Vector3D GetBonePosition(HitboxType HitBox) const {
        Vector3D Offset = Vector3D(0.0f, 0.0f, 0.0f);

        int Bone = GetBoneFromHitbox(HitBox);
        if (Bone < 0 || Bone > 255)
            return LocalOrigin.Add(Offset);

        long BonePtr = mem.Read<long long>(BasePointer + OFF_BONES);
        BonePtr += (Bone * sizeof(Matrix3x4));
        if (!mem.IsValidPointer(BonePtr))
            return LocalOrigin.Add(Offset);

        Matrix3x4 BoneMatrix = mem.Read<Matrix3x4>(BonePtr);
        Vector3D BonePosition = BoneMatrix.GetPosition();

        if (!BonePosition.IsValid())
            return LocalOrigin.Add(Vector3D(0, 0, 0));

        BonePosition += LocalOrigin;
        return BonePosition;
    }
};