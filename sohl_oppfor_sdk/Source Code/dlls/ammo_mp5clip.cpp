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

class CMP5AmmoClip : public CBasePlayerAmmo {
	//=========================================================
	// Spawn
	//=========================================================
	void Spawn(void) {
		Precache();
		SET_MODEL(ENT(pev), "models/w_9mmARclip.mdl");
		CBasePlayerAmmo::Spawn();
	}

	//=========================================================
	// Precache
	//=========================================================
	void Precache(void) {
		PRECACHE_MODEL("models/w_9mmARclip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}

	//=========================================================
	// AddAmmo
	//=========================================================
	BOOL AddAmmo(CBaseEntity *pOther) {
		int bResult = (pOther->GiveAmmo(AMMO_MP5CLIP_GIVE, "9mm", _9MM_MAX_CARRY) != -1);
		if (bResult) {
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}

		return bResult;
	}
};

//=========================================================
// Link entity to Class
//=========================================================
LINK_ENTITY_TO_CLASS(ammo_mp5clip, CMP5AmmoClip);
LINK_ENTITY_TO_CLASS(ammo_9mmAR, CMP5AmmoClip);