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
	void Spawn();
	void KeyValue(KeyValueData* pKeyValueData);
	void MakeAware(CBaseEntity* pEnt);
	void EXPORT Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int ObjectCaps() { return CBaseEntity::ObjectCaps() | FCAP_MASTER; }
	bool IsTriggered(CBaseEntity *pActivator);
};

static unsigned int iParticleIDCount = 0;
#endif
