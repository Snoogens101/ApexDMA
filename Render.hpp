#pragma once
#include "LocalPlayer.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include "Spectator.hpp"
#include "Aimbot.hpp"
#include "Glow.hpp"

void Render(LocalPlayer* Myself, std::vector<Player*>* Players, Camera* GameCamera, Spectator* Spectators, Aimbot* AimAssist, Sense* ESP);