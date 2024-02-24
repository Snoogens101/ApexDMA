// Config.hpp
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Aimbot.hpp"
#include "Camera.hpp"
#include "Glow.hpp"

class Config {
public:
    static Config& GetInstance() {
        static Config instance;
        return instance;
    }

    // Method to initialize the Config instance.
    void Initialize(const std::string& filePath, Aimbot* aimAssistance, Camera* gameCamera, Sense* esp) {
        if (!initialized) {
            configFilePath = filePath;
            AimAssistance = aimAssistance;
            GameCamera = gameCamera;
            ESP = esp;

            Update();
            initialized = true;
        }
    }

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

    void Save() {
        std::ofstream configFile(configFilePath);

        if (!configFile.is_open()) {
            std::cerr << "Failed to open config file: " << configFilePath << std::endl;
            return;
        }

        configFile << "AimSticky=" << (AimAssistance->Sticky ? "true" : "false") << std::endl;
        configFile << "AimFOV=" << AimAssistance->FOV << std::endl;
        configFile << "AimSmooth=" << AimAssistance->Smooth << std::endl;
        configFile << "AimSmoothMaxIncrease=" << AimAssistance->MaxSmoothIncrease << std::endl;
        configFile << "AimRecoilCompensation=" << AimAssistance->RecoilCompensation << std::endl;
        configFile << "AimBotKey=" << AimAssistance->AimBotKey << std::endl;
        configFile << "AimTriggerKey=" << AimAssistance->AimTriggerKey << std::endl;
        configFile << "AimFlickKey=" << AimAssistance->AimFlickKey << std::endl;
        configFile << "GlowItem=" << (ESP->ItemGlow ? "true" : "false") << std::endl;
        configFile << "GlowItemRarity=" << ESP->MinimumItemRarity << std::endl;
        configFile << "ResolutionX=" << GameCamera->ScreenSize.x << std::endl;
		configFile << "ResolutionY=" << GameCamera->ScreenSize.y << std::endl;
		configFile << "FOV=" << GameCamera->FOV << std::endl;
		configFile << "KmboxType=" << AimAssistance->KmboxType << std::endl;
		configFile << "KmboxIP=" << AimAssistance->KmboxIP << std::endl;
		configFile << "KmboxPort=" << AimAssistance->KmboxPort << std::endl;
		configFile << "KmboxUUID=" << AimAssistance->KmboxUUID << std::endl;
		configFile << "KmboxComPort=" << AimAssistance->KmboxComPort << std::endl;
    }

private:
    std::string configFilePath;
    Aimbot* AimAssistance = nullptr;
    Camera* GameCamera = nullptr;
    Sense* ESP = nullptr;
    bool initialized = false;

    // Private constructor
    Config() {}

    // Existing methods...

    // Delete copy constructor and assignment operator
    Config(Config const&) = delete;
    void operator=(Config const&) = delete;

    void updateVariable(const std::string& key, const std::string& value) {
        if (key == "AimSticky") {
            if (value == "true") {
                AimAssistance->Sticky = true;
            }
            else if (value == "false") {
                AimAssistance->Sticky = false;
            }
		}
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
        if (key == "AimBotKey") {
			AimAssistance->AimBotKey = std::stoi(value);
		}
		if (key == "AimTriggerKey") {
			AimAssistance->AimTriggerKey = std::stoi(value);
		}
        if (key == "AimFlickKey") {
            AimAssistance->AimFlickKey = std::stoi(value);
        }
		if (key == "GlowItem") {
            if (value == "true") {
                ESP->ItemGlow = true;
            }
            else if (value == "false") {
                ESP->ItemGlow = false;
            }
		}
        if (key == "GlowItemRarity") {
			ESP->MinimumItemRarity = std::stoi(value);
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
