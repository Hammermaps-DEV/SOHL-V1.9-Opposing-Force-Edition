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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "animation.h"
#include "weapons.h"
#include "player.h"
#include "prop_generic.h"

LINK_ENTITY_TO_CLASS(prop_generic, CPropGeneric);

void CPropGeneric::Spawn() {
	Vector vecSize = pev->size;

	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/error.mdl");

	UTIL_SetOrigin(this, pev->origin);

	pev->solid = SOLID_SLIDEBOX;

	Vector vecMax = vecSize / 2;
	Vector vecMin = -vecMax;
	vecMin.z = 0;
	vecMax.z = vecSize.z;
	UTIL_SetSize(pev, vecMin, vecMax);

	SetBoneController(0, 0);
	SetBoneController(1, 0);
}

void CPropGeneric::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/error.mdl"); //LRC
}