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
// pm_shared.h
//
#if !defined( PM_SHAREDH )
#define PM_SHAREDH
#pragma once

void PM_Init( struct playermove_s *ppmove );
void PM_Move ( struct playermove_s *ppmove, int server );
char PM_FindTextureType( char *name );

// Spectator Movement modes (stored in pev->iuser1, so the physics code can get at them)
#define OBS_NONE				0
#define OBS_CHASE_LOCKED		1
#define OBS_CHASE_FREE			2
#define OBS_ROAMING				3		
#define OBS_IN_EYE				4
#define OBS_MAP_FREE			5
#define OBS_MAP_CHASE			6

#endif