/*
    Copyright 2001 to 2004. The Battle Grounds Team and Contributors

    This file is part of the Battle Grounds Modification for Half-Life.

    The Battle Grounds Modification for Half-Life is free software;
    you can redistribute it and/or modify it under the terms of the
    GNU Lesser General Public License as published by the Free
    Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    The Battle Grounds Modification for Half-Life is distributed in
    the hope that it will be useful, but WITHOUT ANY WARRANTY; without
    even the implied warranty of MERCHANTABILITY or FITNESS FOR A
    PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with The Battle Grounds Modification for Half-Life;
    if not, write to the Free Software Foundation, Inc., 59 Temple Place,
    Suite 330, Boston, MA  02111-1307  USA

    You must obey the GNU Lesser General Public License in all respects for
    all of the code used other than code distributed with the Half-Life
    SDK developed by Valve.  If you modify this file, you may extend this
    exception to your version of the file, but you are not obligated to do so.
    If you do not wish to do so, delete this exception statement from your
    version.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "particle_emitter.h"
#include <ctype.h>

extern char *UTIL_memfgets( byte *pMemFile, int fileSize, int &filePos, char *pBuffer, int bufferSize );

// create ourselves a particle emitter
void CParticleEmitter::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL (ENT(pev), STRING(pev->model));
	pev->effects |= EF_NODRAW;
	
	UTIL_SetOrigin( this, pev->origin ); 		
	UTIL_SetSize( pev, pev->absmin, pev->absmax );

    SetUse ( &CParticleEmitter::Use );

	if(pev->spawnflags & SF_START_ON)
		bIsOn = true;
	else
		bIsOn = false;

	IsTriggered(NULL);
	iID = ++iParticleIDCount;
	flTimeTurnedOn = 0.0;
}

//Load values from the bsp
void CParticleEmitter::KeyValue( KeyValueData* pkvd )
{
	if ( FStrEq(pkvd->szKeyName, "definition_file") )
	{
		strncat(sParticleDefintionFile, pkvd->szValue, strlen(pkvd->szValue));
		pkvd->fHandled = true;
	} else {
		CBaseEntity::KeyValue( pkvd );
	}
}

extern int gmsgParticles;

// Let the player know there is a particleemitter
void CParticleEmitter::MakeAware( CBaseEntity *pEnt )
{
	// don't want to send a to tree again
	if(pEnt->IsPlayer() == false)
		return;

	// particle system has been triggered off or starts off
	if(bIsOn == false)
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
void CParticleEmitter::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// not allowed to trigger
	if(!FBitSet(pev->spawnflags, SF_TRIGGERABLE))
		return;

	bool bTurnOn = true;

	// determine whether we are turning on or off
	if(useType == USE_OFF) {
		bTurnOn = false;
	}else if(useType == USE_ON) {
		bTurnOn = true;
	} else if(useType == USE_SET) {
		if(value != 0)
			bTurnOn = true;
		else
			bTurnOn = false;
	} else {
		if(bIsOn == true)
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
			flTimeTurnedOn = gpGlobals->time;

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
bool CParticleEmitter::IsTriggered( CBaseEntity* ) {
	// not on so it isn't triggered
	if(bIsOn == false)
		return false;

	int iFileSize = 0; int iPos = 0;
	byte *pFile = g_engfuncs.pfnLoadFileForMe(sParticleDefintionFile, &iFileSize);
	if(!pFile) {
		ALERT(at_console, "Bad Mapped Particle definition file specified %s\n", sParticleDefintionFile);
		return false;
	}

	char sBuffer[512]; char sSetting[64]; char sValue[64]; bool bFound = false;
	memset(sBuffer, 0, 512); memset(sSetting, 0, 64); memset(sValue, 0, 64);

	// loop through each line
	int i = 0; int j = 0;
	while(UTIL_memfgets(pFile, iFileSize, iPos, sBuffer, sizeof(sBuffer) - 1) != NULL) {
		i = 0; j = 0;
		// trim leading white spaces
		while(sBuffer[i] && isspace(sBuffer[i]))
			i++;

		// comment so next line
		if(sBuffer[i] == '/' && sBuffer[i+1] == '/')
			continue;

		// read the setting
		while(i < 63 && sBuffer[i] && !isspace(sBuffer[i])) {
			sSetting[i] = sBuffer[i++];
		}
		sSetting[i] = '\0';

		// if the setting isn't the system_life move onto the next line
		if(strcmp(sSetting, "system_life"))
			continue;

		// remove the spaces between setting and value
		while(sBuffer[i] && isspace(sBuffer[i]))
			i++;

		// read the value
		while(j < 63 && sBuffer[i] && !isspace(sBuffer[i])) {
			sValue[j++] = sBuffer[i++];
		}
		sValue[j] = '\0';
		bFound = true;

		// have the value, run for the hills
		break;
	}

	g_engfuncs.pfnFreeFile(pFile);

	// defaults to -1
	if(bFound == false)
		return true;

	// infinite so it must be on
	if(atof(sValue) == -1.0)
		return true;

	// the time the system life + the time it was turned on is in the future
	if(atof(sValue) + flTimeTurnedOn + 0.1 > gpGlobals->time)
		return true;

	// not in the future so its dead
	return false;
}

LINK_ENTITY_TO_CLASS( env_particleemitter, CParticleEmitter );