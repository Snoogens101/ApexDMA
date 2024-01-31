#pragma once
#include <iostream>
#include <vector>
#include "Player.hpp"
#include "LocalPlayer.hpp"
#include "Offsets.hpp"
#include "GlowMode.hpp"
#include "Camera.hpp"

#include "DMALibrary/Memory/Memory.h"
#include "Color.hpp"
#include "Conversion.hpp"
#include "HitboxType.hpp"

// Geometry
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI / 180.f) )

struct Sense {
    // Variables
    Camera* GameCamera;
    LocalPlayer* Myself;
    std::vector<Player*>* Players;
    std::chrono::milliseconds LastUpdateTime;
    int TotalSpectators = 0;
    std::vector<std::string> Spectators;

    Sense(std::vector<Player*>* Players, Camera* GameCamera, LocalPlayer* Myself) {
        this->Players = Players;
        this->GameCamera = GameCamera;
        this->Myself = Myself;
    }

    // Item Glow
    std::vector<GlowMode>* StoredGlowMode = new std::vector<GlowMode>;

    void Initialize() {
        for (int placebo = 0; placebo < 31; placebo++) {
            const GlowMode Ehh = { 135, 132, 35, 127 };
            StoredGlowMode->push_back(Ehh);
        }

        const GlowMode FirstStyle = { 135, 135, 128, 64 };
        const GlowMode SecondStyle = { 135, 135, 160, 64 };
        const GlowMode ThirdStyle = { 135, 135, 255, 64 };
        const GlowMode FourthStyle = { 135, 135, 32, 64 };

        StoredGlowMode->push_back(FirstStyle);
        StoredGlowMode->push_back(SecondStyle);
        StoredGlowMode->push_back(ThirdStyle);
        StoredGlowMode->push_back(FourthStyle);
    }


    void SetGlowState(long long HighlightSettingsPointer, long HighlightSize, int HighlightID, GlowMode NewGlowMode) {
        const GlowMode oldGlowMode = mem.Read<GlowMode>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 4);
        if (NewGlowMode != oldGlowMode)
            mem.Write<GlowMode>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 4, NewGlowMode);
    }

    void SetColorState(long long HighlightSettingsPointer, long HighlightSize, int HighlightID, Color NewColor) {
        const Color oldColor = mem.Read<Color>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 8);
        if (oldColor != NewColor)
            mem.Write<Color>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 8, NewColor);
    }

    void SetGlow(Player* Target, int GlowEnabled, int GlowThroughWall, int HighlightID) {
        std::cout << "Glow Enabled on Player: " << Target->GlowEnable << std::endl;
        std::cout << "Condition GlowEnable != GlowEnabled: " << (Target->GlowEnable != GlowEnabled) << std::endl;

        if (Target->GlowEnable != GlowEnabled) {
            mem.Write<int>(Target->BasePointer + OFF_GLOW_ENABLE, GlowEnabled);
            Target->GlowEnable = GlowEnabled;
            std::cout << "Glow Enabled: " << GlowEnabled << std::endl;
        }

        if (Target->GlowThroughWall != GlowThroughWall) {
            mem.Write<int>(Target->BasePointer + OFF_GLOW_THROUGH_WALL, GlowThroughWall);
            mem.Write<int>(Target->BasePointer + OFF_GLOW_FIX, 2);
            Target->GlowThroughWall = GlowThroughWall;
            std::cout << "Glow Through Wall: " << GlowThroughWall << std::endl;
        }
        
        if (Target->HighlightID != HighlightID) {
            mem.Write<int>(Target->BasePointer + OFF_GLOW_HIGHLIGHT_ID + 1, HighlightID);
            Target->HighlightID = HighlightID;
            std::cout << "Highlight ID: " << HighlightID << std::endl;
        }
    }

    void Update() {
        const long long HighlightSettingsPointer = mem.Read<long long>(OFF_BASE + OFF_GLOW_HIGHLIGHTS);
        const long HighlightSize = 0x34;

        for (int highlightId = 31; highlightId < 35; highlightId++) {
            const GlowMode newGlowMode = StoredGlowMode->at(highlightId);
            SetGlowState(HighlightSettingsPointer, HighlightSize, highlightId, newGlowMode);
        }

        // Player Glow //
        // -> Visible
        const GlowMode VisibleMode = { 2, 6, 32, 127 };
        const Color VisibleColor = { 0.6, 3, 2.04 };
        SetGlowState(HighlightSettingsPointer, HighlightSize, 0, VisibleMode);
        SetColorState(HighlightSettingsPointer, HighlightSize, 0, VisibleColor);

        // -> Invisible
        const GlowMode InvisibleMode = { 2, 6, 32, 100 };
        const Color InvisibleColor = { 4.5, 0.6, 0.6 };
        SetGlowState(HighlightSettingsPointer, HighlightSize, 1, InvisibleMode);
        SetColorState(HighlightSettingsPointer, HighlightSize, 1, InvisibleColor);

        // -> Knocked
        const GlowMode KnockedMode = { 2, 6, 32, 127 };
        const Color KnockedColor = { 1, 1, 0.35 };
        SetGlowState(HighlightSettingsPointer, HighlightSize, 90, KnockedMode);
        SetColorState(HighlightSettingsPointer, HighlightSize, 90, KnockedColor);

        // -> Disabled
        const GlowMode DisabledMode = { 0, 0, 0, 0 };
        const Color DisabledColor = { 1, 1, 1 };
        SetGlowState(HighlightSettingsPointer, HighlightSize, 91, DisabledMode);
        SetColorState(HighlightSettingsPointer, HighlightSize, 91, DisabledColor);

        // -> Locked On
        const GlowMode LockedOnMode = { 136, 6, 32, 127 };
        const Color LockedOnColor = { 0, 0.75, 0.75 };
        SetGlowState(HighlightSettingsPointer, HighlightSize, 92, LockedOnMode);
        SetColorState(HighlightSettingsPointer, HighlightSize, 92, LockedOnColor);

        for (int i = 0; i < Players->size(); i++) {
            Player* Target = Players->at(i);
            if (!Target->IsValid()) continue;
            if (Target->IsDummy()) continue;
            if (Target->IsHostile) continue;

            std::cout << "---" << std::endl;
            std::cout << "Index: " << i << std::endl;
            std::cout << "PlayerIndex " << Target->Index << std::endl;
            // Print Distance
            //std::cout << "Distance: " << Target->DistanceToLocalPlayer << std::endl;
            //std::cout << "---" << std::endl;

            SetGlow(Target, 1, 2, 1);
        }
    }
};

