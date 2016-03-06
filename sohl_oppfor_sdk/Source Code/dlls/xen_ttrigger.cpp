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