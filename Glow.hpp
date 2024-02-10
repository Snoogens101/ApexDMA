#pragma once
#include <iostream>
#include <vector>
#include "Player.hpp"
#include "LocalPlayer.hpp"
#include "Offsets.hpp"
#include "Camera.hpp"

#include "DMALibrary/Memory/Memory.h"
#include "Conversion.hpp"
#include "HitboxType.hpp"
#include <array>

struct Sense {
    // Variables
    Camera* GameCamera;
    LocalPlayer* Myself;
    std::vector<Player*>* Players;
    std::chrono::milliseconds LastUpdateTime;
    int TotalSpectators = 0;
    std::vector<std::string> Spectators;
    uint64_t HighlightSettingsPointer;

    //Colors
    float InvisibleGlowColor[3] = { 0, 1, 0 };
    float VisibleGlowColor[3] = { 1, 0, 0 };

    // Checks
    std::vector<int> GlowEnableInts = { 65, 70 };
    std::vector<int> GlowThroughWallInts = { 1 };
    std::vector<int> HighlightIDInts = { 872415232 };

    Sense(std::vector<Player*>* Players, Camera* GameCamera, LocalPlayer* Myself) {
        this->Players = Players;
        this->GameCamera = GameCamera;
        this->Myself = Myself;
    }

    void Initialize() {
        // idk, nothing for now
    }

    void setCustomGlow(Player* Target, int enable, int wall, bool isVisible)
    {
        if (Target->GlowEnable == 0 && Target->GlowThroughWall == 0 && Target->HighlightID == 0) {
			return;
		}
        uint64_t basePointer = Target->BasePointer;
        int settingIndex = 65;
        std::array<float, 3> glowColorRGB = { 0, 0, 0 };

        if (!isVisible) {
            settingIndex = 65;
            glowColorRGB = { InvisibleGlowColor[0], InvisibleGlowColor[1], InvisibleGlowColor[2] }; // Invisible Enemies
        }
        else if (isVisible) {
            settingIndex = 70;
            glowColorRGB = { VisibleGlowColor[0], VisibleGlowColor[1], VisibleGlowColor[2] }; // Visible Enemies
        }

        if (std::find(GlowEnableInts.begin(), GlowEnableInts.end(), Target->GlowEnable) == GlowEnableInts.end()) {
            uint64_t glowEnableAddress = basePointer + OFF_GLOW_ENABLE;
            mem.Write<int>(glowEnableAddress, enable);
        }

        if (std::find(GlowThroughWallInts.begin(), GlowThroughWallInts.end(), Target->GlowThroughWall) == GlowThroughWallInts.end()) {
            uint64_t glowThroughWallAddress = basePointer + OFF_GLOW_THROUGH_WALL;
            mem.Write<int>(glowThroughWallAddress, wall);
        }

        uint64_t highlightIdAddress = basePointer + OFF_GLOW_HIGHLIGHT_ID;
        unsigned char value = settingIndex;
        mem.Write<unsigned char>(highlightIdAddress, value);

        uint64_t glowFixAddress = basePointer + OFF_GLOW_FIX;
        mem.Write<int>(glowFixAddress, 0);
    }

    void setHighlightSettings() {
        int InvisibleIndex = 65; // Invis
        int VisibleIndex = 70; // Vis
        std::array<unsigned char, 4> highlightFunctionBits = {
            109,   // InsideFunction							2
            6, // OutlineFunction: HIGHLIGHT_OUTLINE_OBJECTIVE			125
            64,  // OutlineRadius: size * 255 / 8				64
            64   // (EntityVisible << 6) | State & 0x3F | (AfterPostProcess << 7) 	64
        };
        std::array<float, 3> invisibleGlowColorRGB = { 0, 1, 0 };
        std::array<float, 3> visibleGlowColorRGB = { 1, 0, 0 };

        uint64_t highlightSettingsPtr = HighlightSettingsPointer;
        if (mem.IsValidPointer(highlightSettingsPtr)) {
            auto handle = mem.CreateScatterHandle();

            // Invisible
            mem.AddScatterWriteRequest(handle, highlightSettingsPtr + OFF_GLOW_HIGHLIGHT_TYPE_SIZE * InvisibleIndex + 0x0, &highlightFunctionBits, sizeof(highlightFunctionBits));
            mem.AddScatterWriteRequest(handle, highlightSettingsPtr + OFF_GLOW_HIGHLIGHT_TYPE_SIZE * InvisibleIndex + 0x4, &invisibleGlowColorRGB, sizeof(invisibleGlowColorRGB));

            // Visible
            mem.AddScatterWriteRequest(handle, highlightSettingsPtr + OFF_GLOW_HIGHLIGHT_TYPE_SIZE * VisibleIndex + 0x0, &highlightFunctionBits, sizeof(highlightFunctionBits));
            mem.AddScatterWriteRequest(handle, highlightSettingsPtr + OFF_GLOW_HIGHLIGHT_TYPE_SIZE * VisibleIndex + 0x4, &visibleGlowColorRGB, sizeof(visibleGlowColorRGB));

            mem.ExecuteWriteScatter(handle);
            mem.CloseScatterHandle(handle);
        }

    }

    Vector2D DummyVector = { 0, 0 };
    void Update() {
        if (Myself->IsDead) return;

        for (int i = 0; i < Players->size(); i++) {
            Player* Target = Players->at(i);
            if (!Target->IsValid()) continue;
            if (Target->IsDummy()) continue;
            if (Target->IsLocal) continue;
            if (!Target->IsHostile) continue;
            if (Target->LocalOrigin.IsZeroVector()) continue;

            if (GameCamera->WorldToScreen(Target->LocalOrigin.ModifyZ(30), DummyVector)) {
                setCustomGlow(Target, 1, 1, Target->IsVisible);
            }
        }

        setHighlightSettings();
    }
};

// TODO:
// Add 

// GlowEnable: -15452929 | GlowThroughWall: 0 | HighlightID: 889132084
// GlowEnable: ff1434ff | GlowThroughWall: 0 | HighlightID: 34ff1434

////if (Target->DistanceToLocalPlayer <= Conversion::ToGameUnits(300)) {
//setCustomGlow(Target, 1, 1, Target->IsVisible);
////}
////else {
////    removeCustomGlow(Target);
////}




//        // Create scatter handle
//auto handle = mem.CreateScatterHandle();
//if (Target->GlowEnable != enable)
//{
//    uint64_t glowEnableAddress = basePointer + OFF_GLOW_ENABLE;
//    int value = enable;
//    mem.AddScatterWriteRequest(handle, glowEnableAddress, &value, sizeof(int));
//}
//
//if (Target->GlowThroughWall != wall)
//{
//    uint64_t glowThroughWallAddress = basePointer + OFF_GLOW_THROUGH_WALL;
//    int value = wall;
//    mem.AddScatterWriteRequest(handle, glowThroughWallAddress, &value, sizeof(int));
//}
//
//uint64_t glowFixAddress = basePointer + OFF_GLOW_FIX;
//int valueB = 0;
//mem.AddScatterWriteRequest(handle, glowFixAddress, &valueB, sizeof(int));
//
//// Execute the scatter write
//mem.ExecuteWriteScatter(handle);
//
//// Close the scatter handle
//mem.CloseScatterHandle(handle);





//void setCustomGlow(Player* Target, int enable, int wall, bool isVisible)
//{
//    if (Target->GlowEnable == 0 && Target->GlowThroughWall == 0 && Target->HighlightID == 0) {
//        return;
//    }
//    uint64_t basePointer = Target->BasePointer;
//    int settingIndex = 65;
//
//    std::array<unsigned char, 4> highlightFunctionBits = {
//        109,   // InsideFunction							2
//        6, // OutlineFunction: HIGHLIGHT_OUTLINE_OBJECTIVE			125
//        64,  // OutlineRadius: size * 255 / 8				64
//        64   // (EntityVisible << 6) | State & 0x3F | (AfterPostProcess << 7) 	64
//    };
//    std::array<float, 3> glowColorRGB = { 0, 0, 0 };
//
//    if (!isVisible) {
//        settingIndex = 65;
//        glowColorRGB = { InvisibleGlowColor[0], InvisibleGlowColor[1], InvisibleGlowColor[2] }; // Invisible Enemies
//    }
//    else if (isVisible) {
//        settingIndex = 70;
//        glowColorRGB = { VisibleGlowColor[0], VisibleGlowColor[1], VisibleGlowColor[2] }; // Visible Enemies
//    }
//
//    if (std::find(GlowEnableInts.begin(), GlowEnableInts.end(), Target->GlowEnable) == GlowEnableInts.end()) {
//        uint64_t glowEnableAddress = basePointer + OFF_GLOW_ENABLE;
//        mem.Write<int>(glowEnableAddress, enable);
//    }
//
//    if (std::find(GlowThroughWallInts.begin(), GlowThroughWallInts.end(), Target->GlowThroughWall) == GlowThroughWallInts.end()) {
//        uint64_t glowThroughWallAddress = basePointer + OFF_GLOW_THROUGH_WALL;
//        mem.Write<int>(glowThroughWallAddress, wall);
//    }
//
//    uint64_t highlightIdAddress = basePointer + OFF_GLOW_HIGHLIGHT_ID;
//    unsigned char value = settingIndex;
//    mem.Write<unsigned char>(highlightIdAddress, value);
//
//    uint64_t highlightSettingsPtr = HighlightSettingsPointer;
//    if (mem.IsValidPointer(highlightSettingsPtr)) {
//        mem.Write<decltype(highlightFunctionBits)>(highlightSettingsPtr + OFF_GLOW_HIGHLIGHT_TYPE_SIZE * settingIndex + 0x0, highlightFunctionBits);
//        mem.Write<decltype(glowColorRGB)>(highlightSettingsPtr + OFF_GLOW_HIGHLIGHT_TYPE_SIZE * settingIndex + 0x4, glowColorRGB);
//    }
//
//    uint64_t glowFixAddress = basePointer + OFF_GLOW_FIX;
//    mem.Write<int>(glowFixAddress, 0);
//}
//
//Vector2D DummyVector = { 0, 0 };
//void Update() {
//    for (int i = 0; i < Players->size(); i++) {
//        Player* Target = Players->at(i);
//        if (!Target->IsValid()) continue;
//        if (Target->IsDummy()) continue;
//        if (Target->IsLocal) continue;
//        if (!Target->IsHostile) continue;
//        if (Target->LocalOrigin.IsZeroVector()) continue;
//
//        if (GameCamera->WorldToScreen(Target->LocalOrigin.ModifyZ(30), DummyVector)) {
//            setCustomGlow(Target, 1, 1, Target->IsVisible);
//        }
//    }
//}