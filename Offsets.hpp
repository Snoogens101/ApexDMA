#pragma once

// Core
constexpr uint64_t OFF_LEVEL = 0x16d7de0;                         //[Miscellaneous]->LevelName
constexpr uint64_t OFF_LOCAL_PLAYER = 0x2162818;                  //[Miscellaneous]->LocalPlayer
constexpr uint64_t OFF_ENTITY_LIST = 0x1db4118;                   //[Miscellaneous]->cl_entitylist

constexpr uint64_t OFF_NAME_LIST = 0xc5eb2d0;                     //[Miscellaneous]->NameList
constexpr uint64_t OFF_NAME_INDEX = 0x38;                         //nameIndex

// HUD
constexpr uint64_t OFF_VIEWRENDER = 0x73cd6e0;                    //[Miscellaneous]->ViewRenderer
constexpr uint64_t OFF_VIEWMATRIX = 0x11a350;                     //[Miscellaneous]->ViewMatrix

// Player
constexpr uint64_t OFF_HEALTH = 0x02fc;                           //[RecvTable.DT_Player]->m_iHealth
constexpr uint64_t OFF_MAXHEALTH = 0x0438;                        //[RecvTable.DT_Player]->m_iMaxHealth
constexpr uint64_t OFF_SHIELD = 0x01a0;                           //[RecvTable.DT_TitanSoul]->m_shieldHealth
constexpr uint64_t OFF_MAXSHIELD = 0x01a4;                        //[RecvTable.DT_TitanSoul]->m_shieldHealthMax

constexpr uint64_t OFF_INATTACK = 0x073ce9a0;                     //[Buttons]->in_attack

constexpr uint64_t OFF_CAMERAORIGIN = 0x1e90;                     //[Miscellaneous]->CPlayer!camera_origin
constexpr uint64_t OFF_STUDIOHDR = 0xfb0;                        //[Miscellaneous]->CBaseAnimating!m_pStudioHdr
constexpr uint64_t OFF_BONES = 0x0d60 + 0x48;                     //m_nForceBone

constexpr uint64_t OFF_LOCAL_ORIGIN = 0x017c;                     //[DataMap.C_BaseEntity]->m_vecAbsOrigin
constexpr uint64_t OFF_ABSVELOCITY = 0x0170;                      //[DataMap.C_BaseEntity]->m_vecAbsVelocity

constexpr uint64_t OFF_ZOOMING = 0x1b91;                          //[RecvTable.DT_Player]->m_bZooming
constexpr uint64_t OFF_TEAM_NUMBER = 0x030c;                      //[RecvTable.DT_BaseEntity]->m_iTeamNum
constexpr uint64_t OFF_NAME = 0x0449;                             //[RecvTable.DT_BaseEntity]->m_iName
constexpr uint64_t OFF_LIFE_STATE = 0x0658;                       //[RecvTable.DT_Player]->m_lifeState
constexpr uint64_t OFF_BLEEDOUT_STATE = 0x26a0;                   //[RecvTable.DT_Player]->m_bleedoutState  
constexpr uint64_t OFF_LAST_VISIBLE_TIME = 0x194d + 0x3;          //[RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x3
constexpr uint64_t OFF_LAST_AIMEDAT_TIME = 0x194d + 0x3 + 0x8;    //[RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x3 + 0x8
constexpr uint64_t OFF_VIEW_ANGLES = 0x24f4 - 0x14;               //[DataMap.C_Player]-> m_ammoPoolCapacity - 0x14
constexpr uint64_t OFF_PUNCH_ANGLES = 0x23f8;                     //[DataMap.C_Player]->m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
constexpr uint64_t OFF_YAW = 0x21fc - 0x8;                        //m_currentFramePlayer.m_ammoPoolCount - 0x8

// Weapon 
constexpr uint64_t OFF_WEAPON_HANDLE = 0x18f4;                    //[RecvTable.DT_Player]->m_latestPrimaryWeapons
constexpr uint64_t OFF_WEAPON_INDEX = 0x1738;                     //[RecvTable.DT_WeaponX]->m_weaponNameIndex
constexpr uint64_t OFF_PROJECTILESCALE = 0x04ec + 0x1978;         //projectile_gravity_scale + [WeaponSettingsMeta]base
constexpr uint64_t OFF_PROJECTILESPEED = 0x04e4 + 0x1978;         //projectile_launch_speed + [WeaponSettingsMeta]base
constexpr uint64_t OFF_OFFHAND_WEAPON = 0x1904;                   //m_latestNonOffhandWeapons
constexpr uint64_t OFF_CURRENTZOOMFOV = 0x1590 + 0x00b8;          //m_playerData + m_curZoomFOV
constexpr uint64_t OFF_TARGETZOOMFOV = 0x1590 + 0x00bc;           //m_playerData + m_targetZoomFOV

// Glow
constexpr uint64_t OFF_GLOW_ENABLE = 0x28C;                       //Script_Highlight_GetCurrentContext
constexpr uint64_t OFF_GLOW_THROUGH_WALL = 0x26C;                 //Script_Highlight_SetVisibilityType
constexpr uint64_t OFF_GLOW_FIX = 0x268;
constexpr uint64_t OFF_GLOW_HIGHLIGHT_ID = 0x28C;                 //[DT_HighlightSettings].m_highlightServerActiveStates    
constexpr uint64_t OFF_GLOW_HIGHLIGHTS = 0xB93F2D0;               //HighlightSettings

// Misc
constexpr long OFF_TIME_BASE = 0x2048;                        //m_currentFramePlayer.timeBase
constexpr long OFFSET_TRAVERSAL_START_TIME = 0x2ac0;          //[RecvTable.DT_LocalPlayerExclusive]->m_traversalStartTime
constexpr long OFFSET_TRAVERSAL_PROGRESS = 0x2abc;            //[RecvTable.DT_LocalPlayerExclusive]->m_traversalProgress