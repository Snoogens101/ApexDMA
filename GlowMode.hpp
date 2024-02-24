#pragma once
#include <iostream>

struct GlowMode {
    std::byte bodyStyle, borderStyle, borderWidth, transparency;

    GlowMode() {}

    GlowMode(int x_val, int y_val, int z_val, int transparency_val) :
        bodyStyle(static_cast<std::byte>(x_val)),
        borderStyle(static_cast<std::byte>(y_val)),
        borderWidth(static_cast<std::byte>(z_val)),
        transparency(static_cast<std::byte>(transparency_val)) {}

    GlowMode(std::byte x_val, std::byte y_val, std::byte z_val, std::byte transparency_val) :
        bodyStyle(x_val),
        borderStyle(y_val),
        borderWidth(z_val),
        transparency(transparency_val) {}

    bool IsZeroVector() const {
        return bodyStyle == std::byte(0)
            && borderStyle == std::byte(0)
            && borderWidth == std::byte(0)
            && borderWidth == std::byte(0);
    }

    bool operator==(const GlowMode& other) const {
        return bodyStyle == other.bodyStyle
            && borderStyle == other.borderStyle
            && borderWidth == other.borderWidth
            && transparency == other.transparency;
    }

    bool operator!=(const GlowMode& other) const {
        return !(*this == other);
    }
};