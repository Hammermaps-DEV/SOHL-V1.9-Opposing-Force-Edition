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
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/error.mdl");

	SetBoneController(0, 0);
	SetBoneController(1, 0);

	pev->solid = SOLID_SLIDEBOX;

	// Automatically set collision box
	studiohdr_t *pstudiohdr;
	pstudiohdr = (studiohdr_t *)GET_MODEL_PTR(edict());

	if (pstudiohdr == NULL)
	{
		Vector vecSize = pev->size;
		Vector vecMax = vecSize / 2;
		Vector vecMin = -vecMax;
		vecMin.z = 0;
		vecMax.z = vecSize.z;
		UTIL_SetSize(pev, vecMin, vecMax);
		ALERT(at_console, "^2prop_default: Unable to get model pointer!\n");
		return;
	}
	
	mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);
	UTIL_SetSize(pev, pseqdesc[pev->sequence].bbmin, pseqdesc[pev->sequence].bbmax);

	UTIL_SetOrigin(this, pev->origin);
	if (DROP_TO_FLOOR(ENT(pev)) == 0)
	{
		ALERT(at_error, "Prop %s fell out of level at %f,%f,%f\n", STRING(pev->classname), pev->origin.x, pev->origin.y, pev->origin.z);
		UTIL_Remove(this);
		return;
	}
}

void CPropGeneric::Precache(void) {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/error.mdl"); //LRC
}