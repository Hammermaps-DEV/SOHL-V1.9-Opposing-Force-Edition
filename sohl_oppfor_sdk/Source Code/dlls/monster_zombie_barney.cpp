/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Spirit of Half-Life and their logos are the property of their respective owners.
*   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*   This product contains software technology licensed from Id
*   Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
*   All Rights Reserved.
*
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/
//=========================================================
// NPC: Barney Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00002 / Date: 18.10.2015
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"monster_zombie_barney.h"

//=========================================================
// Monster's link to Class
//=========================================================
LINK_ENTITY_TO_CLASS(monster_zombie_barney, CZombieBarney);

//=========================================================
// Spawn
//=========================================================
void CZombieBarney::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/zombie_barney.mdl");

	if (pev->health == 0)
		pev->health = gSkillData.zombieBarneyHealth;

	m_flHitgroupHead    = gSkillData.zombieBarneyHead;
	m_flHitgroupChest   = gSkillData.zombieBarneyChest;
	m_flHitgroupStomach = gSkillData.zombieBarneyStomach;
	m_flHitgroupArm     = gSkillData.zombieBarneyArm;
	m_flHitgroupLeg     = gSkillData.zombieBarneyLeg;
	m_flDmgOneSlash     = gSkillData.zombieBarneyDmgOneSlash;
	m_flDmgBothSlash    = gSkillData.zombieBarneyDmgBothSlash;

	m_flDebug = false; //Debug Massages

	CZombie::Spawn();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CZombieBarney::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/zombie_barney.mdl");

	CZombie::Precache();
}
