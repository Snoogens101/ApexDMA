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
#include <array>

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
    uint64_t HighlightSettingsPointer;

    int GlowMaxDistance = 200;

    int GlowRadius = 64;
    int InsideFunction = 2; //Leave
    int OutlineFunction = 125; //Leave
    int BodyStyle = 13;
    int OutlineStyle = 1;

    //Colors
    float InvisibleGlowColor[3] = { 1, 0, 0 };
    float VisibleGlowColor[3] = { 0, 1, 0 };
    float LowGlowColor[3] = { 1, 1, 0 };

    float RedShieldColor[3] = { 1, 0, 0 };
    float PurpleShieldColor[3] = { 0.5, 0, 0.5 };
    float BlueShieldColor[3] = { 0, 0.5, 1 };
    float GreyShieldColor[3] = { 0.540, 0.545, 0.545 };


    Sense(std::vector<Player*>* Players, Camera* GameCamera, LocalPlayer* Myself) {
        this->Players = Players;
        this->GameCamera = GameCamera;
        this->Myself = Myself;
    }

    void Initialize() {

    }

    void setCustomGlow(Player* Target, int enable, int wall, bool isVisible)
    {
        uint64_t basePointer = Target->BasePointer;
        static const int contextId = 0;
        int settingIndex = 65;

        //Custom Glow Body Style
        if (BodyStyle == 13) { //Bright
            InsideFunction = 109;
        }

        //Custom Outline Style
        if (OutlineStyle == 1) { //Bright
            OutlineFunction = 6;
        }

        std::array<unsigned char, 4> highlightFunctionBits = {
            InsideFunction,   // InsideFunction							2
            OutlineFunction, // OutlineFunction: HIGHLIGHT_OUTLINE_OBJECTIVE			125
            GlowRadius,  // OutlineRadius: size * 255 / 8				64
            64   // (EntityVisible << 6) | State & 0x3F | (AfterPostProcess << 7) 	64
        };
        std::array<float, 3> glowColorRGB = { 0, 0, 0 };

        if (!isVisible) {
            settingIndex = 65;
            glowColorRGB = { VisibleGlowColor[0], VisibleGlowColor[1], VisibleGlowColor[2] }; // Visible Enemies
        }
        else if (isVisible) {
            settingIndex = 70;
            glowColorRGB = { InvisibleGlowColor[0], InvisibleGlowColor[1], InvisibleGlowColor[2] }; // Invisible Enemies
        }

        // Create scatter handle
        auto handle = mem.CreateScatterHandle();
        if (Target->GlowEnable != enable)
        {
            uint64_t glowEnableAddress = basePointer + OFF_GLOW_ENABLE;
            int value = enable;
			mem.AddScatterWriteRequest(handle, glowEnableAddress, &value, sizeof(int));
        }

        if (Target->GlowThroughWall != wall)
        {
            uint64_t glowThroughWallAddress = basePointer + OFF_GLOW_THROUGH_WALL;
            int value = wall;
            mem.AddScatterWriteRequest(handle, glowThroughWallAddress, &value, sizeof(value));
        }

        uint64_t glowFixAddress = basePointer + OFF_GLOW_FIX;
        int valueB = 0;
        mem.AddScatterWriteRequest(handle, glowFixAddress, &valueB, sizeof(int));

        // Execute the scatter write
        mem.ExecuteWriteScatter(handle);

        // Close the scatter handle
        mem.CloseScatterHandle(handle);

        uint64_t highlightIdAddress = basePointer + OFF_GLOW_HIGHLIGHT_ID;
        unsigned char valueA = settingIndex;
        mem.Write<unsigned char>(highlightIdAddress, valueA);

        uint64_t highlightSettingsPtr = HighlightSettingsPointer;
        mem.Write<decltype(highlightFunctionBits)>(highlightSettingsPtr + 0x34 * settingIndex + 0, highlightFunctionBits);
        mem.Write<decltype(glowColorRGB)>(highlightSettingsPtr + 0x34 * settingIndex + 4, glowColorRGB);

    }


    void Update() {
        for (int i = 0; i < Players->size(); i++) {
            Player* Target = Players->at(i);
            if (!Target->IsValid()) continue;
            if (Target->IsDummy()) continue;
            if (!Target->IsHostile) continue;

            setCustomGlow(Target, 1, 1, Target->IsVisible);
        }
    }
};

