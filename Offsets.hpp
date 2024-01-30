#pragma once

// Core
extern uintptr_t OFF_BASE;
constexpr long long OFF_LEVEL = 0x16d6ac0;                         //[Miscellaneous]->LevelName
constexpr long long OFF_LOCAL_PLAYER = 0x2161538;                  //[Miscellaneous]->LocalPlayer
constexpr long long OFF_ENTITY_LIST = 0x1db2e38;                   //[Miscellaneous]->cl_entitylist

constexpr long long OFF_NAME_LIST = 0xc5e9fd0;                     //[Miscellaneous]->NameList
constexpr long long OFF_NAME_INDEX = 0x38;                         //nameIndex

// HUD
constexpr long long OFF_VIEWRENDER = 0x73cc3e0;                    //[Miscellaneous]->ViewRenderer
constexpr long long OFF_VIEWMATRIX = 0x11a350;                     //[Miscellaneous]->ViewMatrix

// Player
constexpr long long OFF_HEALTH = 0x02fc;                           //[RecvTable.DT_Player]->m_iHealth
constexpr long long OFF_MAXHEALTH = 0x0438;                        //[RecvTable.DT_Player]->m_iMaxHealth
constexpr long long OFF_SHIELD = 0x01a0;                           //[RecvTable.DT_TitanSoul]->m_shieldHealth
constexpr long long OFF_MAXSHIELD = 0x01a4;                        //[RecvTable.DT_TitanSoul]->m_shieldHealthMax

constexpr long long OFF_INATTACK = 0x073cd6a0;                     //[Buttons]->in_attack

constexpr long long OFF_CAMERAORIGIN = 0x1e90;                     //[Miscellaneous]->CPlayer!camera_origin
constexpr long long OFF_STUDIOHDR = 0xfb0;                        //[Miscellaneous]->CBaseAnimating!m_pStudioHdr
constexpr long long OFF_BONES = 0x0d60 + 0x48;                     //m_nForceBone

constexpr long long OFF_LOCAL_ORIGIN = 0x017c;                     //[DataMap.C_BaseEntity]->m_vecAbsOrigin
constexpr long long OFF_ABSVELOCITY = 0x0170;                      //[DataMap.C_BaseEntity]->m_vecAbsVelocity

constexpr long long OFF_ZOOMING = 0x1b91;                          //[RecvTable.DT_Player]->m_bZooming
constexpr long long OFF_TEAM_NUMBER = 0x030c;                      //[RecvTable.DT_BaseEntity]->m_iTeamNum
constexpr long long OFF_NAME = 0x0449;                             //[RecvTable.DT_BaseEntity]->m_iName
constexpr long long OFF_LIFE_STATE = 0x0658;                       //[RecvTable.DT_Player]->m_lifeState
constexpr long long OFF_BLEEDOUT_STATE = 0x26a0;                   //[RecvTable.DT_Player]->m_bleedoutState  
constexpr long long OFF_LAST_VISIBLE_TIME = 0x194d + 0x3;          //[RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x3
constexpr long long OFF_LAST_AIMEDAT_TIME = 0x194d + 0x3 + 0x8;    //[RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x3 + 0x8
constexpr long long OFF_VIEW_ANGLES = 0x24f4 - 0x14;               //[DataMap.C_Player]-> m_ammoPoolCapacity - 0x14
constexpr long long OFF_PUNCH_ANGLES = 0x23f8;                     //[DataMap.C_Player]->m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
constexpr long long OFF_YAW = 0x21fc - 0x8;                        //m_currentFramePlayer.m_ammoPoolCount - 0x8

// Weapon 
constexpr long long OFF_WEAPON_HANDLE = 0x18f4;                    //[RecvTable.DT_Player]->m_latestPrimaryWeapons
constexpr long long OFF_WEAPON_INDEX = 0x1738;                     //[RecvTable.DT_WeaponX]->m_weaponNameIndex
constexpr long long OFF_PROJECTILESCALE = 0x04ec + 0x1978;         //projectile_gravity_scale + [WeaponSettingsMeta]base
constexpr long long OFF_PROJECTILESPEED = 0x04e4 + 0x1978;         //projectile_launch_speed + [WeaponSettingsMeta]base
constexpr long long OFF_OFFHAND_WEAPON = 0x1904;                   //m_latestNonOffhandWeapons
constexpr long long OFF_CURRENTZOOMFOV = 0x1590 + 0x00b8;          //m_playerData + m_curZoomFOV
constexpr long long OFF_TARGETZOOMFOV = 0x1590 + 0x00bc;           //m_playerData + m_targetZoomFOV

// Glow
constexpr long long OFF_GLOW_ENABLE = 0x28C;                       //Script_Highlight_GetCurrentContext
constexpr long long OFF_GLOW_THROUGH_WALL = 0x26C;                 //Script_Highlight_SetVisibilityType
constexpr long long OFF_GLOW_FIX = 0x268;
constexpr long long OFF_GLOW_HIGHLIGHT_ID = 0x28C;                 //[DT_HighlightSettings].m_highlightServerActiveStates    
constexpr long long OFF_GLOW_HIGHLIGHTS = 0xB93DFD0;               //HighlightSettings