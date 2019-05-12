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
#include "weapons.h"
#include "monsters.h"

float AmmoDamage(const char *pName)
{
	if (!pName)
		return 0;

	if (!strcmp(pName, "9mm"))
		return gSkillData.plrDmg9MM;
	if (!strcmp(pName, "357"))
		return gSkillData.plrDmg357;
	if (!strcmp(pName, "762"))
		return gSkillData.plrDmg762;
	if (!strcmp(pName, "ARgrenades"))
		return gSkillData.plrDmgM203Grenade;
	if (!strcmp(pName, "buckshot"))
		return gSkillData.plrDmgBuckshot;
	if (!strcmp(pName, "bolts"))
		return gSkillData.plrDmgCrossbowMonster;
	if (!strcmp(pName, "rockets"))
		return gSkillData.plrDmgRPG;
	if (!strcmp(pName, "uranium"))
		return gSkillData.plrDmgGauss;
	if (!strcmp(pName, "Hand Grenade"))
		return gSkillData.plrDmgHandGrenade;
	if (!strcmp(pName, "Satchel Charge"))
		return gSkillData.plrDmgSatchel;
	if (!strcmp(pName, "Trip Mine"))
		return gSkillData.plrDmgTripmine;
	if (!strcmp(pName, "shocks"))
		return gSkillData.plrDmgShock;
	if (!strcmp(pName, "spore"))
		return gSkillData.plrDmgSpore;

	return 0;
}


void UpdateStatsFile(float dataTime, char *pMapname, float health, float ammo, int skillLevel)
{
	FILE *fp;

	fp = fopen("stats.txt", "a");
	if (!fp)
		return;
	fprintf(fp, "%6.2f, %6.2f, %6.2f, %s, %2d\n", dataTime, health, ammo, pMapname, skillLevel);
	fclose(fp);
}


#define AMMO_THRESHOLD		10			// This much ammo goes by before it is "interesting"
#define HEALTH_THRESHOLD	10			// Same for health
#define OUTPUT_LATENCY		3			// This many seconds for ammo/health to settle

typedef struct
{
	int		lastAmmo;
	float	lastHealth;
	float	lastOutputTime; // NOTE: These times are in "game" time -- a running total of elapsed time since the game started
	float	nextOutputTime;
	float	dataTime;
	float	gameTime;
	float	lastGameTime;
} TESTSTATS;

TESTSTATS gStats = { 0,0,0,0,0,0,0 };

void UpdateStats(CBasePlayer *pPlayer)
{
	int i;

	int ammoCount[MAX_AMMO_SLOTS];
	memcpy(ammoCount, pPlayer->m_rgAmmo, MAX_AMMO_SLOTS * sizeof(int));

	// Keep a running time, so the graph doesn't overlap

	if (UTIL_GlobalTimeBase() < gStats.lastGameTime)	// Changed level or died, don't b0rk
	{
		gStats.lastGameTime = UTIL_GlobalTimeBase();
		gStats.dataTime = gStats.gameTime;
	}

	gStats.gameTime += UTIL_GlobalTimeBase() - gStats.lastGameTime;
	gStats.lastGameTime = UTIL_GlobalTimeBase();

	for (i = 0; i < MAX_ITEM_TYPES; i++)
	{
		CBasePlayerItem *p = pPlayer->m_rgpPlayerItems[i];
		while (p)
		{
			ItemInfo II;

			memset(&II, 0, sizeof(II));
			p->GetItemInfo(&II);

			int index = pPlayer->GetAmmoIndex(II.pszAmmo1);
			if (index >= 0)
				ammoCount[index] += ((CBasePlayerWeapon *)p)->m_iClip;

			p = p->m_pNext;
		}
	}

	float ammo = 0;
	for (i = 1; i < MAX_AMMO_SLOTS; i++)
	{
		ammo += ammoCount[i] * AmmoDamage(CBasePlayerItem::AmmoInfoArray[i].pszName);
	}

	float health = pPlayer->pev->health + pPlayer->pev->armorvalue * 2;	// Armor is 2X health
	float ammoDelta = fabs(ammo - gStats.lastAmmo);
	float healthDelta = fabs(health - gStats.lastHealth);
	int forceWrite = 0;
	if (health <= 0 && gStats.lastHealth > 0)
		forceWrite = 1;

	if ((ammoDelta > AMMO_THRESHOLD || healthDelta > HEALTH_THRESHOLD) && !forceWrite)
	{
		if (gStats.nextOutputTime == 0)
			gStats.dataTime = gStats.gameTime;

		gStats.lastAmmo = ammo;
		gStats.lastHealth = health;

		gStats.nextOutputTime = gStats.gameTime + OUTPUT_LATENCY;
	}
	else if ((gStats.nextOutputTime != 0 && gStats.nextOutputTime < gStats.gameTime) || forceWrite)
	{
		UpdateStatsFile(gStats.dataTime, (char *)STRING(gpGlobals->mapname), health, ammo, (int)CVAR_GET_FLOAT("skill"));

		gStats.lastAmmo = ammo;
		gStats.lastHealth = health;
		gStats.lastOutputTime = gStats.gameTime;
		gStats.nextOutputTime = 0;
	}
}

void InitStats(CBasePlayer *pPlayer)
{
	gStats.lastGameTime = UTIL_GlobalTimeBase();	// Fixup stats time
}

