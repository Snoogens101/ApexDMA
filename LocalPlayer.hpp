#pragma once
#include <iostream>
#include "Offsets.hpp"
#include "DMALibrary/Memory/Memory.h"
#include "Vector2D.hpp"
#include "Vector3D.hpp"

struct LocalPlayer {
    uint64_t BasePointer;

    bool IsDead;
    bool IsInAttack;
    bool IsKnocked;
    bool IsZooming;

    int Team;
    Vector3D LocalOrigin;
    Vector3D CameraPosition;

    Vector2D ViewAngles;
    Vector2D PunchAngles;
    float ViewYaw;

    int WeaponIndex;
    float WeaponProjectileSpeed;
    float WeaponProjectileScale;
    bool IsHoldingGrenade;
    bool IsReloading;
    int Ammo;

    float ZoomFOV;
    float TargetZoomFOV;

    float TimeBase;
    float TraversalTimeStart;
    float TraversalTimeProgress;

    void ResetPointer() {
        BasePointer = 0;
    }

    bool ValidPosition() {
		if (LocalOrigin.x != 0.0f && LocalOrigin.y != 0.0f && LocalOrigin.z != 0.0f) {
            return true;
		} else {
			return false;
		}
	}

    void Read() {
        if (!mem.IsValidPointer(BasePointer)) return;

        auto handle = mem.CreateScatterHandle();

        // Scatter read request for IsDead
        uint64_t isDeadAddress = BasePointer + OFF_LIFE_STATE;
        mem.AddScatterReadRequest(handle, isDeadAddress, &IsDead, sizeof(bool));

        // Scatter read request for IsKnocked
        uint64_t isKnockedAddress = BasePointer + OFF_BLEEDOUT_STATE;
        mem.AddScatterReadRequest(handle, isKnockedAddress, &IsKnocked, sizeof(bool));

        // Scatter read request for IsZooming
        uint64_t isZoomingAddress = BasePointer + OFF_ZOOMING;
        mem.AddScatterReadRequest(handle, isZoomingAddress, &IsZooming, sizeof(bool));

        // Scatter read request for Team
        uint64_t teamAddress = BasePointer + OFF_TEAM_NUMBER;
		mem.AddScatterReadRequest(handle, teamAddress, &Team, sizeof(int));

        // Scatter read request for LocalOrigin
        uint64_t localOriginAddress = BasePointer + OFF_LOCAL_ORIGIN;
		mem.AddScatterReadRequest(handle, localOriginAddress, &LocalOrigin, sizeof(Vector3D));

        // Scatter read request for CameraPosition
        uint64_t cameraPositionAddress = BasePointer + OFF_CAMERAORIGIN;
		mem.AddScatterReadRequest(handle, cameraPositionAddress, &CameraPosition, sizeof(Vector3D));

        // Scatter read request for ViewAngles
        uint64_t viewAnglesAddress = BasePointer + OFF_VIEW_ANGLES;
		mem.AddScatterReadRequest(handle, viewAnglesAddress, &ViewAngles, sizeof(Vector2D));

        // Scatter read request for ViewYaw
        uint64_t viewYawAddress = BasePointer + OFF_YAW;
        mem.AddScatterReadRequest(handle, viewYawAddress, &ViewYaw, sizeof(float));

        // Scatter read request for WeaponHandle
        uint64_t WeaponHandle;
        uint64_t weaponHandleAddress = BasePointer + OFF_WEAPON_HANDLE;
		mem.AddScatterReadRequest(handle, weaponHandleAddress, &WeaponHandle, sizeof(uint64_t));

        // Scatter read request for OffhandWeaponID
        int OffHandWeaponID;
        uint64_t offhandWeaponIDAddress = BasePointer + OFF_OFFHAND_WEAPON;
		mem.AddScatterReadRequest(handle, offhandWeaponIDAddress, &OffHandWeaponID, sizeof(int));

        // Execute the scatter read
        mem.ExecuteReadScatter(handle);

        // Close the scatter handle
        mem.CloseScatterHandle(handle);

        if (!IsDead && !IsKnocked && WeaponHandle) {
            uint64_t WeaponHandleMasked = WeaponHandle & 0xffff;
            uint64_t WeaponEntity = mem.Read<uint64_t>(mem.OFF_BASE + OFF_ENTITY_LIST + (WeaponHandleMasked << 5), true);

            IsHoldingGrenade = OffHandWeaponID == -251 ? true : false;

            auto handle = mem.CreateScatterHandle();

            // Scatter read request for ZoomFOV
            uint64_t zoomFOVAddress = WeaponEntity + OFF_CURRENTZOOMFOV;
            mem.AddScatterReadRequest(handle, zoomFOVAddress, &ZoomFOV, sizeof(float));

            // Scatter read request for TargetZoomFOV
            uint64_t targetZoomFOVAddress = WeaponEntity + OFF_TARGETZOOMFOV;
            mem.AddScatterReadRequest(handle, targetZoomFOVAddress, &TargetZoomFOV, sizeof(float));

            // Scatter read request for WeaponIndex
            uint64_t weaponIndexAddress = WeaponEntity + OFF_WEAPON_INDEX;
            mem.AddScatterReadRequest(handle, weaponIndexAddress, &WeaponIndex, sizeof(int));

            // Scatter read request for WeaponProjectileSpeed
            uint64_t weaponProjectileSpeedAddress = WeaponEntity + OFF_PROJECTILESPEED;
            mem.AddScatterReadRequest(handle, weaponProjectileSpeedAddress, &WeaponProjectileSpeed, sizeof(float));

            // Scatter read request for WeaponProjectileScale
            uint64_t weaponProjectileScaleAddress = WeaponEntity + OFF_PROJECTILESCALE;
            mem.AddScatterReadRequest(handle, weaponProjectileScaleAddress, &WeaponProjectileScale, sizeof(float));

            // Scatter read request for Weapon Ammo
            uint64_t weaponAmmoAddress = WeaponEntity + OFF_WEAPON_AMMO;
            mem.AddScatterReadRequest(handle, weaponAmmoAddress, &Ammo, sizeof(int));

            // Scatter read request for IsReloading
            uint64_t isReloadingAddress = WeaponEntity + OFF_RELOADING;
            mem.AddScatterReadRequest(handle, isReloadingAddress, &IsReloading, sizeof(bool));

            // Execute the scatter read
            mem.ExecuteReadScatter(handle);

            // Close the scatter handle
            mem.CloseScatterHandle(handle);
        }
    }

    bool IsValid() {
        return mem.IsValidPointer(BasePointer);
    }

    bool IsCombatReady() {
        if (!mem.IsValidPointer(BasePointer)) return false;
        if (IsDead) return false;
        if (IsKnocked) return false;
        return true;
    }
};