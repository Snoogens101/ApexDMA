#include <iostream>
#include "DMALibrary/Memory/Memory.h"

#include "Level.hpp"
#include "Player.hpp"
#include "LocalPlayer.hpp"
#include "Camera.hpp"

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

// Core
bool UpdateCore() {
    try {
        // Map Checking //
        Map->Read();
        std::cout << "Map: " << Map->Name << std::endl;
        if (!Map->IsPlayable) {
            return true;
        }
        std::cout << "Map is playable" << std::endl;

        // Read Local Player //
        Myself->Read();
        if (!Myself->IsValid()) {
            return true;
        }
        std::cout << "Local Player is valid" << std::endl;

        // Populate Players //
        Players->clear();
        std::cout << "Map is firing range: " << Map->IsFiringRange << std::endl;
        if (Map->IsFiringRange) {
            for (int i = 0; i < Dummies->size(); i++) {
                Player* p = Dummies->at(i);
                p->Read();
                if (p->BasePointer != 0 && (p->IsPlayer() || p->IsDummy()))
                    Players->push_back(p);
            }
        }
        else {
            for (int i = 0; i < HumanPlayers->size(); i++) {
                Player* p = HumanPlayers->at(i);
                p->Read();
                if (p->BasePointer != 0 && (p->IsPlayer() || p->IsDummy()))
                    Players->push_back(p);
            }
        }

        // Updates //
        GameCamera->Update();
        //ESP->Update();
        //AimAssist->Update();
        //Trigger->Update();

        return true;
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

    for (int i = 0; i < 15000; i++)
        Dummies->push_back(new Player(i, Myself));

    UpdateCore();
}