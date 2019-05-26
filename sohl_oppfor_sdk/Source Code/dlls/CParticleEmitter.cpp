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
#include "CParticleEmitter.h"
#include <ctype.h>

extern char *UTIL_memfgets(byte *pMemFile, int fileSize, int &filePos, char *pBuffer, int bufferSize);

// create ourselves a particle emitter
void CParticleEmitter::Spawn()
{
	SetSolidType(SOLID_NOT);
	SetMoveType(MOVETYPE_NONE);
	SET_MODEL(ENT(pev), STRING(pev->model));
	pev->effects |= EF_NODRAW;

	UTIL_SetOrigin(this, pev->origin);
	UTIL_SetSize(pev, pev->absmin, pev->absmax);

	SetUse(&CParticleEmitter::Use);

	if (pev->spawnflags & SF_START_ON)
		bIsOn = true;
	else
		bIsOn = false;

	IsTriggered(NULL);
	iID = ++iParticleIDCount;
	flTimeTurnedOn = 0.0;
}

//Load values from the bsp
void CParticleEmitter::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "definition_file"))
	{
		strncat(sParticleDefintionFile, pkvd->szValue, strlen(pkvd->szValue));
		pkvd->fHandled = true;
	}
	else {
		CBaseEntity::KeyValue(pkvd);
	}
}

extern int gmsgParticles;

// Let the player know there is a particleemitter
void CParticleEmitter::MakeAware(CBaseEntity *pEnt)
{
	// don't want to send a to tree again
	if (pEnt->IsPlayer() == false)
		return;

	// particle system has been triggered off or starts off
	if (bIsOn == false)
		return;

	// lets give them everything
	if (CVAR_GET_FLOAT("r_particles") != 0) {
		MESSAGE_BEGIN(MSG_ONE, gmsgParticles, NULL, pEnt->pev);
		WRITE_SHORT(iID);
		WRITE_BYTE(0);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(pev->angles.x);
		WRITE_COORD(pev->angles.y);
		WRITE_COORD(pev->angles.z);
		WRITE_SHORT(0);
		WRITE_STRING(sParticleDefintionFile);
		MESSAGE_END();
	}
}
// add or remove a particle emitter from the client
void CParticleEmitter::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// not allowed to trigger
	if (!FBitSet(pev->spawnflags, SF_TRIGGERABLE))
		return;

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

	if (CVAR_GET_FLOAT("r_particles") != 0) {
		if (bTurnOn == false) {
			// lets remove this ps
			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
			WRITE_SHORT(iID);
			WRITE_BYTE(1);
			MESSAGE_END();
			bIsOn = false;
			flTimeTurnedOn = 0.0;
		}
		else {
			// increment the count just in case
			iID = ++iParticleIDCount;
			flTimeTurnedOn = UTIL_GlobalTimeBase();

			// tell everyone of our new ps
			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
			WRITE_SHORT(iID);
			WRITE_BYTE(0);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD(pev->angles.x);
			WRITE_COORD(pev->angles.y);
			WRITE_COORD(pev->angles.z);
			WRITE_SHORT(0);
			WRITE_STRING(sParticleDefintionFile);
			MESSAGE_END();

			bIsOn = true;
		}
	}
}

// is the particle system on.
bool CParticleEmitter::IsTriggered(CBaseEntity*) {
	// not on so it isn't triggered
	if (bIsOn == false)
		return false;

	int iFileSize = 0; int iPos = 0;
	byte *pFile = g_engfuncs.pfnLoadFileForMe(sParticleDefintionFile, &iFileSize);
	if (!pFile) {
		ALERT(at_console, "Bad Mapped Particle definition file specified %s\n", sParticleDefintionFile);
		return false;
	}

	char sBuffer[512]; char sSetting[64]; char sValue[64]; bool bFound = false;
	memset(sBuffer, 0, 512); memset(sSetting, 0, 64); memset(sValue, 0, 64);

	// loop through each line
	int i = 0; int j = 0;
	while (UTIL_memfgets(pFile, iFileSize, iPos, sBuffer, sizeof(sBuffer) - 1) != NULL) {
		i = 0; j = 0;
		// trim leading white spaces
		while (sBuffer[i] && isspace(sBuffer[i]))
			i++;

		// comment so next line
		if (sBuffer[i] == '/' && sBuffer[i + 1] == '/')
			continue;

		// read the setting
		while (i < 63 && sBuffer[i] && !isspace(sBuffer[i])) {
			sSetting[i] = sBuffer[i++];
		}
		sSetting[i] = '\0';

		// if the setting isn't the system_life move onto the next line
		if (strcmp(sSetting, "system_life"))
			continue;

		// remove the spaces between setting and value
		while (sBuffer[i] && isspace(sBuffer[i]))
			i++;

		// read the value
		while (j < 63 && sBuffer[i] && !isspace(sBuffer[i])) {
			sValue[j++] = sBuffer[i++];
		}
		sValue[j] = '\0';
		bFound = true;

		// have the value, run for the hills
		break;
	}

	g_engfuncs.pfnFreeFile(pFile);

	// defaults to -1
	if (bFound == false)
		return true;

	// infinite so it must be on
	if (atof(sValue) == -1.0)
		return true;

	// the time the system life + the time it was turned on is in the future
	if (atof(sValue) + flTimeTurnedOn + 0.1 > UTIL_GlobalTimeBase())
		return true;

	// not in the future so its dead
	return false;
}

LINK_ENTITY_TO_CLASS(env_particleemitter, CParticleEmitter);