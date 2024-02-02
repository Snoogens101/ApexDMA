#include <iostream>
#include "DMALibrary/Memory/Memory.h"

#include "Level.hpp"
#include "Player.hpp"
#include "LocalPlayer.hpp"
#include "Camera.hpp"
#include "Glow.hpp"
#include <thread>
#include <unordered_set>

// Base Address
uintptr_t OFF_BASE = 0x0;

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

void MiscBaseScatter(Level* map, LocalPlayer* myself, Camera* gameCamera, Sense* esp) {
	// Create scatter handle
	auto handle = mem.CreateScatterHandle();

	// Scatter read request for Level
	uint64_t levelAddress = OFF_BASE + OFF_LEVEL;
	mem.AddScatterReadRequest(handle, levelAddress, &map->NameBuffer, sizeof(map->NameBuffer));

	// Scatter read request for LocalPlayer BasePointer
	uint64_t localPlayerAddress = OFF_BASE + OFF_LOCAL_PLAYER;
	mem.AddScatterReadRequest(handle, localPlayerAddress, &myself->BasePointer, sizeof(uint64_t));

	// Scatter read request for LocalPlayer inAttack
	uint64_t inAttackAddress = OFF_BASE + OFF_INATTACK;
	mem.AddScatterReadRequest(handle, inAttackAddress, &myself->IsInAttack, sizeof(bool));

	// Scatter read request for GameCamera
	uint64_t cameraRenderPointerAddress = OFF_BASE + OFF_VIEWRENDER;
	mem.AddScatterReadRequest(handle, cameraRenderPointerAddress, &gameCamera->RenderPointer, sizeof(uint64_t));

    // Scatter read request for HighlightSettingsPointer
    uint64_t highlightSettingsPointerAddress = OFF_BASE + OFF_GLOW_HIGHLIGHTS;
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
        uint64_t address = OFF_BASE + OFF_ENTITY_LIST + ((index + 1) << 5);
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
        if (player->BasePointer != 0) {
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

void ScatterReadPlayerAttributes(std::vector<Player*>& players) {
    // Create scatter handle
    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < players.size(); ++i) {
        Player* player = players[i];

        // Verify that the BasePointer is not 0 before adding scatter read requests
        if (player->BasePointer != 0) {
            if (!player->IsDummy()) {
                // Scatter read request for IsDead
                uint64_t isDeadAddress = player->BasePointer + OFF_LIFE_STATE;
                mem.AddScatterReadRequest(handle, isDeadAddress, &player->IsDead, sizeof(bool));

                // Scatter read request for IsKnocked
                uint64_t isKnockedAddress = player->BasePointer + OFF_BLEEDOUT_STATE;
                mem.AddScatterReadRequest(handle, isKnockedAddress, &player->IsKnocked, sizeof(bool));
            }
            else {
                player->IsDead = false;
                player->IsKnocked = false;
            }

            // Scatter read request for LocalOrigin
            uint64_t localOriginAddress = player->BasePointer + OFF_LOCAL_ORIGIN;
            mem.AddScatterReadRequest(handle, localOriginAddress, &player->LocalOrigin, sizeof(Vector3D));

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

            // Scatter read requests for Health and Shield
            uint64_t healthAddress = player->BasePointer + OFF_HEALTH;
            uint64_t maxHealthAddress = player->BasePointer + OFF_MAXHEALTH;
            uint64_t shieldAddress = player->BasePointer + OFF_SHIELD;
            uint64_t maxShieldAddress = player->BasePointer + OFF_MAXSHIELD;
            mem.AddScatterReadRequest(handle, healthAddress, &player->Health, sizeof(int));
            mem.AddScatterReadRequest(handle, maxHealthAddress, &player->MaxHealth, sizeof(int));
            mem.AddScatterReadRequest(handle, shieldAddress, &player->Shield, sizeof(int));
            mem.AddScatterReadRequest(handle, maxShieldAddress, &player->MaxShield, sizeof(int));

            if (!player->IsDummy() || player->IsPlayer()) {
				// Scatter read request for ViewYaw
                uint64_t viewYawAddress = player->BasePointer + OFF_YAW;
				mem.AddScatterReadRequest(handle, viewYawAddress, &player->ViewYaw, sizeof(float));
			}

            if (player->IsDummy() || player->IsPlayer()) {
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
bool UpdateCore() {
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
                //std::cout << "Map not playable" << std::endl;
                continue;
            }

            // Read Local Player //
            Myself->Read();
            if (!Myself->IsValid()) {
                Players->clear();
                PlayersPopulated = false;
                //std::cout << "Invalid Local Player" << std::endl;
                continue;
            }

            if (!Myself->ValidPosition()) {
				Players->clear();
                PlayersPopulated = false;
                //std::cout << "Invalid Position" << std::endl;
				continue;
			}

            // Call the scatter function to read BasePointers, Team and Name for all players
            if (Map->IsFiringRange){
				PlayerBasePointerScatter(*Dummies);
                ScatterReadTeamAndName(*Dummies);
			}
			else {
				PlayerBasePointerScatter(*HumanPlayers);
                ScatterReadTeamAndName(*HumanPlayers);
			}

            // Populate Players //

            if (!PlayersPopulated) {
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

            // Call the scatter function to read all player attributes
            ScatterReadPlayerAttributes(*Players);

            // Update Players //
            for (int i = 0; i < Players->size(); i++) {
				Player* p = Players->at(i);
				p->Read();
                //std::cout << p->Index << ": " << p->BasePointer << " | " << p->IsValid() << std::endl;
			}

            // Updates //
            GameCamera->Update();
            ESP->Update();
        }
    }
    catch (const std::exception& ex) {
        std::system("clear");
        std::cout << "Error: " << ex.what() << std::endl;
        return true;
    }

    return false;
}


int main()
{
	if (!mem.Init("r5apex.exe", true, false))
	{
		std::cout << "Failed to initilize DMA" << std::endl;
		return 1;
	}
    std::cout << "DMA initilized" << std::endl;
    OFF_BASE = mem.GetBaseDaddy("r5apex.exe");

    try {
        for (int i = 0; i < 70; i++)
            HumanPlayers->push_back(new Player(i, Myself));

        for (int i = 0; i < 15000; i++)
            Dummies->push_back(new Player(i, Myself));

        ESP->Initialize();

        std::cout << "Core initialized" << std::endl;
        std::cout << "-----------------------------" << std::endl;

        // Create a thread to run UpdateCore
        std::thread coreThread(UpdateCore);

        // Wait for the UpdateCore thread to finish
        coreThread.join();
    }
    catch (...) {}
}