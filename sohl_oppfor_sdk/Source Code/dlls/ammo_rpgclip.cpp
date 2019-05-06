/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Spirit of Half-Life and their logos are the property of their respective owners.
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
*   All Rights Reserved.
*
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"

class CRpgAmmo : public CBasePlayerAmmo {
	//=========================================================
	// Spawn
	//=========================================================
	void Spawn(void) {
		Precache();
		SET_MODEL(ENT(pev), "models/w_rpgammo.mdl");
		CBasePlayerAmmo::Spawn();
	}

	//=========================================================
	// Precache
	//=========================================================
	void Precache(void) {
		PRECACHE_MODEL("models/w_rpgammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}

	//=========================================================
	// AddAmmo
	//=========================================================
	BOOL AddAmmo(CBaseEntity *pOther) {
		int iGive;

		if (IsMultiplayer()) {
			// hand out more ammo per rocket in multiplayer.
			iGive = AMMO_RPGCLIP_GIVE * 2;
		}
		else {
			iGive = AMMO_RPGCLIP_GIVE;
		}

		if (pOther->GiveAmmo(iGive, "rockets", ROCKET_MAX_CARRY) != -1) {
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}

		return FALSE;
	}
};

//=========================================================
// Link entity to Class
//=========================================================
LINK_ENTITY_TO_CLASS(ammo_rpgclip, CRpgAmmo);