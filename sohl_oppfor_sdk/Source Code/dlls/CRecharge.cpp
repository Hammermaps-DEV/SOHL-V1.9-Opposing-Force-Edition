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
/*

===== CRecharge.cpp ========================================================

  battery-related code

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "skill.h"
#include "gamerules.h"
#include "player.h"
#include "CRecharge.h"

TYPEDESCRIPTION CRecharge::m_SaveData[] =
{
	DEFINE_FIELD(CRecharge, m_flNextCharge, FIELD_TIME),
	DEFINE_FIELD(CRecharge, m_iReactivate, FIELD_INTEGER),
	DEFINE_FIELD(CRecharge, m_iJuice, FIELD_INTEGER),
	DEFINE_FIELD(CRecharge, m_iOn, FIELD_INTEGER),
	DEFINE_FIELD(CRecharge, m_flSoundTime, FIELD_TIME),
	DEFINE_FIELD(CRecharge, m_bTriggerable, FIELD_BOOLEAN),
};

IMPLEMENT_SAVERESTORE(CRecharge, CBaseEntity);
LINK_ENTITY_TO_CLASS(func_recharge, CRecharge);

void CRecharge::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "style") ||
		FStrEq(pkvd->szKeyName, "height") ||
		FStrEq(pkvd->szKeyName, "value1") ||
		FStrEq(pkvd->szKeyName, "value2") ||
		FStrEq(pkvd->szKeyName, "value3"))
	{
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "dmdelay"))
	{
		m_iReactivate = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "target"))
	{
		ALERT(at_console, "Healthcharger: has target = %s\n", pkvd->szValue);
		strcpy(m_szTarget, pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue(pkvd);
}

void CRecharge::Spawn()
{
	Precache();

	SetSolidType(SOLID_BSP);
	SetMoveType(MOVETYPE_PUSH);

	UTIL_SetOrigin(this, pev->origin);		// set size and link into world
	UTIL_SetSize(pev, pev->mins, pev->maxs);
	SET_MODEL(ENT(pev), STRING(pev->model));
	m_iJuice = gSkillData.suitchargerCapacity;
	m_bTriggerable = TRUE;
	pev->frame = 0;

	//LRC
	if (m_iStyle >= 32) 
		LIGHT_STYLE(m_iStyle, "a");
	else if (m_iStyle <= -32) 
		LIGHT_STYLE(-m_iStyle, "z");
}

void CRecharge::Precache()
{
	PRECACHE_SOUND("items/suitcharge1.wav");
	PRECACHE_SOUND("items/suitchargeno1.wav");
	PRECACHE_SOUND("items/suitchargeok1.wav");
}

void CRecharge::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// if it's not a player, ignore
	if (!FClassnameIs(pActivator->pev, "player"))
		return;

	// if there is no juice left, turn it off
	if (m_iJuice <= 0)
	{
		if (m_bTriggerable)
		{
			FireTargets(STRING(pev->target), pActivator, this, USE_TOGGLE, 0);
			m_bTriggerable = false;
		}

		pev->frame = 1;

		//LRC
		if (m_iStyle >= 32) 
			LIGHT_STYLE(m_iStyle, "z");
		else if (m_iStyle <= -32) 
			LIGHT_STYLE(-m_iStyle, "a");

		Off();
	}

	CBasePlayer *pPlayer = (CBasePlayer *)pActivator;

	// if the player doesn't have the suit, or there is no juice left, make the deny noise
	if ((m_iJuice <= 0) || (!(pPlayer->m_iHideHUD & ITEM_SUIT)))
	{
		if (m_flSoundTime <= UTIL_GlobalTimeBase())
		{
			m_flSoundTime = UTIL_GlobalTimeBase() + 0.62;
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/suitchargeno1.wav", VOL_LOW, ATTN_NORM);
		}
		return;
	}

	SetNextThink(0.25);
	SetThink(&CRecharge::Off);

	// Time to recharge yet?
	if (m_flNextCharge >= UTIL_GlobalTimeBase())
		return;

	// Make sure that we have a caller
	if (!pActivator)
		return;

	m_hActivator = pActivator;

	//only recharge the player
	if (!m_hActivator->IsPlayer())
		return;

	// Play the on sound or the looping charging sound
	if (!m_iOn)
	{
		m_iOn++;
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/suitchargeok1.wav", VOL_LOW, ATTN_NORM);
		m_flSoundTime = 0.56 + UTIL_GlobalTimeBase();
	}

	if ((m_iOn == 1) && (m_flSoundTime <= UTIL_GlobalTimeBase()))
	{
		m_iOn++;
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "items/suitcharge1.wav", VOL_LOW, ATTN_NORM);
	}

	// charge the player
	if (m_hActivator->TakeArmor(1))
	{
		m_iJuice--;
		m_hActivator->pev->armorvalue += 1;

		if (m_hActivator->pev->armorvalue > 100)
			m_hActivator->pev->armorvalue = 100;
	}

	// govern the rate of charge
	m_flNextCharge = UTIL_GlobalTimeBase() + 0.1;
}

void CRecharge::Recharge()
{
	m_iJuice = gSkillData.suitchargerCapacity;
	m_bTriggerable = true;
	pev->frame = 0;

	//LRC
	if (m_iStyle >= 32) 
		LIGHT_STYLE(m_iStyle, "a");
	else if (m_iStyle <= -32) 
		LIGHT_STYLE(-m_iStyle, "z");

	SetThink(&CRecharge::SUB_DoNothing);
}

void CRecharge::Off()
{
	// Stop looping sound.
	if (m_iOn > 1)
		STOP_SOUND(ENT(pev), CHAN_STATIC, "items/suitcharge1.wav");

	m_iOn = 0;

	if ((!m_iJuice) && ((m_iReactivate = g_pGameRules->FlHEVChargerRechargeTime()) > 0))
	{
		SetNextThink(m_iReactivate);
		SetThink(&CRecharge::Recharge);
	}
	else
		SetThink(&CRecharge::SUB_DoNothing);
}

STATE CRecharge::GetState()
{
	if (m_iOn == 2)
		return STATE_IN_USE;

	if (m_iJuice)
		return STATE_ON;

	return STATE_OFF;
}