#pragma once
#include <iostream>
#include <vector>

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

    float Smooth = 2.0f;
    float MaxSmoothIncrease = 0.20f;
    float FOV = 10;
    float ZoomScale = 1.2;
    float MinDistance = 1;
    float HipfireDistance = 50;
    float ZoomDistance = 200;
    int MinimumDelay = 1;

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
				std::cout << "Kmbox B+ Pro failed to initialize." << std::endl;
			}
            else {
				std::cout << "Kmbox B+ Pro initialized successfully." << std::endl;
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
            comPort.write(cmd);
        }
    }

    void Update() {
        if (Myself->IsZooming)
            FinalDistance = ZoomDistance;
        else FinalDistance = HipfireDistance;

        if (!Myself->IsCombatReady()) { CurrentTarget = nullptr; return; }
        if (!mem.GetKeyboard()->IsKeyDown(0x02) && !Myself->IsInAttack) { ReleaseTarget(); return; }
        if (Myself->IsHoldingGrenade) { ReleaseTarget(); return; }

        Player* Target = CurrentTarget;
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
                (RelativePosition.y / dynamicSmoothing)
            };

            // If step is too small, don't move
            if ((std::abs(step.x) < 1.0f) && (std::abs(step.y) < 1.0f)) {
                return;
            }

            Move(step.x, step.y);
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

    void RecoilControl(QAngle& Angle) {
        QAngle CurrentPunch = QAngle(Myself->PunchAngles.x, Myself->PunchAngles.y).NormalizeAngles();

        Angle.x -= CurrentPunch.x;
        Angle.y -= CurrentPunch.y;
    }


    int GetBestBone(Player* Target) {
        float NearestDistance = 999;
        int NearestBone = 2;
        for (int i = 0; i < 6; i++) {
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
        Vector3D CameraPosition = Myself->CameraPosition;
        for (int i = 0; i < Players->size(); i++) {
            Player* p = Players->at(i);
            if (!IsValidTarget(p)) continue;
            if (p->DistanceToLocalPlayer < Conversion::ToGameUnits(ZoomDistance)) {
                HitboxType BestBone = static_cast<HitboxType>(GetBestBone(p));
                Vector3D TargetPosition = p->GetBonePosition(BestBone);

                float Distance = CameraPosition.Distance(TargetPosition);
                float FOV = CalculateDistanceFromCrosshair(TargetPosition);

                if (FOV > BestFOV) continue;
                if (Distance > BestDistance) continue;

                NearestTarget = p;
                BestDistance = Distance;
            }
        }
        return NearestTarget;
    }

    Vector3D CalculatePredictedPosition(Vector3D targetPosition, Vector3D targetVelocity, float bulletSpeed, float bulletScale) {
        const float gravity = 750.0f / bulletScale;
        float timeStep = 0.1f; // Adjust time step for finer prediction granularity
        Vector3D predictedPosition = targetPosition;
        bool foundPrediction = false;
        float baseBulletSpeed = 18000.0f;
        float adjustmentFactor = 0.75f;
        float speedRatio = baseBulletSpeed / bulletSpeed;
        float adjustVelocityScale = Smooth * pow(speedRatio, adjustmentFactor);
        Vector3D velocity = targetVelocity.Multiply(adjustVelocityScale);


        // Iterate to predict the position considering movement and bullet drop
        for (float t = 0.1f; t < 1.f; t += timeStep) { // Arbitrary upper limit of 2 seconds for prediction
            Vector3D futurePosition = Resolver::GetTargetPosition(targetPosition, velocity, t);
            float bulletTravelTime = Resolver::GetTimeToTarget(targetPosition, futurePosition, bulletSpeed);
            
            if (bulletTravelTime <= t) {
                float bulletDrop = Resolver::GetBasicBulletDrop(targetPosition, futurePosition, bulletSpeed, 1 / gravity);
                futurePosition.z += bulletDrop; // Compensate for bullet drop

                // If this position is reachable within the time it takes for the bullet to travel, use it
                predictedPosition = futurePosition;
                foundPrediction = true;
                break; // Stop the loop once a suitable prediction is found
            }
        }

        if (!foundPrediction) {
            // Fallback: Adjust initial target position for bullet drop without movement prediction
            float initialBulletDrop = Resolver::GetBasicBulletDrop(targetPosition, targetPosition, bulletSpeed, 1 / gravity);
            predictedPosition.z += initialBulletDrop;
        }

        return predictedPosition;
    }

};