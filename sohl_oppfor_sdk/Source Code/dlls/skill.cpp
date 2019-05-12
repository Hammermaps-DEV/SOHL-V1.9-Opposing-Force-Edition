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

//=========================================================
// skill.cpp - code for skill level concerns
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"skill.h"

skilldata_t	gSkillData;

//=========================================================
// take the name of a cvar, tack a digit for the skill level
// on, and return the value.of that Cvar 
//=========================================================
float GetSkillCvar(char *pName)
{
	int		iCount;
	float	flValue;
	char	szBuffer[64];

	iCount = snprintf(szBuffer, 64, "%s%d", pName, gSkillData.iSkillLevel);

	flValue = CVAR_GET_FLOAT(szBuffer);

	if (flValue <= 0)
	{
		ALERT(at_debug, "\n\n** GetSkillCVar Got a zero for %s **\n\n", szBuffer);
	}

	return flValue;
}

