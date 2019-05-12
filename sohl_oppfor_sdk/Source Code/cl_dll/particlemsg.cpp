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

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "particlemgr.h"
#include "particlesys.h"

#include <string.h>
#include <stdio.h>

extern ParticleSystemManager* g_pParticleSystems;

DECLARE_MESSAGE(m_Particle, Particle)

int CHudParticle::Init()
{
	HOOK_MESSAGE(Particle);
	gHUD.AddHudElem(this);

	if (g_pParticleSystems)
	{
		delete g_pParticleSystems;
		g_pParticleSystems = NULL;
	}

	g_pParticleSystems = new ParticleSystemManager();

	return 1;
};

int CHudParticle::VidInit()
{
	g_pParticleSystems->ClearSystems();
	return 1;
};

int CHudParticle:: MsgFunc_Particle(const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int entindex = READ_WORD();
	char *sz = READ_STRING();

	ParticleSystem *pSystem = new ParticleSystem(entindex, sz);
	g_pParticleSystems->AddSystem(pSystem);

	return 1;
}

int CHudParticle::Draw(float flTime)
{
	return 1;
}
