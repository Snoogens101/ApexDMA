#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <array>

#include "HidTable.hpp"
#include "KmboxNet.hpp"
#include "KmboxB.h"

#include "Player.hpp"
#include "LocalPlayer.hpp"
#include "Offsets.hpp"
#include "Camera.hpp"

#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "QAngle.hpp"
#include "Resolver.hpp"

#include "DMALibrary/Memory/Memory.h"
#include "Conversion.hpp"
#include "HitboxType.hpp"

// Conversion
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI / 180.f) )

struct Aimbot {
    bool PredictMovement = true;
    bool PredictBulletDrop = true;

    float FinalDistance = 0;

    bool Sticky = false;
    float Smooth = 2.0f;
    float MaxSmoothIncrease = 0.20f;
    float FOV = 10;
    float ZoomScale = 1.2;
    float MinDistance = 0;
    float HipfireDistance = 50;
    float ZoomDistance = 300;
    int MinimumDelay = 1;
    float RecoilCompensation = 1.0f;

    int AimBotKey = 0x02;
    int AimBotKey2 = 0x02;
    int AimTriggerKey = 0x05;
    int AimFlickKey = 0x06;

    LocalPlayer* Myself;
    std::vector<Player*>* Players;
    Camera* GameCamera;

    Player* CurrentTarget = nullptr;
    bool TargetSelected = true;
    std::chrono::milliseconds LastAimTime{ 0 };

    Aimbot(LocalPlayer* Myself, std::vector<Player*>* GamePlayers, Camera* Cam) {
        this->Myself = Myself;
        this->Players = GamePlayers;
        this->GameCamera = Cam;
    }

    std::array<int, 4> Shotguns = { 104, 96, 97, 88 };

    std::string KmboxType = "Net";
    char KmboxIP[24] = "192.168.2.188";
    char KmboxPort[10] = "8336";
    char KmboxUUID[32] = "24B75054";
    int KmboxComPort = 0;

    _com comPort;

    void Initialize() {
        if (KmboxType == "Net") {
            std::cout << "Initializing Kmbox Net..." << std::endl;
            MinimumDelay = 1;

            int result = kmNet_init(KmboxIP, KmboxPort, KmboxUUID);

            if (result != 0) {
                std::cout << "Kmbox Net failed to initialize." << std::endl;
            }
            else {
                std::cout << "Kmbox Net initialized successfully." << std::endl;
            }
		}
		else if (KmboxType == "BPro") {
			std::cout << "Initializing Kmbox B+ Pro..." << std::endl;
            MinimumDelay = 3;

            if (comPort.open(KmboxComPort, 115200)) {
                std::cout << "Kmbox B+ Pro initialized successfully." << std::endl;
			}
            else {
                std::cout << "Kmbox B+ Pro failed to initialize." << std::endl;
			}
		
        }

    }

    void Move(int x, int y) {
        if (KmboxType == "Net") {
            kmNet_mouse_move(x, y);
        }
        else if (KmboxType == "BPro") {
            char cmd[1024] = { 0 };
            sprintf_s(cmd, "km.move(%d, %d, 10)\r\n", x, y);
		Sleep((int)Utils::RandomRange(MinimumDelay, 1));
            comPort.write(cmd);
        }
    }

    void Reload() {
		if (KmboxType == "Net") {
            if (Myself->IsInAttack) {
                kmNet_mask_mouse_left(1);
                kmNet_mouse_left(0);
            }

			kmNet_keydown(KEY_R);
            Sleep((int)Utils::RandomRange(MinimumDelay, 10));
            kmNet_keyup(KEY_R);
            kmNet_mask_mouse_left(0);
		}
		else if (KmboxType == "BPro") {
			char cmd[1024] = { 0 };
			sprintf_s(cmd, "km.press('r')\r\n");
			Sleep((int)Utils::RandomRange(MinimumDelay, 10));
			comPort.write(cmd);
		}
    }

    void LeftClick() {
        if (KmboxType == "Net") {
            kmNet_mouse_left(1);
		    Sleep((int)Utils::RandomRange(MinimumDelay, 10));
		    kmNet_mouse_left(0);
		}
		else if (KmboxType == "BPro") {
            char cmd[1024] = { 0 };
            sprintf_s(cmd, "km.click(0)\r\n");
			Sleep((int)Utils::RandomRange(MinimumDelay, 10));
            comPort.write(cmd);
		}
	}

    void Update_TacticalReload() {
        if (!Myself->IsCombatReady()) { return; }
		if (!Myself->IsReloading && !Myself->IsHoldingGrenade) {
            if (Myself->Ammo == 1) {
                Reload(); 
            }
		}
	}

    void Update_Aimbot() {
        if (Myself->IsZooming)
            FinalDistance = ZoomDistance;
        else FinalDistance = HipfireDistance;
        if (mem.GetKeyboard()->IsKeyDown(AimTriggerKey)) { return; }

        if (!Myself->IsCombatReady()) { CurrentTarget = nullptr; return; }
        if (!mem.GetKeyboard()->IsKeyDown(AimBotKey) && !mem.GetKeyboard()->IsKeyDown(AimFlickKey) && !Myself->IsInAttack) { ReleaseTarget(); return; }
        if (Myself->IsHoldingGrenade) { ReleaseTarget(); return; }

        Player* Target = CurrentTarget;
        if (Sticky) {
            if (!IsValidTarget(Target)) {
                if (TargetSelected) {
                    return;
                }

                Target = FindBestTarget();
                if (!IsValidTarget(Target)) {
                    ReleaseTarget();
                    return;
                }

                CurrentTarget = Target;
                CurrentTarget->IsLockedOn = true;
                TargetSelected = true;
            }
        }
        else {
            Target = FindBestTarget();
            if (!IsValidTarget(Target)) {
                ReleaseTarget();
                return;
            }

            CurrentTarget = Target;
            CurrentTarget->IsLockedOn = true;
            TargetSelected = true;
        }

        if (TargetSelected && CurrentTarget) {
            std::chrono::milliseconds Now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            if (Now >= LastAimTime + std::chrono::milliseconds(10)) {
                StartAiming(CurrentTarget);
                LastAimTime = Now + std::chrono::milliseconds((int)Utils::RandomRange(MinimumDelay, 10));
            }
            return;
        }
    }

    void Update_Aimbot2() {
        if (Myself->IsZooming)
            FinalDistance = ZoomDistance;
        else FinalDistance = HipfireDistance;
        if (mem.GetKeyboard()->IsKeyDown(AimTriggerKey)) { return; }

        if (!Myself->IsCombatReady()) { CurrentTarget = nullptr; return; }
        if (!mem.GetKeyboard()->IsKeyDown(AimBotKey2) && !mem.GetKeyboard()->IsKeyDown(AimFlickKey) && !Myself->IsInAttack) { ReleaseTarget(); return; }
        if (Myself->IsHoldingGrenade) { ReleaseTarget(); return; }

        Player* Target = CurrentTarget;
        if (Sticky) {
            if (!IsValidTarget(Target)) {
                if (TargetSelected) {
                    return;
                }

                Target = FindBestTarget();
                if (!IsValidTarget(Target)) {
                    ReleaseTarget();
                    return;
                }

                CurrentTarget = Target;
                CurrentTarget->IsLockedOn = true;
                TargetSelected = true;
            }
        }
        else {
            Target = FindBestTarget();
            if (!IsValidTarget(Target)) {
                ReleaseTarget();
                return;
            }

            CurrentTarget = Target;
            CurrentTarget->IsLockedOn = true;
            TargetSelected = true;
        }

        if (TargetSelected && CurrentTarget) {
            std::chrono::milliseconds Now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            if (Now >= LastAimTime + std::chrono::milliseconds(10)) {
                StartAiming(CurrentTarget);
                LastAimTime = Now + std::chrono::milliseconds((int)Utils::RandomRange(MinimumDelay, 10));
            }
            return;
        }
    }

    void StartAiming(Player* Target) {
        Vector3D TargetPosition = CalculatePredictedPosition(Target->GetBonePosition(static_cast<HitboxType>(GetBestBone(Target))), Target->AbsoluteVelocity, Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale);
        Vector2D ScreenPosition = { 0, 0 };
        if (GameCamera->WorldToScreen(TargetPosition, ScreenPosition)) {
            Vector2D Center = GameCamera->GetCenter();
            Vector2D RelativePosition = { ScreenPosition.x - Center.x, ScreenPosition.y - Center.y };
            float baseSmoothing = Smooth; // Base smoothing factor
            float baseFOV = FOV; // Base field of view
            float maxPercentageIncrease = MaxSmoothIncrease; // Maximum increase is 20% of the base smoothing
            float maxIncrease = baseSmoothing * maxPercentageIncrease; // Absolute max increase in smoothing

            // Randomize baseSmoothing within the range [-0.05, +0.05]
            baseSmoothing += Utils::RandomRange(-0.05f, 0.05f);

            // Randomize FOV within the range [-1, +1]
            baseFOV = FOV + Utils::RandomRange(-1.0f, 1.0f);

            // Distance from the target, already calculated
            float distance = CalculateDistanceFromCrosshair(TargetPosition);

            // Calculate dynamic smoothing
            float dynamicSmoothing = baseSmoothing;
            if (distance <= baseFOV) {
                // Linear scaling: From base smoothing at fovDistance to base smoothing + maxIncrease at 1 pixel
                float scale = 1.0f - (distance / baseFOV); // Normalize scale between 0 (at fovDistance) and 1 (at 1 pixel)
                dynamicSmoothing += maxIncrease * scale; // Apply scaled increase
            }

            // Ensure dynamicSmoothing does not decrease below baseSmoothing
            dynamicSmoothing = (std::max)(dynamicSmoothing, baseSmoothing);

            // Calculate the movement step for this frame
            Vector2D step = {
                (RelativePosition.x / dynamicSmoothing),
                (RelativePosition.y / ((Myself->IsInAttack||mem.GetKeyboard()->IsKeyDown(VK_LBUTTON)) ? dynamicSmoothing * RecoilCompensation : dynamicSmoothing))
            };

            // If step is too small, don't move
            if ((std::abs(step.x) < 1.0f) && (std::abs(step.y) < 1.0f)) {
                return;
            }

            Move(step.x, step.y);
		}

        if (!mem.GetKeyboard()->IsKeyDown(AimFlickKey)) {
			return;
		}

        std::vector<int> bones = { 0, 3 };
        for (int boneIndex : bones) {
            Vector3D bonePosition = CalculatePredictedPosition(Target->GetBonePosition(static_cast<HitboxType>(boneIndex)), Target->AbsoluteVelocity, Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale);

            float boxWidth, boxDepth, boxHeight;

            if (boneIndex == 0) { // Head
                boxWidth = boxDepth = boxHeight = 5.0; // Assuming the head is roughly a cube
            }
            else if (boneIndex == 3) { // Body
                boxWidth = 8.0; // X
                boxDepth = 8.0; // Y
                boxHeight = 12.0; // Z
            }
            else {
                continue;
            }

            // Calculate corner points of the box around the bone in world space
            std::vector<Vector3D> corners = {
                {bonePosition.x + boxWidth, bonePosition.y + boxDepth, bonePosition.z + boxHeight},
                {bonePosition.x - boxWidth, bonePosition.y - boxDepth, bonePosition.z - boxHeight},
                {bonePosition.x + boxWidth, bonePosition.y - boxDepth, bonePosition.z + boxHeight},
                {bonePosition.x - boxWidth, bonePosition.y + boxDepth, bonePosition.z - boxHeight},
            };

            float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;

            for (const auto& corner : corners) {
                Vector2D screenPos;
                if (GameCamera->WorldToScreen(corner, screenPos)) {
                    minX = (std::min)(minX, screenPos.x);
                    maxX = (std::max)(maxX, screenPos.x);
                    minY = (std::min)(minY, screenPos.y);
                    maxY = (std::max)(maxY, screenPos.y);
                }
            }

            Vector2D Center = GameCamera->GetCenter();
            if ((Center.x >= (minX - 1)) && (Center.x <= (maxX + 1)) &&
                (Center.y >= (minY - 1.5)) && (Center.y <= (maxY + 1.5))) {
                LeftClick();
                break;
            }
        }
    }

    void Update_Triggerbot() {
        if (Myself->IsZooming)
            FinalDistance = ZoomDistance;
        else FinalDistance = HipfireDistance;

        if (!Myself->IsCombatReady()) { return; }
        if (!mem.GetKeyboard()->IsKeyDown(AimTriggerKey)) { return; }
        if (Myself->IsHoldingGrenade) { return; }

        Player* Target = CurrentTarget;
        if (Sticky) {
            if (!IsValidTarget(Target)) {
                if (TargetSelected) {
                    return;
                }

                Target = FindBestTarget();
                if (!IsValidTarget(Target)) {
                    ReleaseTarget();
                    return;
                }

                CurrentTarget = Target;
                CurrentTarget->IsLockedOn = true;
                TargetSelected = true;
            }
        }
        else {
            Target = FindBestTarget();
            if (!IsValidTarget(Target)) {
                ReleaseTarget();
                return;
            }

            CurrentTarget = Target;
            CurrentTarget->IsLockedOn = true;
            TargetSelected = true;
        }

        if (TargetSelected && CurrentTarget) {
            std::chrono::milliseconds Now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            if (Now >= LastAimTime + std::chrono::milliseconds(10)) {
                StartTrigger(CurrentTarget);
                LastAimTime = Now + std::chrono::milliseconds((int)Utils::RandomRange(MinimumDelay, 10));
            }
            return;
        }
    }

    void StartTrigger(Player* Target) {
        std::vector<int> bones = { 0, 3 };

        for (int boneIndex : bones) {
            Vector3D bonePosition = CalculatePredictedPosition(Target->GetBonePosition(static_cast<HitboxType>(boneIndex)), Target->AbsoluteVelocity, Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale);

            float boxWidth, boxDepth, boxHeight;

            if (boneIndex == 0) { // Head
                boxWidth = boxDepth = boxHeight = 5.0; // Assuming the head is roughly a cube
            }
            else if (boneIndex == 3) { // Body
                boxWidth = 8.0; // X
                boxDepth = 8.0; // Y
                boxHeight = 12.0; // Z
            }

            // Calculate corner points of the box around the bone in world space
            std::vector<Vector3D> corners = {
                {bonePosition.x + boxWidth, bonePosition.y + boxDepth, bonePosition.z + boxHeight},
                {bonePosition.x - boxWidth, bonePosition.y - boxDepth, bonePosition.z - boxHeight},
                {bonePosition.x + boxWidth, bonePosition.y - boxDepth, bonePosition.z + boxHeight},
                {bonePosition.x - boxWidth, bonePosition.y + boxDepth, bonePosition.z - boxHeight},
            };

            float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;

            for (const auto& corner : corners) {
                Vector2D screenPos;
                if (GameCamera->WorldToScreen(corner, screenPos)) {
                    minX = (std::min)(minX, screenPos.x);
                    maxX = (std::max)(maxX, screenPos.x);
                    minY = (std::min)(minY, screenPos.y);
                    maxY = (std::max)(maxY, screenPos.y);
                }
            }

            Vector2D Center = GameCamera->GetCenter();
            if (Center.x >= minX && Center.x <= maxX && Center.y >= minY && Center.y <= maxY) {
                LeftClick();
                break;
            }
        }
    }

    bool IsValidTarget(Player* target) {
        if (target == nullptr ||
            !target->IsValid() ||
            !target->IsCombatReady() ||
            !target->IsVisible ||
            !target->IsHostile ||
            target->Distance2DToLocalPlayer < Conversion::ToGameUnits(MinDistance) ||
            target->Distance2DToLocalPlayer > Conversion::ToGameUnits(FinalDistance))
            return false;
        return true;
    }

    double CalculateDistanceFromCrosshair(Vector3D TargetPosition) {
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).NormalizeAngles();

        if (CameraPosition.Distance(TargetPosition) <= 0.0001f)
            return -1;

        QAngle TargetAngle = Resolver::CalculateAngle(CameraPosition, TargetPosition);
        if (!TargetAngle.isValid())
            return -1;

        return CurrentAngle.distanceTo(TargetAngle);
    }

    void ReleaseTarget() {
        if (CurrentTarget != nullptr && CurrentTarget->IsValid())
            CurrentTarget->IsLockedOn = false;

        TargetSelected = false;
        CurrentTarget = nullptr;
    }

    float GetFOVScale() {
        if (Myself->IsValid()) {
            if (Myself->IsZooming) {
                if (Myself->TargetZoomFOV > 1.0 && Myself->TargetZoomFOV < 90.0) {
                    return tanf(DEG2RAD(Myself->TargetZoomFOV) * (0.64285714285));
                }
            }
        }
        return 1.0;
    }

    int GetBestBone(Player* Target) {
        float NearestDistance = 999;
        int NearestBone = 1;
        for (int i = 0; i < 3; i++) {
            HitboxType Bone = static_cast<HitboxType>(i);
            double DistanceFromCrosshair = CalculateDistanceFromCrosshair(Target->GetBonePosition(Bone));
            if (DistanceFromCrosshair < NearestDistance) {
                NearestBone = i;
                NearestDistance = DistanceFromCrosshair;
            }
        }
        return NearestBone;
    }

    Player* FindBestTarget() {
        Player* NearestTarget = nullptr;
        float BestDistance = 9999;
        float BestFOV = (std::min)(FOV, FOV * (GetFOVScale() * ZoomScale));
        bool Shotgun = std::find(Shotguns.begin(), Shotguns.end(), Myself->WeaponIndex) != Shotguns.end();
        Vector3D CameraPosition = Myself->CameraPosition;
        for (int i = 0; i < Players->size(); i++) {
            Player* p = Players->at(i);
            if (!IsValidTarget(p)) continue;
            if (p->DistanceToLocalPlayer < Conversion::ToGameUnits(ZoomDistance)) {
                HitboxType BestBone = static_cast<HitboxType>(GetBestBone(p));
                Vector3D TargetPosition = p->GetBonePosition(BestBone);

                float Distance = CameraPosition.Distance(TargetPosition);
                float FOV = CalculateDistanceFromCrosshair(TargetPosition);

                if (Shotgun && Conversion::ToMeters(Distance) < 5.0f) {
                    BestFOV *= 10;
                }

                if (FOV > BestFOV) continue;
                if (Distance > BestDistance) continue;

                NearestTarget = p;
                BestDistance = Distance;
            }
        }
        return NearestTarget;
    }

    float CalculateBulletSpeedScale(float smoothingValue) {
        return 1.00f - (smoothingValue - 1) * 0.10f;
    }

    Vector3D CalculatePredictedPosition(Vector3D targetPosition, Vector3D targetVelocity, float bulletSpeed, float bulletScale) {
        Vector3D playerPosition = Myself->CameraPosition;
        Vector3D enemyVelocity = targetVelocity;
        float bulletSpeedScale = CalculateBulletSpeedScale(Smooth);
        // Bocek
        if (Myself->WeaponIndex == 2) {
            bulletSpeed = 20000;
        }

        // Initial distance to target
        float initialDistance = playerPosition.Distance(targetPosition);

        // Use adjusted bullet speed to calculate initial bullet travel time
        float initialBulletTravelTime = initialDistance / (bulletSpeed*bulletSpeedScale);

        // Scale Velocity based on Distance
        float minScale = 1.0f; // Minimum scale at 0 distance
        float maxScale = 3.0f; // Maximum scale at 300 meters
        float scale = 1.0f + ((std::min)(Conversion::ToMeters(initialDistance), 300.0f) / 300.0f) * (maxScale - minScale);
        enemyVelocity = targetVelocity.Multiply(scale);


        // Initial prediction of future position based on target velocity
        Vector3D initialFuturePosition = targetPosition.Add(enemyVelocity.Multiply(initialBulletTravelTime));

        // Refine bullet travel time using the distance to the initial predicted future position
        float refinedDistance = playerPosition.Distance(initialFuturePosition);
        float refinedBulletTravelTime = refinedDistance / (bulletSpeed * bulletSpeedScale);

        // Final prediction of target position using refined bullet travel time
        Vector3D finalPredictedPosition = targetPosition.Add(enemyVelocity.Multiply(refinedBulletTravelTime));

        // Bullet Drop Prediction
        float drop = Resolver::GetBasicBulletDrop(Myself->CameraPosition, finalPredictedPosition, bulletSpeed, bulletScale);
        finalPredictedPosition.z += drop;

        return finalPredictedPosition;
    }

};
