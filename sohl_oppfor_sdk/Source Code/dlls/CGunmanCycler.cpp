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
#include "CCycler.h"
#include "CGenericCycler.h"
#include "CGunmanCycler.h"

void CGunmanCycler::Spawn()
{
	Precache();

	InitBoneControllers();

	SET_MODEL(ENT(pev), STRING(pev->model));

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;
	pev->effects = 0;
	pev->health = 80000;// no cycler should die
	pev->yaw_speed = 5;
	pev->ideal_yaw = pev->angles.y;

	m_flFrameRate = 75;
	m_flGroundSpeed = 0;

	SetBodygroup(1, m_iBodygroup[0]);
	SetBodygroup(2, m_iBodygroup[1]);
	SetBodygroup(3, m_iBodygroup[2]);

	SetNextThink(1.0);

	ResetSequenceInfo();

	if (pev->sequence != 0 || pev->frame != 0)
	{
		m_animate = 0;
		pev->framerate = 0;
	}
	else
	{
		m_animate = 1;
	}
}

void CGunmanCycler::Precache()
{
	PRECACHE_MODEL((char *)STRING(pev->model));
}

void CGunmanCycler::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "cyc_submodel1"))
	{
		m_iBodygroup[0] = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "cyc_submodel2"))
	{
		m_iBodygroup[1] = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "cyc_submodel3"))
	{
		m_iBodygroup[2] = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CGenericCycler::KeyValue(pkvd);
}

void CGunmanCycler::Think()
{
	SetNextThink(0.05);

	if (m_animate > 0)
	{
		StudioFrameAdvance();
	}

	if (m_fSequenceFinished && !m_fSequenceLoops)
	{
		// ResetSequenceInfo();
		// hack to avoid reloading model every frame
		pev->animtime = UTIL_GlobalTimeBase();
		pev->framerate = 1.0;
		m_fSequenceFinished = false;
		m_flLastEventCheck = UTIL_GlobalTimeBase();
		pev->frame = 0;
		if (m_animate <= 0)
			pev->framerate = 0.0;	// FIX: don't reset framerate
	}
}

LINK_ENTITY_TO_CLASS(gunman_cycler, CGunmanCycler);