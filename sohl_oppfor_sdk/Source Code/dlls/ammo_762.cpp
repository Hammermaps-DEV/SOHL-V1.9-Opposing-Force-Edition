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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"

class CSniperAmmo : public CBasePlayerAmmo
{
	//=========================================================
	// Spawn
	//=========================================================
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_m40a1clip.mdl");
		CBasePlayerAmmo::Spawn();
	}

	//=========================================================
	// Precache
	//=========================================================
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_m40a1clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}

	//=========================================================
	// AddAmmo
	//=========================================================
	BOOL AddAmmo(CBaseEntity *pOther)
	{
		if (pOther->GiveAmmo(AMMO_762BOX_GIVE, "762", _762_MAX_CARRY) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(ammo_762, CSniperAmmo);