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
// NPC: Soldier Helmet Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00002 / Date: 19.10.2015
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"monster_zombie_soldier_helmet.h"

//=========================================================
// Monster's link to Class
//=========================================================
LINK_ENTITY_TO_CLASS(monster_zombie_soldier_helmet, CZombieSoldierHelmet);

//=========================================================
// Spawn
//=========================================================
void CZombieSoldierHelmet::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/zombie_soldier_helmet.mdl");

	if (pev->health == 0)
		pev->health = gSkillData.zombieSoldierHealth;

	m_flDebug = false; //Debug Massages

	CZombieSoldier::Spawn();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CZombieSoldierHelmet::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/zombie_soldier_helmet.mdl");

	CZombieSoldier::Precache();
}
