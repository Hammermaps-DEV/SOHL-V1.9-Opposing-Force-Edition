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
// PROP: DEAD Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00001 / Date: 18.10.2015
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
	} else
		CBaseMonster::KeyValue(pkvd);
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CDeadZombie::Classify(void) {
	return m_iClass ? m_iClass : CLASS_HUMAN_MILITARY;
}

//=========================================================
// Spawn DEAD Zombie
//=========================================================
void CDeadZombie::Spawn(void) {
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
