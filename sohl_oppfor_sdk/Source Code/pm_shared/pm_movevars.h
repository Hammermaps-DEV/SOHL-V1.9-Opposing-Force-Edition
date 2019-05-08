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

// pm_movevars.h
#if !defined( PM_MOVEVARSH )
#define PM_MOVEVARSH

// movevars_t                  // Physics variables.
typedef struct movevars_s movevars_t;

struct movevars_s
{
	float	gravity;           // Gravity for map
	float	stopspeed;         // Deceleration when not moving
	float	maxspeed;          // Max allowed speed
	float	spectatormaxspeed;
	float	accelerate;        // Acceleration factor
	float	airaccelerate;     // Same for when in open air
	float	wateraccelerate;   // Same for when in water
	float	friction;          
	float   edgefriction;	   // Extra friction near dropofs 
	float	waterfriction;     // Less in water
	float	entgravity;        // 1.0
	float   bounce;            // Wall bounce value. 1.0
	float   stepsize;          // sv_stepsize;
	float   maxvelocity;       // maximum server velocity.
	float	zmax;			   // Max z-buffer range (for GL)
	float	waveHeight;		   // Water wave height (for GL)
	qboolean footsteps;        // Play footstep sounds
	char	skyName[32];	   // Name of the sky map
	float	rollangle;
	float	rollspeed;
	float	skycolor_r;			// Sky color
	float	skycolor_g;			// 
	float	skycolor_b;			//
	float	skyvec_x;			// Sky vector
	float	skyvec_y;			// 
	float	skyvec_z;			// 
};

extern movevars_t movevars;

#endif