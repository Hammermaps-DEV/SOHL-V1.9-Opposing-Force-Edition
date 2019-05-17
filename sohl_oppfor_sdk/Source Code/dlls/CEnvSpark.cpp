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
#include "saverestore.h"
#include "CEnvSpark.h"

TYPEDESCRIPTION CEnvSpark::m_SaveData[] =
{
	DEFINE_FIELD(CEnvSpark, m_flDelay, FIELD_FLOAT),
	DEFINE_FIELD(CEnvSpark, m_iState, FIELD_INTEGER), //LRC
};

IMPLEMENT_SAVERESTORE(CEnvSpark, CBaseEntity);

LINK_ENTITY_TO_CLASS(env_spark, CEnvSpark);
LINK_ENTITY_TO_CLASS(env_debris, CEnvSpark);

void CEnvSpark::Spawn()
{
	SetThink(NULL);
	SetUse(NULL);

	if (FBitSet(pev->spawnflags, 16))
	{
		SetUse(&CEnvSpark::SparkCyclic);
	}
	else if (FBitSet(pev->spawnflags, 32)) // Use for on/off
	{
		if (FBitSet(pev->spawnflags, 64)) // Start on
		{
			SetThink(&CEnvSpark::SparkThink);	// start sparking
			SetUse(&CEnvSpark::SparkStop);		// set up +USE to stop sparking
		}
		else
			SetUse(&CEnvSpark::SparkStart);
	}
	else
		SetThink(&CEnvSpark::SparkThink);

	if (this->m_pfnThink)
	{
		SetNextThink(0.1 + RANDOM_FLOAT(0, 1.5));

		if (m_flDelay <= 0)
			m_flDelay = 1.5;
	}

	Precache();
}


void CEnvSpark::Precache()
{
	PRECACHE_SOUND("buttons/spark1.wav");
	PRECACHE_SOUND("buttons/spark2.wav");
	PRECACHE_SOUND("buttons/spark3.wav");
	PRECACHE_SOUND("buttons/spark4.wav");
	PRECACHE_SOUND("buttons/spark5.wav");
	PRECACHE_SOUND("buttons/spark6.wav");
}

void CEnvSpark::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "MaxDelay"))
	{
		m_flDelay = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "style") ||
		FStrEq(pkvd->szKeyName, "height") ||
		FStrEq(pkvd->szKeyName, "killtarget") ||
		FStrEq(pkvd->szKeyName, "value1") ||
		FStrEq(pkvd->szKeyName, "value2") ||
		FStrEq(pkvd->szKeyName, "value3"))
		pkvd->fHandled = TRUE;
	else
		CBaseEntity::KeyValue(pkvd);
}

void EXPORT CEnvSpark::SparkCyclic(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (m_pfnThink == NULL)
	{
		DoSpark(pev, pev->origin);
		SetThink(&CEnvSpark::SparkWait);
		SetNextThink(m_flDelay);
	}
	else
	{
		SetThink(&CEnvSpark::SparkThink); // if we're on SparkWait, change to actually spark at the specified time.
	}
}

void EXPORT CEnvSpark::SparkWait()
{
	SetThink(NULL);
}

void EXPORT CEnvSpark::SparkThink()
{
	DoSpark(pev, pev->origin);
	if (pev->spawnflags & 16)
	{
		SetThink(NULL);
	}
	else
	{
		SetNextThink(0.1 + RANDOM_FLOAT(0, m_flDelay));
	}
}

void EXPORT CEnvSpark::SparkStart(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	SetUse(&CEnvSpark::SparkStop);
	SetThink(&CEnvSpark::SparkThink);
	m_iState = STATE_ON; //LRC
	SetNextThink(0.1 + RANDOM_FLOAT(0, m_flDelay));
}

void EXPORT CEnvSpark::SparkStop(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	SetUse(&CEnvSpark::SparkStart);
	SetThink(NULL);
	m_iState = STATE_OFF; //LRC
}

void CEnvSpark::DoSpark(entvars_t *pev, const Vector &location)
{
	Vector tmp = location + pev->size * 0.5;
	UTIL_Sparks(tmp);

	float flVolume = RANDOM_FLOAT(0.25, 0.75) * 0.4;//random volume range
	switch ((int)(RANDOM_FLOAT(0, 1) * 6))
	{
	case 0: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark1.wav", flVolume, ATTN_NORM);	break;
	case 1: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark2.wav", flVolume, ATTN_NORM);	break;
	case 2: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark3.wav", flVolume, ATTN_NORM);	break;
	case 3: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark4.wav", flVolume, ATTN_NORM);	break;
	case 4: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark5.wav", flVolume, ATTN_NORM);	break;
	case 5: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark6.wav", flVolume, ATTN_NORM);	break;
	}
}