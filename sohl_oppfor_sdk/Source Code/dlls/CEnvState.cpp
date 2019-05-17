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
#include "CEnvState.h"

//==================================================
//LRC- a simple entity, just maintains a state
//==================================================

void CEnvState::Spawn()
{
	if (pev->spawnflags & SF_ENVSTATE_START_ON)
		m_iState = STATE_ON;
	else
		m_iState = STATE_OFF;
}

TYPEDESCRIPTION CEnvState::m_SaveData[] =
{
	DEFINE_FIELD(CEnvState, m_iState, FIELD_INTEGER),
	DEFINE_FIELD(CEnvState, m_fTurnOnTime, FIELD_INTEGER),
	DEFINE_FIELD(CEnvState, m_fTurnOffTime, FIELD_INTEGER),
	DEFINE_FIELD(CEnvState, m_sMaster, FIELD_STRING),
};

IMPLEMENT_SAVERESTORE(CEnvState, CPointEntity);

LINK_ENTITY_TO_CLASS(env_state, CEnvState);

void CEnvState::KeyValue(KeyValueData *pkvd)
{
	pkvd->fHandled = TRUE;

	if (FStrEq(pkvd->szKeyName, "turnontime"))
		m_fTurnOnTime = atof(pkvd->szValue);
	else if (FStrEq(pkvd->szKeyName, "turnofftime"))
		m_fTurnOffTime = atof(pkvd->szValue);
	else if (FStrEq(pkvd->szKeyName, "master"))
		m_sMaster = ALLOC_STRING(pkvd->szValue);
	else
		CPointEntity::KeyValue(pkvd);
}

void CEnvState::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType) || IsLockedByMaster())
	{
		if (pev->spawnflags & SF_ENVSTATE_DEBUG)
		{
			ALERT(at_debug, "DEBUG: env_state \"%s\" ", STRING(pev->targetname));
			if (IsLockedByMaster())
				ALERT(at_debug, "ignored trigger %s; locked by master \"%s\".\n", GetStringForUseType(useType), STRING(m_sMaster));
			else if (useType == USE_ON)
				ALERT(at_debug, "ignored trigger USE_ON; already on\n");
			else if (useType == USE_OFF)
				ALERT(at_debug, "ignored trigger USE_OFF; already off\n");
			else
				ALERT(at_debug, "ignored trigger %s.\n", GetStringForUseType(useType));
		}
		return;
	}

	switch (GetState())
	{
	case STATE_ON:
	case STATE_TURN_ON:
		if (m_fTurnOffTime)
		{
			m_iState = STATE_TURN_OFF;
			if (pev->spawnflags & SF_ENVSTATE_DEBUG)
			{
				ALERT(at_debug, "DEBUG: env_state \"%s\" triggered; will turn off in %f seconds.\n", STRING(pev->targetname), m_fTurnOffTime);
			}
			SetNextThink(m_fTurnOffTime);
		}
		else
		{
			m_iState = STATE_OFF;
			if (pev->spawnflags & SF_ENVSTATE_DEBUG)
			{
				ALERT(at_debug, "DEBUG: env_state \"%s\" triggered, turned off", STRING(pev->targetname));
				if (pev->target)
				{
					ALERT(at_debug, ": firing \"%s\"", STRING(pev->target));
					if (pev->noise2)
						ALERT(at_debug, " and \"%s\"", STRING(pev->noise2));
				}
				else if (pev->noise2)
					ALERT(at_debug, ": firing \"%s\"", STRING(pev->noise2));
				ALERT(at_debug, ".\n");
			}
			FireTargets(STRING(pev->target), pActivator, this, USE_OFF, 0);
			FireTargets(STRING(pev->noise2), pActivator, this, USE_TOGGLE, 0);
			DontThink();
		}
		break;
	case STATE_OFF:
	case STATE_TURN_OFF:
		if (m_fTurnOnTime)
		{
			m_iState = STATE_TURN_ON;
			if (pev->spawnflags & SF_ENVSTATE_DEBUG)
			{
				ALERT(at_debug, "DEBUG: env_state \"%s\" triggered; will turn on in %f seconds.\n", STRING(pev->targetname), m_fTurnOnTime);
			}
			SetNextThink(m_fTurnOnTime);
		}
		else
		{
			m_iState = STATE_ON;
			if (pev->spawnflags & SF_ENVSTATE_DEBUG)
			{
				ALERT(at_debug, "DEBUG: env_state \"%s\" triggered, turned on", STRING(pev->targetname));
				if (pev->target)
				{
					ALERT(at_debug, ": firing \"%s\"", STRING(pev->target));
					if (pev->noise1)
						ALERT(at_debug, " and \"%s\"", STRING(pev->noise1));
				}
				else if (pev->noise1)
					ALERT(at_debug, ": firing \"%s\"", STRING(pev->noise1));
				ALERT(at_debug, ".\n");
			}
			FireTargets(STRING(pev->target), pActivator, this, USE_ON, 0);
			FireTargets(STRING(pev->noise1), pActivator, this, USE_TOGGLE, 0);
			DontThink();
		}
		break;
	}
}

void CEnvState::Think()
{
	if (m_iState == STATE_TURN_ON)
	{
		m_iState = STATE_ON;
		if (pev->spawnflags & SF_ENVSTATE_DEBUG)
		{
			ALERT(at_debug, "DEBUG: env_state \"%s\" turned itself on", STRING(pev->targetname));
			if (pev->target)
			{
				ALERT(at_debug, ": firing %s", STRING(pev->target));
				if (pev->noise1)
					ALERT(at_debug, " and %s", STRING(pev->noise1));
			}
			else if (pev->noise1)
				ALERT(at_debug, ": firing %s", STRING(pev->noise1));
			ALERT(at_debug, ".\n");
		}
		FireTargets(STRING(pev->target), this, this, USE_ON, 0);
		FireTargets(STRING(pev->noise1), this, this, USE_TOGGLE, 0);
	}
	else if (m_iState == STATE_TURN_OFF)
	{
		m_iState = STATE_OFF;
		if (pev->spawnflags & SF_ENVSTATE_DEBUG)
		{
			ALERT(at_debug, "DEBUG: env_state \"%s\" turned itself off", STRING(pev->targetname));
			if (pev->target)
				ALERT(at_debug, ": firing %s", STRING(pev->target));
			if (pev->noise2)
				ALERT(at_debug, " and %s", STRING(pev->noise2));
			else if (pev->noise2)
				ALERT(at_debug, ": firing %s", STRING(pev->noise2));
			ALERT(at_debug, ".\n");
		}
		FireTargets(STRING(pev->target), this, this, USE_OFF, 0);
		FireTargets(STRING(pev->noise2), this, this, USE_TOGGLE, 0);
	}
}