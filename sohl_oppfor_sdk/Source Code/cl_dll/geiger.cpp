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
//
// Geiger.cpp
//
// implementation of CHudAmmo class
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "parsemsg.h"
#include "soundengine.h"

DECLARE_MESSAGE(m_Geiger, Geiger )

int CHudGeiger::Init(void)
{
	HOOK_MESSAGE( Geiger );

	m_iGeigerRange = 0;
	m_iFlags = 0;

	gHUD.AddHudElem(this);

	srand( (unsigned)time( NULL ) );

	return 1;
};

int CHudGeiger::VidInit(void)
{
	return 1;
};

int CHudGeiger::MsgFunc_Geiger(const char *pszName,  int iSize, void *pbuf)
{

	BEGIN_READ( pbuf, iSize );

	// update geiger data
	m_iGeigerRange = READ_BYTE();
	m_iGeigerRange = m_iGeigerRange << 2;
	
	m_iFlags |= HUD_ACTIVE;

	return 1;
}

int CHudGeiger::Draw (float flTime)
{
	int pct;
	float flvol = 0.0f;
	int rg[3];
	int i;
	
	if (m_iGeigerRange < 1000 && m_iGeigerRange > 0)
	{
		// peicewise linear is better than continuous formula for this
		if (m_iGeigerRange > 800)
		{
			pct = 0;			//Con_Printf ( "range > 800\n");
		}
		else if (m_iGeigerRange > 600)
		{
			pct = 2;
			flvol = 0.4;		//Con_Printf ( "range > 600\n");
			rg[0] = 1;
			rg[1] = 1;
			i = 2;
		}
		else if (m_iGeigerRange > 500)
		{
			pct = 4;
			flvol = 0.5;		//Con_Printf ( "range > 500\n");
			rg[0] = 1;
			rg[1] = 2;
			i = 2;
		}
		else if (m_iGeigerRange > 400)
		{
			pct = 8;
			flvol = 0.6;		//Con_Printf ( "range > 400\n");
			rg[0] = 1;
			rg[1] = 2;
			rg[2] = 3;
			i = 3;
		}
		else if (m_iGeigerRange > 300)
		{
			pct = 8;
			flvol = 0.7;		//Con_Printf ( "range > 300\n");
			rg[0] = 2;
			rg[1] = 3;
			rg[2] = 4;
			i = 3;
		}
		else if (m_iGeigerRange > 200)
		{
			pct = 28;
			flvol = 0.78;		//Con_Printf ( "range > 200\n");
			rg[0] = 2;
			rg[1] = 3;
			rg[2] = 4;
			i = 3;
		}
		else if (m_iGeigerRange > 150)
		{
			pct = 40;
			flvol = 0.80;		//Con_Printf ( "range > 150\n");
			rg[0] = 3;
			rg[1] = 4;
			rg[2] = 5;
			i = 3;
		}
		else if (m_iGeigerRange > 100)
		{
			pct = 60;
			flvol = 0.85;		//Con_Printf ( "range > 100\n");
			rg[0] = 3;
			rg[1] = 4;
			rg[2] = 5;
			i = 3;
		}
		else if (m_iGeigerRange > 75)
		{
			pct = 80;
			flvol = 0.9;		//Con_Printf ( "range > 75\n");
			//gflGeigerDelay = cl.time + GEIGERDELAY * 0.75;
			rg[0] = 4;
			rg[1] = 5;
			rg[2] = 6;
			i = 3;
		}
		else if (m_iGeigerRange > 50)
		{
			pct = 90;
			flvol = 0.95;		//Con_Printf ( "range > 50\n");
			rg[0] = 5;
			rg[1] = 6;
			i = 2;
		}
		else
		{
			pct = 95;
			flvol = 1.0;		//Con_Printf ( "range < 50\n");
			rg[0] = 5;
			rg[1] = 6;
			i = 2;
		}

		flvol = (flvol * ((rand() & 127)) / 255) + 0.25; // UTIL_RandomFloat(0.25, 0.5);

		if ((rand() & 127) < pct || (rand() & 127) < pct)
		{
			//S_StartDynamicSound (-1, 0, rgsfx[rand() % i], r_origin, flvol, 1.0, 0, 100);	
			char sz[256];
			
			int j = rand() & 1;
			if (i > 2)
				j += rand() & 1;

			snprintf(sz, 256, "player/geiger%d.wav", j + 1);
			gSoundEngine.PlaySound("common/wpn_select.wav", g_vecZero, SND_2D, 0, CVAR_GET_FLOAT("volume_sfx"));
		}
	}

	return 1;
}
