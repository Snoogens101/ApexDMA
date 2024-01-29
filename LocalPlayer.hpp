#pragma once
#include "Offsets.hpp"
#include "DMALibrary/Memory/Memory.h"
#include "Vector2D.hpp"
#include "Vector3D.hpp"

struct LocalPlayer {
    long BasePointer;

    bool IsDead;
    bool IsInAttack;
    bool IsKnocked;
    bool IsZooming;

    int Team;
    Vector3D LocalOrigin;
    Vector3D CameraPosition;

    Vector2D ViewAngles;
    Vector2D PunchAngles;

    int WeaponIndex;
    float WeaponProjectileSpeed;
    float WeaponProjectileScale;
    bool IsHoldingGrenade;

    float ZoomFOV;
    float TargetZoomFOV;

    float ViewYaw;

    void ResetPointer() {
        BasePointer = 0;
    }

    void Read() {
        BasePointer = mem.Read<long>(OFF_REGION + OFF_LOCAL_PLAYER);
        if (BasePointer == 0) return;

        IsDead = mem.Read<short>(BasePointer + OFF_LIFE_STATE) > 0;
        IsKnocked = mem.Read<short>(BasePointer + OFF_BLEEDOUT_STATE) > 0;
        IsZooming = mem.Read<short>(BasePointer + OFF_ZOOMING) > 0;
        IsInAttack = mem.Read<short>(OFF_REGION + OFF_INATTACK) > 0;

        Team = mem.Read<int>(BasePointer + OFF_TEAM_NUMBER);
        LocalOrigin = mem.Read<Vector3D>(BasePointer + OFF_LOCAL_ORIGIN);
        CameraPosition = mem.Read<Vector3D>(BasePointer + OFF_CAMERAORIGIN);
        ViewAngles = mem.Read<Vector2D>(BasePointer + OFF_VIEW_ANGLES);
        PunchAngles = mem.Read<Vector2D>(BasePointer + OFF_PUNCH_ANGLES);

        ViewYaw = mem.Read<float>(BasePointer + OFF_YAW);

        if (!IsDead && !IsKnocked) {
            long WeaponHandle = mem.Read<long>(BasePointer + OFF_WEAPON_HANDLE);
            long WeaponHandleMasked = WeaponHandle & 0xffff;
            long WeaponEntity = mem.Read<long>(OFF_REGION + OFF_ENTITY_LIST + (WeaponHandleMasked << 5));

            int OffHandWeaponID = mem.Read<int>(BasePointer + OFF_OFFHAND_WEAPON);
            IsHoldingGrenade = OffHandWeaponID == -251 ? true : false;

            ZoomFOV = mem.Read<float>(WeaponEntity + OFF_CURRENTZOOMFOV);
            TargetZoomFOV = mem.Read<float>(WeaponEntity + OFF_TARGETZOOMFOV);

            WeaponIndex = mem.Read<int>(WeaponEntity + OFF_WEAPON_INDEX);
            WeaponProjectileSpeed = mem.Read<float>(WeaponEntity + OFF_PROJECTILESPEED);
            WeaponProjectileScale = mem.Read<float>(WeaponEntity + OFF_PROJECTILESCALE);
        }
    }

    bool IsValid() {
        return BasePointer != 0;
    }

    bool IsCombatReady() {
        if (BasePointer == 0) return false;
        if (IsDead) return false;
        if (IsKnocked) return false;
        return true;
    }
};