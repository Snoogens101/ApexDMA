#pragma once
#include "Offsets.hpp"
#include "DMALibrary/Memory/Memory.h"

struct Level {
    std::string Name;
    bool IsPlayable;
    bool IsFiringRange;

    void Read() {
        Name = mem.Read<std::string>(OFF_REGION + OFF_LEVEL);
        IsPlayable = !Name.empty() && Name != "mp_lobby";
        IsFiringRange = Name == "mp_rr_canyonlands_staging_mu1";
    }
};