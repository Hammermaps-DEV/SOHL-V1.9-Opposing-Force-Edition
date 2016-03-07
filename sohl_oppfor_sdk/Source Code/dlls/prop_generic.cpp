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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "animation.h"
#include "weapons.h"
#include "player.h"
#include "prop_generic.h"

LINK_ENTITY_TO_CLASS(prop_generic, CPropGeneric);

void CPropGeneric::Spawn(void) {
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

void CPropGeneric::Precache(void) {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/error.mdl"); //LRC
}