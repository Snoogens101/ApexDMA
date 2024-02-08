// Config.hpp
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Aimbot.hpp"
#include "Camera.hpp"

class Config {
public:
    Config(const std::string& filePath, Aimbot* aimAssistance, Camera* gameCamera)
        : configFilePath(filePath), AimAssistance(aimAssistance), GameCamera(gameCamera) {}

    void Update() {
        std::ifstream configFile(configFilePath);
        std::string line;

        if (!configFile.is_open()) {
            std::cerr << "Failed to open config file: " << configFilePath << std::endl;
            return;
        }

        while (getline(configFile, line)) {
            std::istringstream lineStream(line);
            std::string key, value;
            if (getline(lineStream, key, '=') && getline(lineStream, value)) {
                updateVariable(key, value);
            }
        }
    }

private:
    std::string configFilePath;
    Aimbot* AimAssistance;
    Camera* GameCamera;

    void updateVariable(const std::string& key, const std::string& value) {
        if (key == "AimFOV") {
			AimAssistance->FOV = std::stof(value);
		}
        if (key == "AimSmooth") {
            AimAssistance->Smooth = std::stof(value);
        }
        if (key == "AimSmoothMaxIncrease") {
            AimAssistance->MaxSmoothIncrease = std::stof(value);
		}
        if (key == "AimRecoilCompensation") {
            AimAssistance->RecoilCompensation = std::stof(value);
        }
        if (key == "ResolutionX") {
			GameCamera->ScreenSize.x = std::stoi(value);
		}
        if (key == "ResolutionY") {
            GameCamera->ScreenSize.y = std::stoi(value);
        }
        if (key == "FOV") {
			GameCamera->FOV = std::stof(value);
		}
        if (key == "KmboxType") {
			AimAssistance->KmboxType = value;
		}
        if (key == "KmboxIP") {
            // Ensure we don't exceed the buffer size, including space for the null terminator
            std::strncpy(AimAssistance->KmboxIP, value.c_str(), sizeof(AimAssistance->KmboxIP) - 1);
            // Ensure null termination
            AimAssistance->KmboxIP[sizeof(AimAssistance->KmboxIP) - 1] = '\0';
        }
        if (key == "KmboxPort") {
			// Ensure we don't exceed the buffer size, including space for the null terminator
            std::strncpy(AimAssistance->KmboxPort, value.c_str(), sizeof(AimAssistance->KmboxPort) - 1);
			// Ensure null termination
			AimAssistance->KmboxPort[sizeof(AimAssistance->KmboxPort) - 1] = '\0';
		}
        if (key == "KmboxUUID") {
			// Ensure we don't exceed the buffer size, including space for the null terminator
			std::strncpy(AimAssistance->KmboxUUID, value.c_str(), sizeof(AimAssistance->KmboxUUID) - 1);
			// Ensure null termination
			AimAssistance->KmboxUUID[sizeof(AimAssistance->KmboxUUID) - 1] = '\0';
        }
        if (key == "KmboxComPort") {
            AimAssistance->KmboxComPort = std::stoi(value);
        }
    }
};
