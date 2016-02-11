/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "animation.h"
#include "effects.h"
#include "baseactanimating.h"
#include "xen_hair.h"

//=========================================================
// Link Entity to Class
//=========================================================
LINK_ENTITY_TO_CLASS(xen_hair, CXenHair);

//=========================================================
// Spawn
//=========================================================
void CXenHair::Spawn(void) {
	Precache();
	SET_MODEL(edict(), "models/hair.mdl");
	UTIL_SetSize(pev, Vector(-4, -4, 0), Vector(4, 4, 32));
	pev->sequence = 0;

	if (!(pev->spawnflags & 0x0001)) {
		pev->frame = RANDOM_FLOAT(0, 255);
		pev->framerate = RANDOM_FLOAT(0.7, 1.4);
	}

	ResetSequenceInfo();

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	SetNextThink(RANDOM_FLOAT(0.1, 0.4));	// Load balance these a bit
}

//=========================================================
// Think
//=========================================================
void CXenHair::Think(void) {
	StudioFrameAdvance();
	SetNextThink(0.5);
}

//=========================================================
// Precache
//=========================================================
void CXenHair::Precache(void) {
	PRECACHE_MODEL("models/hair.mdl");
}