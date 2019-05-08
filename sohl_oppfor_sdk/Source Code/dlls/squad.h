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
// squad.h
//=========================================================

// these are special group roles that are assigned to members when the group is formed.
// the reason these are explicitly assigned and tasks like throwing grenades to flush out 
// enemies is that it's bad to have two members trying to flank left at the same time, but 
// ok to have two throwing grenades at the same time. When a squad member cannot attack the 
// enemy, it will choose to execute its special role.
#define		bits_SQUAD_FLANK_LEFT		( 1 << 0 )
#define		bits_SQUAD_FLANK_RIGHT		( 1 << 1 )
#define		bits_SQUAD_ADVANCE			( 1 << 2 )
#define		bits_SQUAD_FLUSH_ATTACK		( 1 << 3 )