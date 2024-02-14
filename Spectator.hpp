#pragma once
#include <iostream>
#include <vector>
#include <array>
#include "Player.hpp"
#include "LocalPlayer.hpp"


struct Spectator {
    // Variables
    LocalPlayer* Myself;
    std::vector<Player*>* Players;
    int TotalSpectators = 0;
    std::vector<std::string> Spectators;
    std::chrono::milliseconds LastUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    Spectator(std::vector<Player*>* Players, LocalPlayer* Myself) {
        this->Players = Players;
        this->Myself = Myself;
    }

    void Update() {
        std::chrono::milliseconds Now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        if (Now >= LastUpdateTime + std::chrono::milliseconds(1500))
        {
            int TempTotalSpectators = 0;
            std::vector<std::string> TempSpectators;
            for (int i = 0; i < Players->size(); i++)
            {
                Player* p = Players->at(i);
                if (!mem.IsValidPointer(p->BasePointer))
					continue;
                if (p->BasePointer == Myself->BasePointer)
                    continue;
                if (!p->IsPlayer())
					continue;
                if (p->ViewYaw == 0 || Myself->ViewYaw == 0)
                    continue;
                if (std::fabs(p->ViewYaw - Myself->ViewYaw) < 0.1f && p->IsDead)
                {
                    std::cout << "Spec: " << p->ViewYaw << ", Play: " << Myself->ViewYaw << std::endl;
                    TempTotalSpectators++;
                }
            }

            TotalSpectators = TempTotalSpectators;
            Spectators = TempSpectators;
            LastUpdateTime = Now;
        }
    }


};