/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"

class CSporeAmmo : public CBasePlayerAmmo {
	//=========================================================
	// Spawn
	//=========================================================
	void Spawn(void) {
		Precache();
		SET_MODEL(ENT(pev), "models/spore.mdl");
		CBasePlayerAmmo::Spawn();
	}

	//=========================================================
	// Precache
	//=========================================================
	void Precache(void) {
		PRECACHE_MODEL("models/spore.mdl");
		PRECACHE_SOUND("weapons/spore_ammo.wav");
	}

	//=========================================================
	// AddAmmo
	//=========================================================
	BOOL AddAmmo(CBaseEntity *pOther) {
		int bResult = (pOther->GiveAmmo(AMMO_SPORE_GIVE, "spore", SPORE_MAX_CARRY) != -1);
		if (bResult) {
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/spore_ammo.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};

//=========================================================
// Link entity to Class
//=========================================================
LINK_ENTITY_TO_CLASS(ammo_spore, CSporeAmmo);