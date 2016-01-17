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
// NPC: Construction Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00001 / Date: 16.01.2016
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"monster_zombie_construction.h"

//=========================================================
// Monster's link to Class
//=========================================================
LINK_ENTITY_TO_CLASS(monster_zombie_construction, CZombieConstruction);

//=========================================================
// Spawn
//=========================================================
void CZombieConstruction::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/zombie_construction.mdl");

	if (pev->health == 0)
		pev->health = gSkillData.zombieConstructionHealth;

	m_flHitgroupHead = gSkillData.zombieConstructionHead;
	m_flHitgroupChest = gSkillData.zombieConstructionChest;
	m_flHitgroupStomach = gSkillData.zombieConstructionStomach;
	m_flHitgroupArm = gSkillData.zombieConstructionArm;
	m_flHitgroupLeg = gSkillData.zombieConstructionLeg;
	m_flDmgOneSlash = gSkillData.zombieConstructionDmgOneSlash;
	m_flDmgBothSlash = gSkillData.zombieConstructionDmgBothSlash;

	m_flDebug = false; //Debug Massages

	CZombie::Spawn();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CZombieConstruction::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/zombie_construction.mdl");

	CZombie::Precache();
}
