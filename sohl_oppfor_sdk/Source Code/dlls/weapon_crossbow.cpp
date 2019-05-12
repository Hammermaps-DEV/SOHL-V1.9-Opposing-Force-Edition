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

//=========================================================
// Weapon: Crossbow * http://half-life.wikia.com/wiki/Black_Mesa_Crossbow
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "proj_bolt.h"
#include "weapon_crossbow.h"

extern bool gInfinitelyAmmo;

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_crossbow, CCrossbow);

//=========================================================
// Spawn Crossbow
//=========================================================
void CCrossbow::Spawn() {
	Precache();
	m_iId = WEAPON_CROSSBOW;

	SET_MODEL(ENT(pev), "models/w_crossbow.mdl");
	m_iDefaultAmmo = CROSSBOW_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CCrossbow::Precache() {
	PRECACHE_MODEL("models/w_crossbow.mdl");
	PRECACHE_MODEL("models/v_crossbow.mdl");
	PRECACHE_MODEL("models/p_crossbow.mdl");

	PRECACHE_SOUND("weapons/xbow_fire1.wav");
	PRECACHE_SOUND("weapons/xbow_reload1.wav");
	PRECACHE_SOUND("weapons/reload2.wav");
	PRECACHE_SOUND("weapons/reload3.wav");

	UTIL_PrecacheOther("crossbow_bolt");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CCrossbow::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "bolts";
	p->iMaxAmmo1 = BOLT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = CROSSBOW_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 2;
	p->iId = WEAPON_CROSSBOW;
	p->iFlags = 0;
	p->iWeight = CROSSBOW_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CCrossbow::PrimaryAttack() {
	if (m_iChargeLevel && IsMultiplayer()) {
		FireSniperBolt(); //MP
	}
	else {
		FireBolt(); //SP
	}
}

//=========================================================
// SecondaryAttack * Only in "weapon_crossbow.h"
//=========================================================

//=========================================================
// FireSniperBolt * only in Multiplayer
//=========================================================
void CCrossbow::FireSniperBolt() {
	m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.75;

	if (m_iClip == 0) {
		PlayEmptySound();
		return;
	}

	TraceResult tr;

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

	if(!gInfinitelyAmmo)
		m_iClip--;

	// make twang sound
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/xbow_fire1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0, 0xF));

	if (m_iClip)
	{
		SendWeaponAnim((int)CROSSBOW_FIRE::sequence);
		m_iBody++;
	}
	else
		SendWeaponAnim((int)CROSSBOW_FIRE_LAST::sequence);

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors(anglesAim);
	Vector vecSrc = m_pPlayer->GetGunPosition() - gpGlobals->v_up * 2;
	Vector vecDir = gpGlobals->v_forward;

	UTIL_TraceLine(vecSrc, vecSrc + vecDir * 8192, dont_ignore_monsters, m_pPlayer->edict(), &tr);

	if (tr.pHit->v.takedamage)
	{
		switch (RANDOM_LONG(0, 1))
		{
		case 0: EMIT_SOUND(tr.pHit, CHAN_BODY, "weapons/xbow_hitbod1.wav", VOL_NORM, ATTN_NORM); break;
		case 1: EMIT_SOUND(tr.pHit, CHAN_BODY, "weapons/xbow_hitbod2.wav", VOL_NORM, ATTN_NORM); break;
		}

		ClearMultiDamage();
		Instance(tr.pHit)->TraceAttack(m_pPlayer->pev, 120, vecDir, &tr, DMG_BULLET | DMG_NEVERGIB);
		ApplyMultiDamage(pev, m_pPlayer->pev);
	}
	else {
		// create a bolt
		CBolt *pBolt = CBolt::BoltCreate();
		pBolt->pev->origin = tr.vecEndPos - vecDir * 10;
		pBolt->pev->angles = UTIL_VecToAngles(vecDir);
		pBolt->pev->solid = SOLID_NOT;
		pBolt->SetTouch(NULL);
		pBolt->SetThink(&CBolt::SUB_Remove);

		EMIT_SOUND(pBolt->edict(), CHAN_WEAPON, "weapons/xbow_hit1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM);

		if (UTIL_PointContents(tr.vecEndPos) != CONTENTS_WATER) {
			UTIL_Sparks(tr.vecEndPos);
		}

		if (FClassnameIs(tr.pHit, "worldspawn")) {
			// let the bolt sit around for a while if it hit static architecture
			pBolt->pev->nextthink = UTIL_GlobalTimeBase() + 5.0;
		}
		else {
			pBolt->pev->nextthink = UTIL_GlobalTimeBase();
		}
	}
}

//=========================================================
// FireBolt * only in Singeplayer
//=========================================================
void CCrossbow::FireBolt() {
	
	if (m_iClip == 0) {
		PlayEmptySound();
		return;
	}

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

	if (!gInfinitelyAmmo)
		m_iClip--;

	// make twang sound
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/xbow_fire1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0, 0xF));

	if (m_iClip) {
		SendWeaponAnim((int)CROSSBOW_FIRE::sequence);
		m_iBody++;
	}
	else
		SendWeaponAnim((int)CROSSBOW_FIRE_LAST::sequence);

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors(anglesAim);

	anglesAim.x = -anglesAim.x;
	Vector vecSrc = m_pPlayer->GetGunPosition() - gpGlobals->v_up * 2;
	Vector vecDir = gpGlobals->v_forward;

	CBolt *pBolt = CBolt::BoltCreate();
	pBolt->pev->origin = vecSrc;
	pBolt->pev->angles = anglesAim;
	pBolt->pev->owner = m_pPlayer->edict();

	if (m_pPlayer->pev->waterlevel == 3 && m_pPlayer->pev->watertype > CONTENT_FLYFIELD) {
		pBolt->pev->velocity = vecDir * BOLT_WATER_VELOCITY;
		pBolt->pev->speed = BOLT_WATER_VELOCITY;
	}
	else {
		pBolt->pev->velocity = vecDir * BOLT_AIR_VELOCITY;
		pBolt->pev->speed = BOLT_AIR_VELOCITY;
	}
	pBolt->pev->avelocity.z = 10;

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}

	if (m_iClip >= 2) {
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)CROSSBOW_FIRE::frames, (int)CROSSBOW_FIRE::fps);
	}
	else {
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)CROSSBOW_FIRE_LAST::frames, (int)CROSSBOW_FIRE_LAST::fps);
	}

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle;
	m_pPlayer->m_flNextAttack = m_flTimeWeaponIdle;
}

//=========================================================
// Deploy
//=========================================================
BOOL CCrossbow::Deploy() {
	if (m_iClip) {
		return DefaultDeploy("models/v_crossbow.mdl", "models/p_crossbow.mdl", (int)CROSSBOW_DRAW::sequence,
			"bow", CalculateWeaponTime((int)CROSSBOW_DRAW::frames, (int)CROSSBOW_DRAW::fps));
	}
	else {
		return DefaultDeploy("models/v_crossbow.mdl", "models/p_crossbow.mdl", (int)CROSSBOW_DRAW_EMPTY::sequence,
			"bow", CalculateWeaponTime((int)CROSSBOW_DRAW_EMPTY::frames, (int)CROSSBOW_DRAW_EMPTY::fps));
	}
}

//=========================================================
// Holster
//=========================================================
void CCrossbow::Holster() {
	m_fInReload = FALSE;// cancel any reload in progress.
	ZoomReset();

	if (m_iClip) {
		SendWeaponAnim((int)CROSSBOW_HOLSTER::sequence);
		m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)CROSSBOW_HOLSTER::frames, (int)CROSSBOW_HOLSTER::fps);
	}
	else {
		SendWeaponAnim((int)CROSSBOW_HOLSTER_EMPTY::sequence);
		m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)CROSSBOW_HOLSTER_EMPTY::frames, (int)CROSSBOW_HOLSTER_EMPTY::fps);
	}
}

//=========================================================
// Reload
//=========================================================
void CCrossbow::Reload() {
	if (m_iChargeLevel) {
		ZoomReset();
	}

	if (m_iClip) { return; }

	DefaultReload(CROSSBOW_MAX_CLIP, (int)CROSSBOW_RELOAD::sequence,
		CalculateWeaponTime((int)CROSSBOW_RELOAD::frames, (int)CROSSBOW_RELOAD::fps));
}

//=========================================================
// ZoomUpdate
//=========================================================
void CCrossbow::ZoomUpdate() {
	if (m_pPlayer->pev->button & IN_ATTACK2) {
		if (m_iChargeLevel == 0) {
			if (m_flShockTime > UTIL_GlobalTimeBase()) return;
			m_iChargeLevel = 1;
			m_flTimeUpdate = UTIL_GlobalTimeBase() + 0.5;
		}

		if (m_iChargeLevel == 1) {
			m_pPlayer->m_iFOV = 50;
			m_iChargeLevel = 2;//ready to zooming, wait for 0.5 secs
		}

		if (m_flTimeUpdate > UTIL_GlobalTimeBase()) {
			return;
		}

		if (m_iChargeLevel == 2 && m_pPlayer->m_iFOV > 20) {
			m_pPlayer->m_iFOV--;
			m_flTimeUpdate = UTIL_GlobalTimeBase() + 0.02;
		}

		if (m_iChargeLevel == 3) {
			ZoomReset();
		}
	}
	else if (m_iChargeLevel > 1) {
		m_iChargeLevel = 3;
	}
}

//=========================================================
// ZoomReset
//=========================================================
void CCrossbow::ZoomReset() {
	m_flShockTime = UTIL_GlobalTimeBase() + 0.5;
	m_pPlayer->m_iFOV = 90;
	m_iChargeLevel = 0;//clear zoom
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CCrossbow::WeaponIdle() {
	ZoomUpdate();
	ResetEmptySound();

	//Update Bodys
	switch (m_iClip) {
	case 1: pev->body = 4; break;
	case 2: pev->body = 3; break;
	case 3: pev->body = 2; break;
	case 4: pev->body = 1; break;
	case 5: pev->body = 0; break;
	}

	if (m_iChargeLevel != 0) {
		return;
	}

	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
		return;
	}

	int iAnim;
	float flRand = RANDOM_FLOAT(0, 1);
	if (flRand <= 0.5) {
		if (m_iClip) {
			iAnim = (int)CROSSBOW_IDLE::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)CROSSBOW_IDLE::frames, (int)CROSSBOW_IDLE::fps);
		}
		else {
			iAnim = (int)CROSSBOW_IDLE_EMPTY::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)CROSSBOW_IDLE_EMPTY::frames, (int)CROSSBOW_IDLE_EMPTY::fps);
		}
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	}
	else if (flRand <= 0.7 && m_iClip) {
		iAnim = (int)CROSSBOW_FIDGET::sequence;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)CROSSBOW_FIDGET::frames, (int)CROSSBOW_FIDGET::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	}
	else {
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
		m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
	}

	SendWeaponAnim(iAnim);
}