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

//=========================================================
// Weapon: Shotgun * http://half-life.wikia.com/wiki/HECU_SPAS-12
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00002 / Date: 07.02.2016
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "weapon_shotgun.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS( weapon_shotgun, CShotgun );

//=========================================================
// Spawn Shotgun
//=========================================================
void CShotgun::Spawn(void) {
	Precache( );

	SET_MODEL(ENT(pev), "models/w_shotgun.mdl");
	m_iDefaultAmmo = SHOTGUN_DEFAULT_GIVE;

	FallInit();// get ready to fall
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CShotgun::Precache(void) {
	PRECACHE_MODEL("models/v_shotgun.mdl");
	PRECACHE_MODEL("models/w_shotgun.mdl");
	PRECACHE_MODEL("models/p_shotgun.mdl");

	PRECACHE_MODEL ("models/shotgunshell.mdl"); // brass shell

	PRECACHE_SOUND("weapons/scock1.wav"); //by model
	PRECACHE_SOUND("weapons/dbarrel1.wav");
	PRECACHE_SOUND("weapons/sbarrel1.wav");
	PRECACHE_SOUND("weapons/reload1.wav");
	PRECACHE_SOUND("weapons/reload3.wav");

	m_usShotgunFire = PRECACHE_EVENT( 1, "events/shotgun.sc" );
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CShotgun::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";
	p->iMaxAmmo1 = BUCKSHOT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SHOTGUN_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SHOTGUN;
	p->iWeight = SHOTGUN_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CShotgun::PrimaryAttack(void) {
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3) {
		PlayEmptySound(4);
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.15;
		return;
	}

	if (m_iClip == 0) {
		PlayEmptySound(4);
		Reload();
		return;
	}

	if (m_iChargeLevel >= 1) {
		Charge(true);
	} else {
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

		m_iClip--;
		m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
		Vector vecDir;

		if (IsMultiplayer())
			vecDir = m_pPlayer->FireBulletsPlayer(4, vecSrc, vecAiming, VECTOR_CONE_DM_SHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);
		else
			vecDir = m_pPlayer->FireBulletsPlayer(6, vecSrc, vecAiming, VECTOR_CONE_7DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);

		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usShotgunFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, pev->body, 0, 0, 0);

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
			// HEV suit - indicate out of ammo condition
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 
			CalculateWeaponTime((int)SHOTGUN_SHOOT::frames, (int)SHOTGUN_SHOOT::fps);

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
		m_iChargeLevel = 0;
	}
}

//=========================================================
// SecondaryAttack
//=========================================================
void CShotgun::SecondaryAttack(void) {
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3) {
		PlayEmptySound(4);
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.15;
		return;
	}

	if (m_iClip == 1) {
		PrimaryAttack();
		return;
	}

	if (m_iClip == 0) {
		PlayEmptySound(4);
		Reload();
		return;
	}

	if (m_iChargeLevel >= 1) {
		Charge(true);
	} else {
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

		m_iClip -= 2;

		m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
		Vector vecDir;

		if (IsMultiplayer())
			vecDir = m_pPlayer->FireBulletsPlayer(8, vecSrc, vecAiming, VECTOR_CONE_DM_DOUBLESHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);
		else
			vecDir = m_pPlayer->FireBulletsPlayer(12, vecSrc, vecAiming, VECTOR_CONE_9DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);

		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usShotgunFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, pev->body, 1, 0, 0);

		if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
			// HEV suit - indicate out of ammo condition
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
		}

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 
			CalculateWeaponTime((int)SHOTGUN_SHOOT_BIG::frames, (int)SHOTGUN_SHOOT_BIG::fps);

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);

		m_iChargeLevel = 0;
		m_pPlayer->pev->punchangle.x -= 5;
	}
}

//=========================================================
// Deploy
//=========================================================
BOOL CShotgun::Deploy(void) {
	return DefaultDeploy("models/v_shotgun.mdl", "models/p_shotgun.mdl", (int)SHOTGUN_DRAW::sequence,
		"shotgun", CalculateWeaponTime((int)SHOTGUN_DRAW::frames, (int)SHOTGUN_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CShotgun::Holster(void) {
	m_fInReload = FALSE;// cancel any reload in progress.
	SendWeaponAnim((int)SHOTGUN_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)SHOTGUN_HOLSTER::frames, (int)SHOTGUN_HOLSTER::fps);
}

//=========================================================
// Reload
//=========================================================
void CShotgun::Reload(void) {
	if ((m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == SHOTGUN_MAX_CLIP) || 
			m_flNextPrimaryAttack > UTIL_GlobalTimeBase() || m_flNextSecondaryAttack > UTIL_GlobalTimeBase()) {
		return;
	} else { Charge(false); }
}

//=========================================================
// Reload Charge
//=========================================================
void CShotgun::Charge(bool m_BeginAttack) {
	if((m_iClip != SHOTGUN_MAX_CLIP && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]) && !m_BeginAttack) {
		if(m_iChargeLevel == 0) {
			SendWeaponAnim((int)SHOTGUN_START_RELOAD::sequence);
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SHOTGUN_START_RELOAD::frames, (int)SHOTGUN_START_RELOAD::fps);
			m_pPlayer->m_flNextAttack = m_flTimeWeaponIdle;
			m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + m_flTimeWeaponIdle;
			m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + m_flTimeWeaponIdle;
			m_iChargeLevel = 1;
		} else if (m_iChargeLevel == 1) {
			if (m_flTimeWeaponIdle < UTIL_GlobalTimeBase()) {
				m_iChargeLevel = 2;

				if (RANDOM_LONG(0, 1)) {
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));
				} else {
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));
				}

				SendWeaponAnim((int)SHOTGUN_RELOAD::sequence);
				m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 
					CalculateWeaponTime((int)SHOTGUN_RELOAD::frames, (int)SHOTGUN_RELOAD::fps);
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle;
				m_pPlayer->m_flNextAttack = m_flTimeWeaponIdle;
			}
		} else {
			if (m_flTimeWeaponIdle < UTIL_GlobalTimeBase()) {
				m_iClip += 1;
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
				m_iChargeLevel = 1;
				if (m_iClip == SHOTGUN_MAX_CLIP) {
					m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.1;
				}
			}
		}
	} else {
		// reload debounce has timed out
		SendWeaponAnim((int)SHOTGUN_PUMP::sequence);
		m_iChargeLevel = 0;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)SHOTGUN_PUMP::frames, (int)SHOTGUN_PUMP::fps);
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle;
		m_pPlayer->m_flNextAttack = m_flNextSecondaryAttack;
	}
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CShotgun::WeaponIdle(void) {
	if ((m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) && !m_iChargeLevel) {
		return;
	}

	if (m_iChargeLevel != 0) {
		Charge(false);
	} else {
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5) {
			iAnim = (int)SHOTGUN_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SHOTGUN_IDLE1::frames, (int)SHOTGUN_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.7) {
			iAnim = (int)SHOTGUN_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SHOTGUN_IDLE1::frames, (int)SHOTGUN_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.9) {
			iAnim = (int)SHOTGUN_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SHOTGUN_IDLE1::frames, (int)SHOTGUN_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else {
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
		}

		SendWeaponAnim(iAnim);
	}
}