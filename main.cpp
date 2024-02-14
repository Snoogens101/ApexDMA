#include <iostream>
#include <thread>
#include <unordered_set>
#include <locale>

#include "DMALibrary/Memory/Memory.h"
#include "Level.hpp"
#include "Player.hpp"
#include "LocalPlayer.hpp"
#include "Camera.hpp"
#include "Glow.hpp"
#include "Aimbot.hpp"
#include "Config.hpp"
#include "Spectator.hpp"
#include "Profiling.hpp"

#include "Render.hpp"


// Game Objects
Level* Map = new Level();
LocalPlayer* Myself = new LocalPlayer();
Camera* GameCamera = new Camera();

// Players
std::vector<Player*>* HumanPlayers = new std::vector<Player*>;
std::vector<Player*>* Dummies = new std::vector<Player*>;
std::vector<Player*>* Players = new std::vector<Player*>;

// Features
Sense* ESP = new Sense(Players, GameCamera, Myself);
Aimbot* AimAssist = new Aimbot(Myself, Players, GameCamera);
Spectator* Spectators = new Spectator(Players, Myself);

void MiscBaseScatter(Level* map, LocalPlayer* myself, Camera* gameCamera, Sense* esp) {
	// Create scatter handle
	auto handle = mem.CreateScatterHandle();

	// Scatter read request for Level
	uint64_t levelAddress = mem.OFF_BASE + OFF_LEVEL;
	mem.AddScatterReadRequest(handle, levelAddress, &map->NameBuffer, sizeof(map->NameBuffer));

	// Scatter read request for LocalPlayer BasePointer
	uint64_t localPlayerAddress = mem.OFF_BASE + OFF_LOCAL_PLAYER;
	mem.AddScatterReadRequest(handle, localPlayerAddress, &myself->BasePointer, sizeof(uint64_t));

	// Scatter read request for LocalPlayer inAttack
	uint64_t inAttackAddress = mem.OFF_BASE + OFF_INATTACK;
	mem.AddScatterReadRequest(handle, inAttackAddress, &myself->IsInAttack, sizeof(uint32_t));

	// Scatter read request for GameCamera
	uint64_t cameraRenderPointerAddress = mem.OFF_BASE + OFF_VIEWRENDER;
	mem.AddScatterReadRequest(handle, cameraRenderPointerAddress, &gameCamera->RenderPointer, sizeof(uint64_t));

    // Scatter read request for HighlightSettingsPointer
    uint64_t highlightSettingsPointerAddress = mem.OFF_BASE + OFF_GLOW_HIGHLIGHTS;
    mem.AddScatterReadRequest(handle, highlightSettingsPointerAddress, &esp->HighlightSettingsPointer, sizeof(uint64_t));

	// Execute the scatter read
	mem.ExecuteReadScatter(handle);

	// Close the scatter handle
	mem.CloseScatterHandle(handle);
}

void PlayerBasePointerScatter(std::vector<Player*>& players) {
    // Create scatter handle
    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < players.size(); ++i) {
        int index = players[i]->Index;
        uint64_t address = mem.OFF_BASE + OFF_ENTITY_LIST + ((static_cast<unsigned long long>(index) + 1) << 5);
        mem.AddScatterReadRequest(handle, address, &players[i]->BasePointer, sizeof(uint64_t));
    }

    // Execute the scatter read
    mem.ExecuteReadScatter(handle);

    // Close the scatter handle
    mem.CloseScatterHandle(handle);
}

void ScatterReadTeamAndName(std::vector<Player*>& players) {
    // Create scatter handle
    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < players.size(); ++i) {
        Player* player = players[i];

        // Verify that the BasePointer is not 0 before adding scatter read requests
        if (mem.IsValidPointer(player->BasePointer)) {
            // Scatter read request for NameBuffer
            uint64_t nameBufferAddress = player->BasePointer + OFF_NAME;
            mem.AddScatterReadRequest(handle, nameBufferAddress, player->NameBuffer, sizeof(player->NameBuffer));

            // Scatter read request for Team
            uint64_t teamAddress = player->BasePointer + OFF_TEAM_NUMBER;
            mem.AddScatterReadRequest(handle, teamAddress, &player->Team, sizeof(int));
        }
    }
    // Execute the scatter read
    mem.ExecuteReadScatter(handle);

    // Convert NameBuffer to a std::string for the Name field
    for (size_t i = 0; i < players.size(); ++i) {
        Player* player = players[i];
        if (player->BasePointer != 0) {
            // Convert the NameBuffer to a std::string for the Name field
            player->Name = std::string(player->NameBuffer);
        }
    }

    // Close the scatter handle
    mem.CloseScatterHandle(handle);
}

void ScatterReadPlayerValidity(std::vector <Player*>& players) {
	// Create scatter handle
	auto handle = mem.CreateScatterHandle();

	for (size_t i = 0; i < players.size(); ++i) {
		Player* player = players[i];

		// Verify that the BasePointer is not 0 before adding scatter read requests
		if (mem.IsValidPointer(player->BasePointer)) {
			// Scatter read request for IsPlayer
			uint64_t validAddress = player->BasePointer;
			mem.AddScatterReadRequest(handle, validAddress, &player->Valid, sizeof(uint64_t));
		}
	}

	// Execute the scatter read
	mem.ExecuteReadScatter(handle);

	// Close the scatter handle
	mem.CloseScatterHandle(handle);

    for (int i = 0; i < Players->size(); i++) {
        Player* p = Players->at(i);
        p->ValidCheck();
    }
}

void ScatterReadPlayerAttributes(std::vector<Player*>& players) {
    // Create scatter handle
    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < players.size(); ++i) {
        Player* player = players[i];

        if (mem.IsValidPointer(player->BasePointer) && !player->IsPlayer() && !Map->IsFiringRange) {
            player->BasePointer = 0;
            continue;
        }

        // Verify that the BasePointer is not 0 before adding scatter read requests
        if (mem.IsValidPointer(player->BasePointer)) {
            if (player->IsPlayer()) {
                // Scatter read request for IsDead
                uint64_t isDeadAddress = player->BasePointer + OFF_LIFE_STATE;
                mem.AddScatterReadRequest(handle, isDeadAddress, &player->IsDead, sizeof(bool));

                // Scatter read request for IsKnocked
                uint64_t isKnockedAddress = player->BasePointer + OFF_BLEEDOUT_STATE;
                mem.AddScatterReadRequest(handle, isKnockedAddress, &player->IsKnocked, sizeof(bool));

                // Scatter read request for AbsoluteVelocity
                uint64_t absoluteVelocityAddress = player->BasePointer + OFF_ABSVELOCITY;
                mem.AddScatterReadRequest(handle, absoluteVelocityAddress, &player->AbsoluteVelocity, sizeof(Vector3D));

                // Scatter read requests for Glow
                uint64_t glowEnableAddress = player->BasePointer + OFF_GLOW_ENABLE;
                uint64_t glowThroughWallAddress = player->BasePointer + OFF_GLOW_THROUGH_WALL;
                uint64_t highlightIDAddress = player->BasePointer + OFF_GLOW_HIGHLIGHT_ID + 1;
                mem.AddScatterReadRequest(handle, glowEnableAddress, &player->GlowEnable, sizeof(int));
                mem.AddScatterReadRequest(handle, glowThroughWallAddress, &player->GlowThroughWall, sizeof(int));
                mem.AddScatterReadRequest(handle, highlightIDAddress, &player->HighlightID, sizeof(int));

                // Scatter read request for Visibility
                uint64_t lastTimeAimedAtAddress = player->BasePointer + OFF_LAST_AIMEDAT_TIME;
                uint64_t lastVisibleTimeAddress = player->BasePointer + OFF_LAST_VISIBLE_TIME;
                mem.AddScatterReadRequest(handle, lastTimeAimedAtAddress, &player->LastTimeAimedAt, sizeof(int));
                mem.AddScatterReadRequest(handle, lastVisibleTimeAddress, &player->LastVisibleTime, sizeof(int));

                // Scatter read request for Yaw
                uint64_t viewYawAddress = player->BasePointer + OFF_YAW;
                mem.AddScatterReadRequest(handle, viewYawAddress, &player->ViewYaw, sizeof(float));
            }
            else {
                player->IsDead = false;
                player->IsKnocked = false;
            }

            if (player->IsDummy() && Map->IsFiringRange || player->IsPlayer()) {
                // Scatter read request for LocalOrigin
                uint64_t localOriginAddress = player->BasePointer + OFF_LOCAL_ORIGIN;
                mem.AddScatterReadRequest(handle, localOriginAddress, &player->LocalOrigin, sizeof(Vector3D));

                // Scatter read requests for Health
                uint64_t healthAddress = player->BasePointer + OFF_HEALTH;
                mem.AddScatterReadRequest(handle, healthAddress, &player->Health, sizeof(int));

                // Scatter read request for ModelPointer
                uint64_t modelPointerAddress = player->BasePointer + OFF_STUDIOHDR;
                mem.AddScatterReadRequest(handle, modelPointerAddress, &player->ModelPointer, sizeof(uint64_t));

				// Scatter read request for BonePtr
				uint64_t bonePointerAddress = player->BasePointer + OFF_BONES;
				mem.AddScatterReadRequest(handle, bonePointerAddress, &player->BonePointer, sizeof(uint64_t));
            }
        }
    }

    // Execute the scatter read
    mem.ExecuteReadScatter(handle);

    // Close the scatter handle
    mem.CloseScatterHandle(handle);
}

// Core
std::chrono::microseconds TotalProfilingElapsed;
std::chrono::microseconds LocalPlayerProfilingElapsed;
std::chrono::microseconds PlayerPopulateProfilingElapsed;
std::chrono::microseconds PlayerAttributesProfilingElapsed;
std::chrono::microseconds GameCameraProfilingElapsed;
std::chrono::microseconds ESPProfilingElapsed;
std::chrono::microseconds AimAssistProfilingElapsed;
std::chrono::microseconds ReadPlayersProfilingElapsed;
void UpdateCore() {
    static bool PlayersPopulated = false;
    try {
        while (true) {
            // Initial Misc Reads //
            MiscBaseScatter(Map, Myself, GameCamera, ESP);

            // Map Checking //
            Map->Read();
            if (!Map->IsPlayable) {
                Players->clear();
                PlayersPopulated = false;
                continue;
            }

            // Local Player Update //
            Myself->Read();

            if (!Myself->IsValid()) {
                Players->clear();
                PlayersPopulated = false;
                continue;
            }


            if (!Myself->ValidPosition()) {
				Players->clear();
                PlayersPopulated = false;
				continue;
			}

            // Populate Players //

            // Dummy Fix
            if (Map->IsFiringRange) {
                static auto lastExecTime = std::chrono::steady_clock::now() - std::chrono::seconds(5); // Subtract to ensure it runs the first time

                // Check if 5 seconds have passed since the last execution
                auto currentTime = std::chrono::steady_clock::now();
                if (currentTime - lastExecTime >= std::chrono::seconds(5)) {
                    PlayersPopulated = false;
                    Players->clear();
                    lastExecTime = currentTime;
                }
            }

            if (!PlayersPopulated) {
                // Call the scatter function to read BasePointers, Team and Name for all players
                if (Map->IsFiringRange) {
                    PlayerBasePointerScatter(*Dummies);
                    ScatterReadTeamAndName(*Dummies);
                }
                else {
                    PlayerBasePointerScatter(*HumanPlayers);
                    ScatterReadTeamAndName(*HumanPlayers);
                }

                if (Map->IsFiringRange) {
                    for (int i = 0; i < Dummies->size(); i++) {
                        Player* p = Dummies->at(i);
                        if (p->BasePointer != 0 && (p->IsPlayer() || p->IsDummy()))
                            Players->push_back(p);
                    }
                }
                else {
                    for (int i = 0; i < HumanPlayers->size(); i++) {
                        Player* p = HumanPlayers->at(i);
                        if (p->BasePointer != 0 && (p->IsPlayer() || p->IsDummy()))
                            Players->push_back(p);
                    }
                }
                std::cout << "Players Populated" << std::endl;
                PlayersPopulated = true;
            }

            // Updates //
            // Update Player Validity
            ScatterReadPlayerValidity(*Players);

            // Update Player Attributes
            ScatterReadPlayerAttributes(*Players);

            // Update Camera
            GameCamera->Update();

            // Update Players
            for (int i = 0; i < Players->size(); i++) {
                Player* p = Players->at(i);
                p->Read();
            }

            // Update ESP
            ESP->Update();

            // Update AimAssist
            AimAssist->Update_Aimbot();
            AimAssist->Update_Triggerbot();
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
        return;
    }
}

void Profiling() {
	while (true) {
		std::cout << "---- Profiling ----" << std::endl;
		std::cout << "LocalPlayer: " << LocalPlayerProfilingElapsed.count() << "us" << std::endl;
		std::cout << "PlayerPopulate: " << PlayerPopulateProfilingElapsed.count() << "us" << std::endl;
		std::cout << "PlayerAttributes: " << PlayerAttributesProfilingElapsed.count() << "us" << std::endl;
        std::cout << "ReadPlayers: " << ReadPlayersProfilingElapsed.count() << "us" << std::endl;
		std::cout << "GameCamera: " << GameCameraProfilingElapsed.count() << "us" << std::endl;
		std::cout << "ESP: " << ESPProfilingElapsed.count() << "us" << std::endl;
		std::cout << "AimAssist: " << AimAssistProfilingElapsed.count() << "us" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

int main()
{
    std::cout << "-----------------------------" << std::endl;
	std::cout << "Apex Legends DMA Paste" << std::endl;
	std::cout << "-----------------------------" << std::endl;

	// Initialize DMA
	if (!mem.Init("r5apex.exe", true, false))
	{
		std::cout << "Failed to initilize DMA" << std::endl;
        std::cout << "Press ENTER to continue...";
        std::cin.get();
	}
    std::cout << "DMA initilized" << std::endl;

    if (!mem.GetKeyboard()->InitKeyboard())
    {
        std::cout << "Failed to initialize keyboard hotkeys through kernel." << std::endl;
        std::cout << "Press ENTER to continue...";
        std::cin.get();
    }

    try {
        for (int i = 0; i < 70; i++)
            HumanPlayers->push_back(new Player(i, Myself, Map));

        for (int i = 0; i < 15000; i++)
            Dummies->push_back(new Player(i, Myself, Map));

        std::cout << "-----------------------------" << std::endl;
        std::locale::global(std::locale("C"));
        Config config("config.cfg", AimAssist, GameCamera);
        config.Update();
        std::cout << "Config loaded" << std::endl;
        std::cout << "-----------------------------" << std::endl;
        GameCamera->Initialize();
        ESP->Initialize();
        AimAssist->Initialize();
        std::cout << "-----------------------------" << std::endl;
        std::cout << "Core initialized" << std::endl;
        std::cout << "-----------------------------" << std::endl;

        // Threads
        std::thread coreThread(UpdateCore);
        //std::thread renderThread(Render, Myself, Players, GameCamera, Spectators, AimAssist);
        //std::thread profilingThread(Profiling);
        coreThread.join();
    }
    catch (...) {}

    std::cout << "Press ENTER to exit...";
    std::cin.get();  // Wait for user to press Enter
    return 0;
}