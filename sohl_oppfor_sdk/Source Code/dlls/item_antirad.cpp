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
#include "player.h"
#include "items.h"

void CItemAntiRad::Spawn(void)
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_rad.mdl");
	CItem::Spawn();
}

void CItemAntiRad::Precache(void)
{
	PRECACHE_MODEL("models/w_rad.mdl");
}

BOOL CItemAntiRad::MyTouch(CBasePlayer *pPlayer)
{
	pPlayer->SetSuitUpdate("!HEV_DET5", FALSE, SUIT_NEXT_IN_1MIN); //TODO: find right suit notifcation

	pPlayer->m_rgItems[ITEM_ANTIRAD] += 1;
	MESSAGE_BEGIN(MSG_ONE, gmsgInventory, NULL, pPlayer->pev);//AJH msg change inventory
	WRITE_SHORT((ITEM_ANTIRAD));						//which item to change
	WRITE_SHORT(pPlayer->m_rgItems[ITEM_ANTIRAD]);		//set counter to this ammount
	MESSAGE_END();
	if (pev->noise)	//AJH
		EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, STRING(pev->noise), 1, ATTN_NORM);
	else
		EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM);

	return TRUE;
}

void CItemAntiRad::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{

	if (!(pActivator->IsPlayer())) {
		ALERT(at_debug, "DEBUG: AntiRad syringe used by non-player\n");
		return;
	}

	CBasePlayer* m_hActivator = (CBasePlayer*)pActivator;
	ALERT(at_console, "HazardSuit: AntiRad shots remaining: %i\n", m_hActivator->m_rgItems[ITEM_ANTIRAD]);
}
LINK_ENTITY_TO_CLASS(item_antirad, CItemAntiRad);