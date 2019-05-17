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
#include "CEnvGlobal.h"

TYPEDESCRIPTION CEnvGlobal::m_SaveData[] =
{
	DEFINE_FIELD(CEnvGlobal, m_globalstate, FIELD_STRING),
	DEFINE_FIELD(CEnvGlobal, m_triggermode, FIELD_INTEGER),
	DEFINE_FIELD(CEnvGlobal, m_initialstate, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CEnvGlobal, CBaseEntity);

LINK_ENTITY_TO_CLASS(env_global, CEnvGlobal);

void CEnvGlobal::KeyValue(KeyValueData *pkvd)
{
	pkvd->fHandled = TRUE;

	if (FStrEq(pkvd->szKeyName, "globalstate"))		// State name
		m_globalstate = ALLOC_STRING(pkvd->szValue);
	else if (FStrEq(pkvd->szKeyName, "triggermode"))
		m_triggermode = atoi(pkvd->szValue);
	else if (FStrEq(pkvd->szKeyName, "initialstate"))
		m_initialstate = atoi(pkvd->szValue);
	else
		CPointEntity::KeyValue(pkvd);
}

void CEnvGlobal::Spawn()
{
	if (!m_globalstate)
	{
		REMOVE_ENTITY(ENT(pev));
		return;
	}
	if (FBitSet(pev->spawnflags, SF_GLOBAL_SET))
	{
		if (!gGlobalState.EntityInTable(m_globalstate))
			gGlobalState.EntityAdd(m_globalstate, gpGlobals->mapname, (GLOBALESTATE)m_initialstate);
	}
}

void CEnvGlobal::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	GLOBALESTATE oldState = gGlobalState.EntityGetState(m_globalstate);
	GLOBALESTATE newState;

	if (useType == USE_ON) {		//
		newState = GLOBAL_ON;	//AJH Allow env_global to use USE_TYPE
	}
	else if (useType == USE_OFF) {	//
		newState = GLOBAL_OFF;	//
	}
	else {						//

		switch (m_triggermode)
		{
		case 0:
			newState = GLOBAL_OFF;
			break;

		case 1:
			newState = GLOBAL_ON;
			break;

		case 2:
			newState = GLOBAL_DEAD;
			break;

		default:
		case 3:
			if (oldState == GLOBAL_ON)
				newState = GLOBAL_OFF;
			else if (oldState == GLOBAL_OFF)
				newState = GLOBAL_ON;
			else
				newState = oldState;
		}
	}

	if (gGlobalState.EntityInTable(m_globalstate))
		gGlobalState.EntitySetState(m_globalstate, newState);
	else
		gGlobalState.EntityAdd(m_globalstate, gpGlobals->mapname, newState);
}