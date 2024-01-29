#pragma once

// Core    
constexpr long OFF_REGION = 0x140000000;                      //[Static]->Region
constexpr long OFF_LEVEL = 0x16966f0;                         //[Miscellaneous]->LevelName
constexpr long OFF_LOCAL_PLAYER = 0x211fac8;                  //[Miscellaneous]->LocalPlayer
constexpr long OFF_ENTITY_LIST = 0x1d71858;                   //[Miscellaneous]->cl_entitylist

constexpr long OFF_NAME_LIST = 0xc275bf0;                     //[Miscellaneous]->NameList
constexpr long OFF_NAME_INDEX = 0x38;                         //nameIndex

// HUD
constexpr long OFF_VIEWRENDER = 0x7389d40;                    //[Miscellaneous]->ViewRenderer
constexpr long OFF_VIEWMATRIX = 0x11a350;                     //[Miscellaneous]->ViewMatrix

// Player
constexpr long OFF_HEALTH = 0x036c;                           //[RecvTable.DT_Player]->m_iHealth
constexpr long OFF_MAXHEALTH = 0x04a8;                        //[RecvTable.DT_Player]->m_iMaxHealth
constexpr long OFF_SHIELD = 0x01a0;                           //[RecvTable.DT_TitanSoul]->m_shieldHealth
constexpr long OFF_MAXSHIELD = 0x01a4;                        //[RecvTable.DT_TitanSoul]->m_shieldHealthMax

constexpr long OFF_INATTACK = 0x0738af90;                     //[Buttons]->in_attack

constexpr long OFF_CAMERAORIGIN = 0x1f00;                     //[Miscellaneous]->CPlayer!camera_origin
constexpr long OFF_STUDIOHDR = 0x1020;                        //[Miscellaneous]->CBaseAnimating!m_pStudioHdr
constexpr long OFF_BONES = 0x0dd0 + 0x48;                     //m_nForceBone

constexpr long OFF_LOCAL_ORIGIN = 0x017c;                     //[DataMap.C_BaseEntity]->m_vecAbsOrigin
constexpr long OFF_ABSVELOCITY = 0x0170;                      //[DataMap.C_BaseEntity]->m_vecAbsVelocity

constexpr long OFF_ZOOMING = 0x1c01;                          //[RecvTable.DT_Player]->m_bZooming
constexpr long OFF_TEAM_NUMBER = 0x037c;                      //[RecvTable.DT_BaseEntity]->m_iTeamNum
constexpr long OFF_NAME = 0x04b9;                             //[RecvTable.DT_BaseEntity]->m_iName
constexpr long OFF_LIFE_STATE = 0x06c8;                       //[RecvTable.DT_Player]->m_lifeState
constexpr long OFF_BLEEDOUT_STATE = 0x2710;                   //[RecvTable.DT_Player]->m_bleedoutState  
constexpr long OFF_LAST_VISIBLE_TIME = 0x19bd + 0x3;          //[RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x3
constexpr long OFF_LAST_AIMEDAT_TIME = 0x19bd + 0x3 + 0x8;    //[RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x3 + 0x8
constexpr long OFF_VIEW_ANGLES = 0x2564 - 0x14;               //[DataMap.C_Player]-> m_ammoPoolCapacity - 0x14
constexpr long OFF_PUNCH_ANGLES = 0x2468;                     //[DataMap.C_Player]->m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
constexpr long OFF_YAW = 0x226c - 0x8;                        //m_currentFramePlayer.m_ammoPoolCount - 0x8

// Weapon 
constexpr long OFF_WEAPON_HANDLE = 0x1964;                    //[RecvTable.DT_Player]->m_latestPrimaryWeapons
constexpr long OFF_WEAPON_INDEX = 0x17a8;                     //[RecvTable.DT_WeaponX]->m_weaponNameIndex
constexpr long OFF_PROJECTILESCALE = 0x04e4 + 0x19e0;         //projectile_gravity_scale + [WeaponSettingsMeta]base
constexpr long OFF_PROJECTILESPEED = 0x04dc + 0x19e0;         //projectile_launch_speed + [WeaponSettingsMeta]base
constexpr long OFF_OFFHAND_WEAPON = 0x1974;                   //m_latestNonOffhandWeapons
constexpr long OFF_CURRENTZOOMFOV = 0x1600 + 0x00b8;          //m_playerData + m_curZoomFOV
constexpr long OFF_TARGETZOOMFOV = 0x1600 + 0x00bc;           //m_playerData + m_targetZoomFOV

// Glow
constexpr long OFF_GLOW_ENABLE = 0x294;                       //Script_Highlight_GetCurrentContext
constexpr long OFF_GLOW_THROUGH_WALL = 0x278;                 //Script_Highlight_SetVisibilityType
constexpr long OFF_GLOW_FIX = 0x270;
constexpr long OFF_GLOW_HIGHLIGHT_ID = 0x298;                 //[DT_HighlightSettings].m_highlightServerActiveStates    
constexpr long OFF_GLOW_HIGHLIGHTS = 0xb5cc530;               //HighlightSettings