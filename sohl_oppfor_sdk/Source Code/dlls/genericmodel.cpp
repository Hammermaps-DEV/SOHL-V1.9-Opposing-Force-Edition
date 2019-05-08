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
#include "genericmodel.h"

LINK_ENTITY_TO_CLASS(model_generic, CGenericModel);

TYPEDESCRIPTION	CGenericModel::m_SaveData[] =
{
	DEFINE_FIELD(CGenericModel, m_iszSequence, FIELD_STRING),
};

IMPLEMENT_SAVERESTORE(CGenericModel, CBaseAnimating);

void CGenericModel::KeyValue(KeyValueData* pkvd)
{
	// UNDONE_WC: explicitly ignoring these fields, but they shouldn't be in the map file!
	if (FStrEq(pkvd->szKeyName, "sequencename")) {
		m_iszSequence = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue(pkvd);
}

void CGenericModel::Spawn(void)
{
	Precache();

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;

	SET_MODEL(ENT(pev), STRING(pev->model));
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	if (!FStringNull(m_iszSequence))
	{
		pev->sequence = LookupSequence(STRING(m_iszSequence));

		if (pev->sequence < 0)
		{
			ALERT(at_warning, "Generic model %s: Unknown sequence named: %s\n", STRING(pev->model), STRING(m_iszSequence));
			pev->sequence = 0;
		}
	}
	else
	{
		pev->sequence = 0;
	}

	pev->frame = 0;
	pev->framerate = 1.0f;

	SetThink(&CGenericModel::IdleThink);
	SetTouch(NULL);
	SetUse(NULL);

	SetNextThink(0.1);
}

void CGenericModel::Precache(void)
{
	PRECACHE_MODEL((char*)STRING(pev->model));
}

void CGenericModel::IdleThink(void)
{
	float flInterval = StudioFrameAdvance();

	pev->nextthink = UTIL_GlobalTimeBase() + 0.5;

	DispatchAnimEvents(flInterval);

	if (m_fSequenceFinished)
	{
		if (m_fSequenceLoops)
		{
			pev->frame = 0;
			ResetSequenceInfo();
		}
		else
		{
			SetThink(NULL);
		}
	}
}