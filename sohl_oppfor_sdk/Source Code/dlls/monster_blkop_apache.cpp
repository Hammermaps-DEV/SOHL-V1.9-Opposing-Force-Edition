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
// NPC: AH-64 Apache * http://half-life.wikia.com/wiki/AH-64_Apache
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "particle_defs.h"
#include "monster_apache.h"
#include "monster_blkop_apache.h"

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS(monster_blkop_apache, CBlackOPApache);

#define SF_WAITFORTRIGGER	(0x04 | 0x40) // UNDONE: Fix!
#define SF_NOWRECKAGE		0x08

//=========================================================
// Spawn Apache
//=========================================================
void CBlackOPApache::Spawn(void) {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/blkop_apache.mdl");

	UTIL_SetSize(pev, Vector(-32, -32, -64), Vector(32, 32, 0));
	UTIL_SetOrigin(this, pev->origin);

	// Motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	pev->flags |= FL_MONSTER;
	pev->takedamage = DAMAGE_AIM;

	if (pev->health == 0)
		pev->health = gSkillData.apacheBlkopHealth;

	m_flFieldOfView = -0.707; // 270 degrees

	// Change Rotor Sound
	m_iRotorSound = 1;
	if (RANDOM_LONG(0, 100) >= 60) {
		m_iRotorSound = 3;
	}

	pev->sequence = 0;
	ResetSequenceInfo();
	pev->frame = RANDOM_LONG(0, 0xFF);

	InitBoneControllers();

	if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_32) {
		SetUse(&CApache::StartupUse);
	} else {
		SetThink(&CApache::HuntThink);
		SetTouch(&CApache::FlyTouch);
		SetNextThink(1.0);
	}

	m_iRockets = 10;
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CBlackOPApache::Classify(void) {
	return m_iClass ? m_iClass : CLASS_HUMAN_MILITARY;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CBlackOPApache::Precache(void) {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/blkop_apache.mdl");

	CApache::Precache();
}