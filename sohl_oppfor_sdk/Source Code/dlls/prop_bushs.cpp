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

void CPropBushs::Spawn() {
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

	SetSolidType(SOLID_NOT);
	SetMoveType(MOVETYPE_NONE);
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

void CPropBushs::Precache() {
	if (strchr(strdup(STRING(m_isBush)), '0') && !pev->model)
		PRECACHE_MODEL(RandomMDL);
	else if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/error.mdl"); //LRC
}