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
#include "CDecoreAsteroid.h"

void CDecoreAsteroid::Spawn()
{
	CGunmanCycler::Spawn();

	m_iAsteroidSize = ASTEROID_SIZE_SMALL;
	m_flMaxRotation = 0.5f;
	m_flMinRotation = 0.1f;

	switch (m_iAsteroidSize)
	{
	case ASTEROID_SIZE_BIG:
	{
		pev->body = 1;
		pev->scale = 5.0f;
		break;
	}
	case ASTEROID_SIZE_MEDIUM:
	{
		pev->body = 1;
		pev->scale = 5.0f;
		break;
	}
	case ASTEROID_SIZE_SMALL:
	{
		pev->body = 0;
		pev->scale = 5.0f;
		break;
	}
	default:
	{
		pev->body = 0;
		pev->scale = 3.0f;
	}
	break;
	}

	SetMoveType(MOVETYPE_FLY);
}

void CDecoreAsteroid::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "asteroidsize"))
	{
		m_iAsteroidSize = clamp(atoi(pkvd->szValue), ASTEROID_SIZE_SMALL, ASTEROID_SIZE_BIG);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "size"))
	{
		pev->scale = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "maxrotation"))
	{
		m_flMaxRotation = clamp(atof(pkvd->szValue), 0.0, 360.0f);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "minrotation"))
	{
		m_flMinRotation = clamp(atof(pkvd->szValue), 0.0, 360.0f);;
		pkvd->fHandled = TRUE;
	}
	else
		CGunmanCycler::KeyValue(pkvd);
}

void CDecoreAsteroid::Think()
{
	CGunmanCycler::Think();

	float flRotate = RANDOM_FLOAT(m_flMinRotation, m_flMaxRotation);

	pev->angles.x += flRotate;
	pev->angles.y += flRotate;
	pev->angles.z += flRotate;

	if (pev->angles.x >= 360.0)
		pev->angles.x = 0.0;

	if (pev->angles.y >= 360.0)
		pev->angles.y = 0.0;

	if (pev->angles.z >= 360.0)
		pev->angles.z = 0.0;
}

LINK_ENTITY_TO_CLASS(decore_asteroid, CDecoreAsteroid);