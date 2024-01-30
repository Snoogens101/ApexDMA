#pragma once
#include <random>

namespace Conversion {
    float ToGameUnits(float Meters) {
        return 39.37007874 * Meters;
    }

    float ToMeters(float GameUnits) {
        return GameUnits / 39.37007874;
    }
};

namespace Utils {
    inline float RandomFloat() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        return dis(gen);
    }

    inline float RandomRange(float min, float max) {
        if (min > max) {
            float Temp = min;
            min = max;
            max = Temp;
        }
        return RandomFloat() * (max - min) + min;
    }
}