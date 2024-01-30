#pragma once
#include "Offsets.hpp"
#include "DMALibrary/Memory/Memory.h"

struct Level {
    std::string Name;
    bool IsPlayable;
    bool IsFiringRange;

    void Read() {
        char buffer[1024] = { 0 };
        bool success = mem.Read(OFF_BASE + OFF_LEVEL, &buffer, 1024);

        if (!success) {
            Name = "Unknown";
        }
        else {
            Name = std::string(buffer);
        }
        std::cout << "Level: " << Name << std::endl;
        IsPlayable = !Name.empty() && Name != "mp_lobby";
        IsFiringRange = Name == "mp_rr_canyonlands_staging_mu1";
    }
};