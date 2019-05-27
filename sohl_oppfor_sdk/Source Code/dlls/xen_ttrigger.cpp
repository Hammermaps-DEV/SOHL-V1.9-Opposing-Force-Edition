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
#include "xen_ttrigger.h"

LINK_ENTITY_TO_CLASS(xen_ttrigger, CXenTreeTrigger);

CXenTreeTrigger *CXenTreeTrigger::TriggerCreate(edict_t *pOwner, const Vector &position)
{
	CXenTreeTrigger *pTrigger = GetClassPtr((CXenTreeTrigger *)NULL);
	pTrigger->pev->origin = position;
	pTrigger->pev->classname = MAKE_STRING("xen_ttrigger");
	pTrigger->pev->solid = SOLID_TRIGGER;
	pTrigger->pev->movetype = MOVETYPE_NONE;
	pTrigger->pev->owner = pOwner;

	return pTrigger;
}


void CXenTreeTrigger::Touch(CBaseEntity *pOther)
{
	if (pev->owner)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance(pev->owner);
		pEntity->Touch(pOther);
	}
}