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
// NPC: Barney Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00001 / Date: 17.10.2015
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

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid    = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor  = BLOOD_COLOR_GREEN;

	if (pev->health == 0)
		pev->health = gSkillData.zombieBarneyHealth;

	pev->view_ofs   = VEC_VIEW;// position of the eyes relative to monster's origin.
	m_flFieldOfView = 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState  = MONSTERSTATE_NONE;
	m_afCapability  = bits_CAP_DOORS_GROUP;

	m_flHitgroupHead    = gSkillData.zombieBarneyHead;
	m_flHitgroupChest   = gSkillData.zombieBarneyChest;
	m_flHitgroupStomach = gSkillData.zombieBarneyStomach;
	m_flHitgroupArm     = gSkillData.zombieBarneyArm;
	m_flHitgroupLeg     = gSkillData.zombieBarneyLeg;
	m_flDmgOneSlash     = gSkillData.zombieBarneyDmgOneSlash;
	m_flDmgBothSlash    = gSkillData.zombieBarneyDmgBothSlash;

	MonsterInit();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CZombieBarney::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/zombie_barney.mdl");
}
