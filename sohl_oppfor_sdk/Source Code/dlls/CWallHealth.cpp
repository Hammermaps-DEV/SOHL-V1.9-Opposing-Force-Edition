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
#include "monsters.h"
#include "player.h"
#include "gamerules.h"
#include "CWallHealth.h"

//-------------------------------------------------------------
// Wall mounted health kit
//-------------------------------------------------------------
TYPEDESCRIPTION CWallHealth::m_SaveData[] =
{
	DEFINE_FIELD(CWallHealth, m_flNextCharge, FIELD_TIME),
	DEFINE_FIELD(CWallHealth, m_iReactivate, FIELD_INTEGER),
	DEFINE_FIELD(CWallHealth, m_iJuice, FIELD_INTEGER),
	DEFINE_FIELD(CWallHealth, m_iOn, FIELD_INTEGER),
	DEFINE_FIELD(CWallHealth, m_flSoundTime, FIELD_TIME),
	DEFINE_FIELD(CWallHealth, m_bTriggerable, FIELD_BOOLEAN),
};
IMPLEMENT_SAVERESTORE(CWallHealth, CBaseEntity);

LINK_ENTITY_TO_CLASS(func_healthcharger, CWallHealth);

void CWallHealth::KeyValue(KeyValueData *pkvd)
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

void CWallHealth::Spawn()
{
	Precache();

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin(this, pev->origin);		// set size and link into world
	UTIL_SetSize(pev, pev->mins, pev->maxs);
	SET_MODEL(ENT(pev), STRING(pev->model));
	m_iJuice = gSkillData.healthchargerCapacity;
	m_bTriggerable = true;
	pev->frame = 0;

	//LRC
	if (m_iStyle >= 32)
		LIGHT_STYLE(m_iStyle, "a");
	else if (m_iStyle <= -32)
		LIGHT_STYLE(-m_iStyle, "z");
}

void CWallHealth::Precache()
{
	PRECACHE_SOUND("items/medshot4.wav");
	PRECACHE_SOUND("items/medshotno1.wav");
	PRECACHE_SOUND("items/medcharge4.wav");
}

void CWallHealth::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Make sure that we have a caller & if it's not a player, ignore
	if (!pActivator || !pActivator->IsPlayer())
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

	CBasePlayer *pPlayer = static_cast<CBasePlayer *>(pActivator);

	// if the player doesn't have the suit, or there is no juice left, make the deny noise
	if ((m_iJuice <= 0) || (!(pPlayer->m_iHideHUD & ITEM_SUIT)))
	{
		if (m_flSoundTime <= UTIL_GlobalTimeBase())
		{
			m_flSoundTime = UTIL_GlobalTimeBase() + 0.62;
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshotno1.wav", 1.0, ATTN_NORM);
		}

		return;
	}

	SetNextThink(0.25);
	SetThink(&CWallHealth::Off);

	// Time to recharge yet?
	if (m_flNextCharge >= UTIL_GlobalTimeBase())
		return;

	// Play the on sound or the looping charging sound
	if (!m_iOn)
	{
		m_iOn++;
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0, ATTN_NORM);
		m_flSoundTime = 0.56 + UTIL_GlobalTimeBase();
	}

	if ((m_iOn == 1) && (m_flSoundTime <= UTIL_GlobalTimeBase()))
	{
		m_iOn++;
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "items/medcharge4.wav", 1.0, ATTN_NORM);
	}

	// charge the player
	if (pActivator->TakeHealth(1, DMG_GENERIC))
		m_iJuice--;

	// govern the rate of charge
	m_flNextCharge = UTIL_GlobalTimeBase() + 0.1;
}

void CWallHealth::Recharge()
{
	EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0, ATTN_NORM);
	m_iJuice = gSkillData.healthchargerCapacity;
	m_bTriggerable = true;
	pev->frame = 0;

	//LRC
	if (m_iStyle >= 32)
		LIGHT_STYLE(m_iStyle, "a");
	else if (m_iStyle <= -32)
		LIGHT_STYLE(-m_iStyle, "z");

	SetThink(&CWallHealth::SUB_DoNothing);
}

void CWallHealth::Off()
{
	// Stop looping sound.
	if (m_iOn > 1)
		STOP_SOUND(ENT(pev), CHAN_STATIC, "items/medcharge4.wav");

	m_iOn = 0;

	if ((!m_iJuice) && ((m_iReactivate = g_pGameRules->FlHealthChargerRechargeTime()) > 0))
	{
		SetNextThink(m_iReactivate);
		SetThink(&CWallHealth::Recharge);
	}
	else
		SetThink(&CWallHealth::SUB_DoNothing);
}

STATE CWallHealth::GetState()
{
	if (m_iOn == 2)
		return STATE_IN_USE;

	if (m_iJuice)
		return STATE_ON;

	return STATE_OFF;
}