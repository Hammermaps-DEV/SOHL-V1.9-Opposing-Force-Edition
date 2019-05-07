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
#include "func_grass.h"
#include "particle_emitter.h"

extern int gmsgGrassParticles;

// create ourselves a garden
void CGrass::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL(ENT(pev), STRING(pev->model));
	pev->effects |= EF_NODRAW;

	UTIL_SetOrigin(this, pev->origin);
	UTIL_SetSize(pev, pev->absmin, pev->absmax);

	SetUse(&CGrass::GrassUse);

	bIsOn = true;
	iID = ++iParticleIDCount;
}

//Load values from the bsp
void CGrass::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "definition_file")) {
		strncat(sParticleDefintionFile, pkvd->szValue, strlen(pkvd->szValue));
		pkvd->fHandled = true;
	}
	else {
		CBaseEntity::KeyValue(pkvd);
	}
}

// Let the player know there is a grass system
void CGrass::MakeAware(CBaseEntity *pEnt)
{
	// don't want to send a to tree again
	if (pEnt->IsPlayer() == false)
		return;

	// particle system has been triggered off or starts off
	if (bIsOn == false)
		return;

	// lets give them everything
	MESSAGE_BEGIN(MSG_ONE, gmsgGrassParticles, NULL, pEnt->pev);
	WRITE_SHORT(iID);
	WRITE_BYTE(0);
	WRITE_COORD(pev->absmax.x);
	WRITE_COORD(pev->absmax.y);
	WRITE_COORD(pev->absmax.z);
	WRITE_COORD(pev->absmin.x);
	WRITE_COORD(pev->absmin.y);
	WRITE_COORD(pev->absmin.z);
	WRITE_STRING(sParticleDefintionFile);
	MESSAGE_END();
}

// add or remove a grass system from the client
void CGrass::GrassUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	bool bTurnOn = true;

	// determine whether we are turning on or off
	if (useType == USE_OFF) {
		bTurnOn = false;
	}
	else if (useType == USE_ON) {
		bTurnOn = true;
	}
	else if (useType == USE_SET) {
		if (value != 0)
			bTurnOn = true;
		else
			bTurnOn = false;
	}
	else {
		if (bIsOn == true)
			bTurnOn = false;
		else
			bTurnOn = true;
	}

	if (bTurnOn == false) {
		// lets remove this ps
		MESSAGE_BEGIN(MSG_ALL, gmsgGrassParticles);
		WRITE_SHORT(iID);
		WRITE_BYTE(1);
		MESSAGE_END();
		bIsOn = false;
	}
	else {
		// increment the count just in case
		iID = ++iParticleIDCount;

		// tell everyone about our new grass
		MESSAGE_BEGIN(MSG_ALL, gmsgGrassParticles);
		WRITE_SHORT(iID);
		WRITE_BYTE(0);
		WRITE_COORD(pev->absmax.x);
		WRITE_COORD(pev->absmax.y);
		WRITE_COORD(pev->absmax.z);
		WRITE_COORD(pev->absmin.x);
		WRITE_COORD(pev->absmin.y);
		WRITE_COORD(pev->absmin.z);
		WRITE_STRING(sParticleDefintionFile);
		MESSAGE_END();

		bIsOn = true;
	}
}

LINK_ENTITY_TO_CLASS(func_grass, CGrass);
