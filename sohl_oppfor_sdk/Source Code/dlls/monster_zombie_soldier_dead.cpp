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
// DEAD Zombie PROP
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include    "monster_zombie_soldier_dead.h"

char *CDeadZombieSoldier::m_szPoses[] = { "dead_on_back", "dead_on_stomach" };

void CDeadZombieSoldier::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "pose"))
	{
		m_iPose = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue(pkvd);
}

LINK_ENTITY_TO_CLASS(monster_zombie_soldier_dead, CDeadZombieSoldier);

//=========================================================
// ********** CDeadZombie SPAWN **********
//=========================================================
void CDeadZombieSoldier::Spawn(void)
{
	PRECACHE_MODEL("models/zombie_soldier.mdl");
	SET_MODEL(ENT(pev), "models/zombie_soldier.mdl");

	pev->effects = 0;
	pev->yaw_speed = 8;
	pev->sequence = 0;
	pev->body = 1;
	m_bloodColor = BLOOD_COLOR_RED;

	pev->sequence = LookupSequence(m_szPoses[m_iPose]);

	if (pev->sequence == -1)
	{
		ALERT(at_console, "Dead zombie soldier with bad pose\n");
		pev->sequence = 0;
		pev->effects = EF_BRIGHTFIELD;
	}

	// Corpses have less health
	pev->health = 8;

	MonsterInitDead();
}