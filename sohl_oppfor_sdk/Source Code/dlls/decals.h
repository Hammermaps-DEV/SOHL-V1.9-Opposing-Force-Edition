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
#ifndef DECALS_H
#define DECALS_H

//
// Dynamic Decals
//
enum decal_e
{
	DECAL_GUNSHOT1 = 0,
	DECAL_GUNSHOT2,
	DECAL_GUNSHOT3,
	DECAL_GUNSHOT4,
	DECAL_GUNSHOT5,

	DECAL_LAMBDA1,
	DECAL_LAMBDA2,
	DECAL_LAMBDA3,
	DECAL_LAMBDA4,
	DECAL_LAMBDA5,
	DECAL_LAMBDA6,

	DECAL_SCORCH1,
	DECAL_SCORCH2,
	DECAL_SCORCH3,

	DECAL_SHOCKSCORCH1,
	DECAL_SHOCKSCORCH2,
	DECAL_SHOCKSCORCH3,

	DECAL_SPORESPLAT1,
	DECAL_SPORESPLAT2,
	DECAL_SPORESPLAT3,

	DECAL_BLOOD1,
	DECAL_BLOOD2,
	DECAL_BLOOD3,
	DECAL_BLOOD4,
	DECAL_BLOOD5,
	DECAL_BLOOD6,
	DECAL_BLOOD7,
	DECAL_BLOOD8,

	DECAL_GBLOOD1,
	DECAL_GBLOOD2,

	DECAL_BBLOOD1,
	DECAL_BBLOOD2,

	DECAL_YBLOOD1,
	DECAL_YBLOOD2,
	DECAL_YBLOOD3,
	DECAL_YBLOOD4,
	DECAL_YBLOOD5,
	DECAL_YBLOOD6,

	DECAL_GLASSBREAK1,
	DECAL_GLASSBREAK2,
	DECAL_GLASSBREAK3,

	DECAL_BIGSHOT1,
	DECAL_BIGSHOT2,
	DECAL_BIGSHOT3,
	DECAL_BIGSHOT4,
	DECAL_BIGSHOT5,

	DECAL_SPIT1,
	DECAL_SPIT2,

	DECAL_BPROOF1,		// Bulletproof glass decal
	DECAL_GARGSTOMP1,	// Gargantua stomp crack

	DECAL_SMALLSCORCH1,	// Small scorch mark
	DECAL_SMALLSCORCH2,	// Small scorch mark
	DECAL_SMALLSCORCH3,	// Small scorch mark

	DECAL_MOMMABIRTH,	// Big momma birth splatter
	DECAL_MOMMASPLAT,
};

typedef struct
{
	char	*name;
	int		index;
} DLL_DECALLIST;

extern DLL_DECALLIST gDecals[];

#endif	// DECALS_H
