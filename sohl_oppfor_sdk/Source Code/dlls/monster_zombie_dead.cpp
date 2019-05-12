/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Half-Life and their logos are the property of their respective owners.
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
*	Spirit of Half-Life, by Laurie R. Cheers. (LRC)
*   Modified by Lucas Brucksch (Code merge & Effects)
*   Modified by Andrew J Hamilton (AJH)
*   Modified by XashXT Group (g-cont...)
*
*   Code used from Battle Grounds Team and Contributors.
*   Code used from SamVanheer (Opposing Force code)
*   Code used from FWGS Team (Fixes for SOHL)
*   Code used from LevShisterov (Bugfixed and improved HLSDK)
*	Code used from Fograin (Half-Life: Update MOD)
*
***/
//=========================================================
// PROP: DEAD Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include    "monster_zombie_dead.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
char *CDeadZombie::m_szPoses[] = { "dead_on_back", "dead_on_stomach" };

//=========================================================
// Monster's link to Class
//=========================================================
LINK_ENTITY_TO_CLASS(monster_zombie_dead, CDeadZombie);

//=========================================================
// Read options from bsp * put in .fgd file as option *
// [pose] = dead_on_back,dead_on_stomach
//=========================================================
void CDeadZombie::KeyValue(KeyValueData *pkvd) {
	if (FStrEq(pkvd->szKeyName, "pose")) {
		m_iPose = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue(pkvd);
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CDeadZombie::Classify() {
	return m_iClass ? m_iClass : CLASS_HUMAN_MILITARY;
}

//=========================================================
// Spawn DEAD Zombie
//=========================================================
void CDeadZombie::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/zombie.mdl");

	pev->effects = 0;
	pev->yaw_speed = 8;
	pev->sequence = 0;
	pev->body = 1;
	pev->health = 8;
	m_bloodColor = BLOOD_COLOR_RED;

	pev->sequence = LookupSequence(m_szPoses[m_iPose]);

	if (pev->sequence == -1) {
		ALERT(at_console, "Dead zombie with bad pose\n");
		pev->sequence = 0;
		pev->effects = EF_BRIGHTFIELD;
	}

	MonsterInitDead();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CDeadZombie::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/zombie.mdl");
}
