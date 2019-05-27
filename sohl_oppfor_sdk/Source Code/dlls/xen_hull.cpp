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
#include "xen_hull.h"

//=========================================================
// Link Entity to Class
//=========================================================
LINK_ENTITY_TO_CLASS(xen_hull, CXenHull);

CXenHull *CXenHull::CreateHull(CBaseEntity *source, const Vector &mins, const Vector &maxs, const Vector &offset) {
	CXenHull *pHull = GetClassPtr((CXenHull *)NULL);
	UTIL_SetOrigin(pHull, source->pev->origin + offset);
	SET_MODEL(pHull->edict(), STRING(source->pev->model));
	pHull->pev->solid = SOLID_BBOX;
	pHull->pev->classname = MAKE_STRING("xen_hull");
	pHull->pev->movetype = MOVETYPE_NONE;
	pHull->pev->owner = source->edict();
	UTIL_SetSize(pHull->pev, mins, maxs);
	pHull->pev->renderamt = 0;
	pHull->pev->rendermode = kRenderTransTexture;
	return pHull;
}