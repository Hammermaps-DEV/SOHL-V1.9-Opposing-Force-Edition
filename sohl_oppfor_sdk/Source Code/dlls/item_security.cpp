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
#include "items.h"

class CItemSecurity : public CItem
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_security.mdl");
		CItem::Spawn();
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_security.mdl");
	}
	BOOL MyTouch(CBasePlayer *pPlayer)
	{
		pPlayer->m_rgItems[ITEM_SECURITY] += 1;		//AJH implement a new system with different cards instead of just MORE cards

		MESSAGE_BEGIN(MSG_ONE, gmsgInventory, NULL, pPlayer->pev);	//AJH msg change inventory
		WRITE_SHORT((ITEM_SECURITY));						//which item to change
		WRITE_SHORT(pPlayer->m_rgItems[ITEM_SECURITY]);		//set counter to this ammount
		MESSAGE_END();

		if (pev->noise)	//AJH
			EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, STRING(pev->noise), 1, ATTN_NORM);
		else
			EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM);

		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_security, CItemSecurity);