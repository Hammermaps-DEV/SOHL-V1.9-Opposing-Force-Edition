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
// Weapon: Spore Launcher * http://half-life.wikia.com/wiki/Spore_Launcher
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "proj_sporegrenade.h"
#include "weapon_sporelauncher.h"

extern bool gInfinitelyAmmo;

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_sporelauncher, CSporelauncher);

//=========================================================
// Spawn Spore Launcher
//=========================================================
void CSporelauncher::Spawn() {
	Precache();

	SET_MODEL(ENT(pev), "models/w_spore_launcher.mdl");
	m_iDefaultAmmo = SPORELAUNCHER_DEFAULT_GIVE;

	pev->sequence = 0;
	pev->animtime = UTIL_GlobalTimeBase() + 0.1;
	pev->framerate = 1.0;

	FallInit();// get ready to fall
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CSporelauncher::Precache() {
	PRECACHE_MODEL("models/v_spore_launcher.mdl");
	PRECACHE_MODEL("models/w_spore_launcher.mdl");
	PRECACHE_MODEL("models/p_spore_launcher.mdl");

	PRECACHE_MODEL("sprites/tinyspit.spr");

	PRECACHE_SOUND("weapons/splauncher_altfire.wav");
	PRECACHE_SOUND("weapons/splauncher_bounce.wav");
	PRECACHE_SOUND("weapons/splauncher_fire.wav");
	PRECACHE_SOUND("weapons/splauncher_impact.wav");
	PRECACHE_SOUND("weapons/splauncher_pet.wav");
	PRECACHE_SOUND("weapons/splauncher_reload.wav");

	UTIL_PrecacheOther("spore");

	m_usSporeFire = PRECACHE_EVENT(1, "events/spore.sc");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CSporelauncher::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "spore";
	p->iMaxAmmo1 = SPORE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SPORELAUNCHER_MAX_CLIP;
	p->iSlot = 6;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SPORELAUNCHER;
	p->iWeight = SPORELAUNCHER_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CSporelauncher::PrimaryAttack() {
	if (m_iClip == 0) {
		PlayEmptySound();
		Reload();
		return;
	}

	if (m_iChargeLevel >= 1) {
		Charge(true);
	}
	else {
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

		if(!gInfinitelyAmmo)
			m_iClip--;

		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
		Vector vecDir, vecVel;

		UTIL_MakeVectors(m_pPlayer->pev->v_angle);

		vecSrc = vecSrc + gpGlobals->v_forward * 16;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		vecSrc = vecSrc + gpGlobals->v_up * -12;

		vecVel = gpGlobals->v_forward * 800;
		vecDir = gpGlobals->v_forward + gpGlobals->v_right + gpGlobals->v_up;
		vecDir = vecDir;

		CSporeGrenade::ShootContact(m_pPlayer->pev, vecSrc, vecVel);

		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usSporeFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, true);

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
			// HEV suit - indicate out of ammo condition
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)SPLAUNCHER_SHOOT::frames, (int)SPLAUNCHER_SHOOT::fps);

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
		m_iChargeLevel = 0;
	}
}

//=========================================================
// SecondaryAttack
//=========================================================
void CSporelauncher::SecondaryAttack() {
	if (m_iClip == 0) {
		PlayEmptySound();
		Reload();
		return;
	}

	if (m_iChargeLevel >= 1) {
		Charge(true);
	}
	else {
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

		if(!gInfinitelyAmmo)
			m_iClip--;

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_2DEGREES);
		Vector vecDir, vecVel;

		UTIL_MakeVectors(m_pPlayer->pev->v_angle);

		vecSrc = vecSrc + gpGlobals->v_forward * 16;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		vecSrc = vecSrc + gpGlobals->v_up * -12;

		vecVel = gpGlobals->v_forward * 800;
		vecDir = gpGlobals->v_forward + gpGlobals->v_right + gpGlobals->v_up;
		vecDir = vecDir;

		CSporeGrenade::ShootTimed(m_pPlayer->pev, vecSrc, vecVel, RANDOM_FLOAT(3, 5));

		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usSporeFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, false);

		if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
			// HEV suit - indicate out of ammo condition
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
		}

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)SPLAUNCHER_SHOOT::frames, (int)SPLAUNCHER_SHOOT::fps);

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
		m_iChargeLevel = 0;
	}
}

//=========================================================
// Deploy
//=========================================================
BOOL CSporelauncher::Deploy() {
	return DefaultDeploy("models/v_spore_launcher.mdl", "models/p_spore_launcher.mdl", (int)SPLAUNCHER_DRAW::sequence,
		"shotgun", CalculateWeaponTime((int)SPLAUNCHER_DRAW::frames, (int)SPLAUNCHER_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CSporelauncher::Holster() {
	m_fInReload = FALSE;// cancel any reload in progress.
	SendWeaponAnim((int)SPLAUNCHER_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)SPLAUNCHER_HOLSTER::frames, (int)SPLAUNCHER_HOLSTER::fps);
}

//=========================================================
// Reload
//=========================================================
void CSporelauncher::Reload() {
	if ((m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == SPORELAUNCHER_MAX_CLIP) ||
		m_flNextPrimaryAttack > UTIL_GlobalTimeBase() || m_flNextSecondaryAttack > UTIL_GlobalTimeBase()) {
		return;
	}
	else { Charge(false); }
}

//=========================================================
// Reload Charge
//=========================================================
void CSporelauncher::Charge(bool m_BeginAttack) {
	if ((m_iClip != SPORELAUNCHER_MAX_CLIP && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]) && !m_BeginAttack) {
		if (m_iChargeLevel == 0) {
			SendWeaponAnim((int)SPLAUNCHER_RELOAD_REACH::sequence);
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SPLAUNCHER_RELOAD_REACH::frames, (int)SPLAUNCHER_RELOAD_REACH::fps);
			m_pPlayer->m_flNextAttack = m_flTimeWeaponIdle;
			m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + m_flTimeWeaponIdle;
			m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + m_flTimeWeaponIdle;
			m_iChargeLevel = 1;
		}
		else if (m_iChargeLevel == 1) {
			if (m_flTimeWeaponIdle < UTIL_GlobalTimeBase()) {
				m_iChargeLevel = 2;

				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/splauncher_reload.wav", VOL_NORM, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));

				SendWeaponAnim((int)SPLAUNCHER_RELOAD_LOAD::sequence);
				m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
					CalculateWeaponTime((int)SPLAUNCHER_RELOAD_LOAD::frames, (int)SPLAUNCHER_RELOAD_LOAD::fps);
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle;
				m_pPlayer->m_flNextAttack = m_flTimeWeaponIdle;
			}
		}
		else {
			if (m_flTimeWeaponIdle < UTIL_GlobalTimeBase()) {
				m_iClip += 1;
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
				m_iChargeLevel = 1;
				if (m_iClip == SPORELAUNCHER_MAX_CLIP) {
					m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.1;
				}
			}
		}
	}
	else {
		// reload debounce has timed out
		SendWeaponAnim((int)SPLAUNCHER_RELOAD_AIM::sequence);
		m_iChargeLevel = 0;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)SPLAUNCHER_RELOAD_AIM::frames, (int)SPLAUNCHER_RELOAD_AIM::fps);
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle;
		m_pPlayer->m_flNextAttack = m_flNextSecondaryAttack;
	}
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CSporelauncher::WeaponIdle() {
	if ((m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) && !m_iChargeLevel) {
		return;
	}

	if (m_iChargeLevel != 0) {
		Charge(false);
	}
	else {
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5) {
			iAnim = (int)SPLAUNCHER_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SPLAUNCHER_IDLE1::frames, (int)SPLAUNCHER_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.7) {
			iAnim = (int)SPLAUNCHER_IDLE2::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SPLAUNCHER_IDLE2::frames, (int)SPLAUNCHER_IDLE2::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.9) {
			iAnim = (int)SPLAUNCHER_FIDGET::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SPLAUNCHER_FIDGET::frames, (int)SPLAUNCHER_FIDGET::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else {
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
		}

		SendWeaponAnim(iAnim);
	}
}