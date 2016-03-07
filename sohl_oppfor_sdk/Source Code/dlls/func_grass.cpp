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
	} else {
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
	} else if (useType == USE_ON) {
		bTurnOn = true;
	} else if (useType == USE_SET) {
		if (value != 0)
			bTurnOn = true;
		else
			bTurnOn = false;
	} else {
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
	} else {
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
