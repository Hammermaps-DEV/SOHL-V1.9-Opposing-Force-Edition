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
#include "CDecoreSpacedebris.h"

void CDecoreSpacedebris::Spawn()
{
	CGunmanCycler::Spawn();

	pev->body = RANDOM_LONG(0, 3);

	SetSolidType(SOLID_NOT);
	SetMoveType(MOVETYPE_NONE);
	pev->effects |= EF_NODRAW;

	m_blRotate = false;
	m_flAnglespeed = 3.0f;
}

void CDecoreSpacedebris::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "dirx"))
	{
		pev->movedir.x = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "diry"))
	{
		pev->movedir.y = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "dirz"))
	{
		pev->movedir.z = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "forwardspeed"))
	{
		pev->speed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "anglespeed"))
	{
		m_flAnglespeed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "modelname"))
	{
		pev->model = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CGunmanCycler::KeyValue(pkvd);
}

void CDecoreSpacedebris::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	pev->effects &= ~EF_NODRAW;
	SetSolidType(SOLID_BBOX);
	SetMoveType(MOVETYPE_BOUNCEMISSILE);

	Vector vecAngles = UTIL_VecToAngles(pev->movedir);
	vecAngles.x = -vecAngles.x;
	vecAngles.y += 90.0f;

	UTIL_MakeVectors(vecAngles);

	Vector vecMoar = gpGlobals->v_forward * pev->speed;
	pev->velocity = pev->velocity + vecMoar;
	m_blRotate = true;
}

void CDecoreSpacedebris::Think()
{
	CGunmanCycler::Think();

	if (m_blRotate)
	{
		pev->angles.x += m_flAnglespeed * 0.1;
		pev->angles.y += m_flAnglespeed * 0.1;
		pev->angles.z += m_flAnglespeed * 0.1;

		if (pev->angles.x >= 360.0)
			pev->angles.x = 0.0;

		if (pev->angles.y >= 360.0)
			pev->angles.y = 0.0;

		if (pev->angles.z >= 360.0)
			pev->angles.z = 0.0;
	}
}

LINK_ENTITY_TO_CLASS(decore_spacedebris, CDecoreSpacedebris);