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
// PROP: DEAD Soldier Helmet Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00001 / Date: 19.10.2015
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include    "monster_zombie_soldier_helmet_dead.h"

//=========================================================
// Monster's link to Class
//=========================================================
LINK_ENTITY_TO_CLASS(monster_zombie_soldier_helmet_dead, CDeadZombieSoldierHelmet);

//=========================================================
// Spawn DEAD-Soldier Helmet Zombie
//=========================================================
void CDeadZombieSoldierHelmet::Spawn(void) {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/zombie_soldier_helmet.mdl");

	pev->sequence = LookupSequence(m_szPoses[m_iPose]);
	if (pev->sequence == -1) {
		ALERT(at_console, "Dead zombie soldier helmet with bad pose\n");
		pev->sequence = 0;
		pev->effects = EF_BRIGHTFIELD;
	}

	CDeadZombieSoldier::Spawn();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CDeadZombieSoldierHelmet::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/zombie_soldier_helmet.mdl");

	CDeadZombieSoldier::Precache();
}
