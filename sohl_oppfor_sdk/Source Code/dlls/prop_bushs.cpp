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
#include "prop_bushs.h"

#define PROP_BUSH_MODELS 6

LINK_ENTITY_TO_CLASS(prop_bush, CPropBushs);

void CPropBushs::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "bush")) {
		m_isBush = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CGenericModel::KeyValue(pkvd);
}

void CPropBushs::Spawn(void) {
	if (strchr(strdup(STRING(m_isBush)), '0') && !pev->model) {
		int m_iBush = RANDOM_LONG(2, PROP_BUSH_MODELS);
		if (m_iBush < 10) {
			snprintf(RandomMDL, 128, "%models/bush/bush_0%d.mdl", m_iBush);
		}
		else {
			snprintf(RandomMDL, 128, "%models/bush/bush_%d.mdl", m_iBush);
		}
	}

	Precache();

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;

	if (strchr(strdup(STRING(m_isBush)), '0') && !pev->model)
		SET_MODEL(ENT(pev), RandomMDL);
	else if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/error.mdl");

	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	pev->sequence = 0;

	pev->frame = 0;
	pev->framerate = 1.0f;

	SetThink(NULL);
	SetTouch(NULL);
	SetUse(NULL);
}

void CPropBushs::Precache(void) {
	if (strchr(strdup(STRING(m_isBush)), '0') && !pev->model)
		PRECACHE_MODEL(RandomMDL);
	else if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/error.mdl"); //LRC
}