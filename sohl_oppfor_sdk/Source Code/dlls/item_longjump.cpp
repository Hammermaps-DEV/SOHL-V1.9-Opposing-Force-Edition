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
#include "player.h"
#include "items.h"

extern int gmsgItemPickup;
extern bool gEvilImpulse101;

class CItemLongJump : public CItem
{
	void Spawn() override
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_longjump.mdl");
		CItem::Spawn();
	}
	void Precache() override
	{
		PRECACHE_MODEL("models/w_longjump.mdl");
	}

	bool MyTouch(CBasePlayer *pPlayer) override
	{
		if (!pPlayer->IsPlayer() || pPlayer->m_fLongJump || pPlayer->pev->deadflag != DEAD_NO)
			return false;

		if (pPlayer->m_iHideHUD & ITEM_SUIT)
		{
			pPlayer->m_fLongJump = TRUE;// player now has longjump module

			g_engfuncs.pfnSetPhysicsKeyValue(pPlayer->edict(), "slj", "1");

			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev);
			WRITE_STRING(STRING(pev->classname));
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ONE, gmsgInventory, NULL, pPlayer->pev);//AJH msg change inventory
			WRITE_SHORT((ITEM_LONGJUMP));						//which item to change
			WRITE_SHORT(1);										//set counter to this ammount
			MESSAGE_END();

			if (!gEvilImpulse101)  // Play the longjump sound UNDONE: Kelly? correct sound?
				EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_A1");

			return true;
		}

		return false;
	}
};

LINK_ENTITY_TO_CLASS(item_longjump, CItemLongJump);