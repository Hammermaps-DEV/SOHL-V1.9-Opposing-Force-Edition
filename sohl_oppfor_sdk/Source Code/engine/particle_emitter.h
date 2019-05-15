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

#ifndef PATICLE_EMITTER
#define PATICLE_EMITTER

#include "particle_defs.h"

const unsigned int SF_START_ON = 1;
const unsigned int SF_TRIGGERABLE = 2;

class CParticleEmitter : public CPointEntity {
	char sParticleDefintionFile[MAX_PARTICLE_PATH];
	unsigned int iID;
	float flTimeTurnedOn;
public:
	bool bIsOn;
    void Spawn( void );
    void KeyValue( KeyValueData* pKeyValueData );
    void MakeAware( CBaseEntity* pEnt );
	void EXPORT Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int ObjectCaps( void ) { return CBaseEntity::ObjectCaps() | FCAP_MASTER; }
	bool IsTriggered( CBaseEntity *pActivator );
};

static unsigned int iParticleIDCount = 0;
#endif
