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
#include "player.h"
#include "skill.h"
#include "items.h"

extern int gmsgItemPickup;
extern int gEvilImpulse101;

#define SF_SUIT_SHORTLOGON		0x0001

class CItemSuit : public CItem
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_suit.mdl");
		CItem::Spawn();
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_suit.mdl");
	}
	BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->pev->deadflag != DEAD_NO)
		{
			return FALSE;
		}

		if (pPlayer->pev->weapons & (1 << WEAPON_SUIT))
			return FALSE;

		if (pev->spawnflags & SF_SUIT_SHORTLOGON)
			EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_A0");		// short version of suit logon,
		else
			EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_AAx");	// long version of suit logon

		pPlayer->pev->weapons |= (1 << WEAPON_SUIT);

		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_suit, CItemSuit);