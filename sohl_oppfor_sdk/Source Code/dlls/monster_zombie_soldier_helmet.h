/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
//=========================================================
// NPC: Soldier Helmet Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_ZOMBIE_SOLDIER_HELMET_H
#define MONSTER_ZOMBIE_SOLDIER_HELMET_H

// include
#include "monster_zombie_soldier.h"

// class definition
class CZombieSoldierHelmet : public CZombieSoldier {
public:
	// void
	void Spawn(void);
	void Precache(void);
};

#endif // MONSTER_ZOMBIE_SOLDIER_HELMET_H