#pragma once
struct Color {
    float red;
    float green;
    float blue;

    bool operator==(const Color& other) const {
        return (red == other.red) && (green == other.green) && (blue == other.blue);
    }

    bool operator!=(const Color& other) const {
        return !(*this == other);
    }
};