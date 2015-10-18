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
// NPC: Soldier Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00002 / Date: 18.10.2015
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"monster_zombie_soldier.h"

//=========================================================
// Monster's link to Class
//=========================================================
LINK_ENTITY_TO_CLASS( monster_zombie_soldier, CZombieSoldier );

//=========================================================
// Spawn
//=========================================================
void CZombieSoldier :: Spawn() {
	Precache( );

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/zombie_soldier.mdl");

	if (pev->health == 0)
		pev->health		= gSkillData.zombieSoldierHealth;

	m_flHitgroupHead    = gSkillData.zombieSoldierHead;
	m_flHitgroupChest   = gSkillData.zombieSoldierChest;
	m_flHitgroupStomach = gSkillData.zombieSoldierStomach;
	m_flHitgroupArm     = gSkillData.zombieSoldierArm;
	m_flHitgroupLeg     = gSkillData.zombieSoldierLeg;
	m_flDmgOneSlash     = gSkillData.zombieSoldierDmgOneSlash;
	m_flDmgBothSlash    = gSkillData.zombieSoldierDmgBothSlash;

	CZombie::Spawn();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CZombieSoldier :: Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/zombie_soldier.mdl");

	CZombie::Precache();
}
