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
#include "gamerules.h"

extern int gmsgItemPickup;
extern bool gEvilImpulse101;

#define CHARGE_IN_MEDKIT 50	//Possibly implement skill system
#define MAX_MEDKIT 200 //possibly allow mapper to change, say in worldspawn


#define ITEM_NOTPICKEDUP 0
#define ITEM_PICKEDUP 1
#define ITEM_DRAINED 2	//The item has had some 'charge' removed but remains in existence

void CItemMedicalKit::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_portablemed.mdl");	// create a new model and spawn it here
	pev->dmg = pev->health; //Store initial charge

	SetMoveType(MOVETYPE_TOSS);
	SetSolidType(SOLID_TRIGGER);
	UTIL_SetOrigin(this, pev->origin);
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	SetTouch(&CItemMedicalKit::ItemTouch);

	if (DROP_TO_FLOOR(ENT(pev)) == 0)
	{
		ALERT(at_error, "Item %s fell out of level at %f,%f,%f", GetClassname(), pev->origin.x, pev->origin.y, pev->origin.z);
		UTIL_Remove(this);
	}
}

void CItemMedicalKit::Precache()
{
	PRECACHE_MODEL("models/w_portablemed.mdl");	// create a new model and precache it here
	PRECACHE_SOUND("items/smallmedkit1.wav");
}

bool CItemMedicalKit::MyTouch(CBasePlayer *pPlayer)
{
	if (pPlayer->IsPlayer() || 
		pPlayer->pev->deadflag != DEAD_NO || 
		pPlayer->m_rgItems[ITEM_HEALTHKIT] >= (int)CVAR_GET_FLOAT("max_medkit")) {
		return false;
	}

	pPlayer->m_rgItems[ITEM_HEALTHKIT] += (pev->health) ? pev->health : CHARGE_IN_MEDKIT;//increment/decrement counter by this ammount
	MESSAGE_BEGIN(MSG_ONE, gmsgInventory, NULL, pPlayer->pev);	//msg change inventory
	WRITE_SHORT((ITEM_HEALTHKIT));							//which item to change
	WRITE_SHORT(pPlayer->m_rgItems[ITEM_HEALTHKIT]);		//set counter to this ammount
	MESSAGE_END();

	if (pPlayer->m_rgItems[ITEM_HEALTHKIT] > MAX_MEDKIT)// We have more 'charge' than the player is allowed to have
	{
		pev->health = pPlayer->m_rgItems[ITEM_HEALTHKIT] - MAX_MEDKIT; //set the amount of charge left in the kit to be the difference
		pPlayer->m_rgItems[ITEM_HEALTHKIT] = MAX_MEDKIT;//set players kit charge to max
		return true;
	}

	if (g_pGameRules->ItemShouldRespawn(this))
	{
		pev->health = pev->dmg; //Reset initial health;
		Respawn();
	}
	else
	{
		SetTouch(NULL); //Is this necessary?
		UTIL_Remove(this);
	}

	return true;
}

void CItemMedicalKit::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{

	if (!(pActivator->IsPlayer())) {
		ALERT(at_debug, "DEBUG: Medical kit used by non-player\n");
		return;
	}

	EMIT_SOUND(pActivator->edict(), CHAN_ITEM, "items/smallmedkit1.wav", 1, ATTN_NORM); //play sound to tell player it's been used

	auto m_hActivator = static_cast<CBasePlayer*>(pActivator);
	int m_fHealthUsed = (int)m_hActivator->TakeHealth(m_hActivator->m_rgItems[ITEM_HEALTHKIT], DMG_GENERIC); //Actually give the health
	m_hActivator->m_rgItems[ITEM_HEALTHKIT] -= m_fHealthUsed;//increment/decrement counter by this ammount

	MESSAGE_BEGIN(MSG_ONE, gmsgInventory, NULL, m_hActivator->pev);	//msg change inventory
	WRITE_SHORT((ITEM_HEALTHKIT));								//which item to change
	WRITE_SHORT(m_hActivator->m_rgItems[ITEM_HEALTHKIT]);			//set counter to this ammount
	MESSAGE_END();

	ALERT(at_console, "AutoMedicalKit: I have healed %i health\n", m_fHealthUsed);
	ALERT(at_console, "AutoMedicalKit: Charge remaining for healing: %i\n", m_hActivator->m_rgItems[ITEM_HEALTHKIT]);
}

void CItemMedicalKit::ItemTouch(CBaseEntity *pOther)
{
	// if it's not a player, ignore
	if (!pOther->IsPlayer())
		return;

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;

	// ok, a player is touching this item, but can he have it?
	if (!g_pGameRules->CanHaveItem(pPlayer, this))
	{
		// no? Ignore the touch.
		return;
	}

	if (MyTouch(pPlayer))
	{
		if (pev->noise)
			EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, STRING(pev->noise), 1, ATTN_NORM);
		else
			EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM);

		SUB_UseTargets(pOther, USE_TOGGLE, 0);

		// player grabbed the item. 
		g_pGameRules->PlayerGotItem(pPlayer, this);

	}
}

LINK_ENTITY_TO_CLASS(item_medicalkit, CItemMedicalKit);