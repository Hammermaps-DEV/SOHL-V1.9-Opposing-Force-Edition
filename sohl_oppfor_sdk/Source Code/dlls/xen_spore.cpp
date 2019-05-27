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
#include "animation.h"
#include "effects.h"
#include "baseactanimating.h"
#include "xen_hull.h"
#include "xen_spore.h"

//=========================================================
// Link Entity to Class
//=========================================================
LINK_ENTITY_TO_CLASS(xen_spore_small, CXenSporeSmall);
LINK_ENTITY_TO_CLASS(xen_spore_medium, CXenSporeMed);
LINK_ENTITY_TO_CLASS(xen_spore_large, CXenSporeLarge);

const char *CXenSpore::pModelNames[] = {
	"models/fungus(small).mdl",
	"models/fungus.mdl",
	"models/fungus(large).mdl"
};

// I just eyeballed these -- fill in hulls for the legs
const Vector CXenSporeLarge::m_hullSizes[] = {
	Vector(90, -25, 0),
	Vector(25, 75, 0),
	Vector(-15, -100, 0),
	Vector(-90, -35, 0),
	Vector(-90, 60, 0),
};

//=========================================================
// Spawn
//=========================================================
void CXenSpore::Spawn() {
	Precache();

	SET_MODEL(ENT(pev), pModelNames[pev->skin]);
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_YES;

	//	SetActivity( ACT_IDLE );
	pev->sequence = 0;
	pev->frame = RANDOM_FLOAT(0, 255);
	pev->framerate = RANDOM_FLOAT(0.7, 1.4);
	ResetSequenceInfo();
	SetNextThink(RANDOM_FLOAT(0.1, 0.4));	// Load balance these a bit
}

void CXenSporeSmall::Spawn() {
	pev->skin = 0;
	CXenSpore::Spawn();
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 64));
}

void CXenSporeMed::Spawn() {
	pev->skin = 1;
	CXenSpore::Spawn();
	UTIL_SetSize(pev, Vector(-40, -40, 0), Vector(40, 40, 120));
}

void CXenSporeLarge::Spawn() {
	pev->skin = 2;
	CXenSpore::Spawn();
	UTIL_SetSize(pev, Vector(-48, -48, 110), Vector(48, 48, 240));

	Vector forward, right;

	UTIL_MakeVectorsPrivate(pev->angles, forward, right, NULL);

	// Rotate the leg hulls into position
	for (int i = 0; i < HL_ARRAYSIZE(m_hullSizes); i++)
		CXenHull::CreateHull(this, Vector(-12, -12, 0), Vector(12, 12, 120), (m_hullSizes[i].x * forward) + (m_hullSizes[i].y * right));
}

//=========================================================
// Precache
//=========================================================
void CXenSpore::Precache()
{
	PRECACHE_MODEL((char *)pModelNames[pev->skin]);
}

//=========================================================
// Think
//=========================================================
void CXenSpore::Think()
{
	float flInterval = StudioFrameAdvance();
	SetNextThink(0.1);
}