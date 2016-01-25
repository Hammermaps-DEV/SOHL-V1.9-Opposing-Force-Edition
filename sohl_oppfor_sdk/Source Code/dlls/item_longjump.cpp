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
#include "player.h"
#include "skill.h"
#include "items.h"
#include "gamerules.h"

extern int gmsgItemPickup;
extern int gEvilImpulse101;

class CItemLongJump : public CItem {
	void Spawn(void) {
		Precache();
		SET_MODEL(ENT(pev), "models/w_longjump.mdl");
		CItem::Spawn();
	}

	void Precache(void) {
		PRECACHE_MODEL("models/w_longjump.mdl");
	}

	BOOL MyTouch(CBasePlayer *pPlayer) {
		if (pPlayer->m_fLongJump) {
			return FALSE;
		}

		if (pPlayer->m_iHideHUD & ITEM_SUIT) {
			pPlayer->m_fLongJump = TRUE;// player now has longjump module

			g_engfuncs.pfnSetPhysicsKeyValue(pPlayer->edict(), "slj", "1");

			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev);
			WRITE_STRING(STRING(pev->classname));
			MESSAGE_END();

			if (!gEvilImpulse101)  // Play the longjump sound UNDONE: Kelly? correct sound?
				EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_A1");

			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_longjump, CItemLongJump);