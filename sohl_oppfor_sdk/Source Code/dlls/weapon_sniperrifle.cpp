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
*	Base Source-Code written by Raven City and Marc-Antoine Lortie (https://github.com/malortie).
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/
//=========================================================
// Weapon: M40A1 Sniper Rifle * 
// http://half-life.wikia.com/wiki/M40A1_Sniper_Rifle
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "monsters.h"
#include "player.h"
#include "gamerules.h"
#include "weapon_sniperrifle.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_sniperrifle, CSniperrifle);
LINK_ENTITY_TO_CLASS(weapon_m40a1, CSniperrifle);

//=========================================================
// Spawn M40A1 Sniper Rifle
//=========================================================
void CSniperrifle::Spawn(void) {
	Precache();

	SET_MODEL(ENT(pev), "models/w_m40a1.mdl");
	m_iDefaultAmmo = SNIPERRIFLE_DEFAULT_GIVE;

	m_fNeedAjustBolt = FALSE;
	m_iBoltState = BOLTSTATE_FINE;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CSniperrifle::Precache(void) {
	PRECACHE_MODEL("models/v_m40a1.mdl");
	PRECACHE_MODEL("models/w_m40a1.mdl");
	PRECACHE_MODEL("models/p_m40a1.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl");// brass shellTE_MODEL

	PRECACHE_SOUND("items/9mmclip1.wav"); //by model

	PRECACHE_SOUND("weapons/sniper_bolt1.wav"); //by model
	PRECACHE_SOUND("weapons/sniper_bolt2.wav"); //by model

	PRECACHE_SOUND("weapons/sniper_fire.wav");
	PRECACHE_SOUND("weapons/sniper_fire_last_round.wav");

	PRECACHE_SOUND("weapons/sniper_miss.wav");

	PRECACHE_SOUND("weapons/sniper_reload_first_seq.wav"); //by model
	PRECACHE_SOUND("weapons/sniper_reload_second_seq.wav"); //by model

	PRECACHE_SOUND("weapons/sniper_zoom.wav");

	m_usSniper = PRECACHE_EVENT(1, "events/sniper.sc");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CSniperrifle::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762";
	p->iMaxAmmo1 = _762_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SNIPERRIFLE_MAX_CLIP;
	p->iFlags = 0;
	p->iSlot = 5;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_SNIPERRIFLE;
	p->iWeight = SNIPERRIFLE_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CSniperrifle::PrimaryAttack(void) {
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3) {
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.15;
		return;
	}

	if (m_iClip <= 0) {
		if (!m_fFireOnEmpty)
			Reload();
		else {
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.2;
		}

		return;
	}

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	m_iClip--;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	m_fNeedAjustBolt = (m_iClip <= 0) ? 1 : 0;

	// If this was the last round in the clip, make sure to schedule
	// bolt adjustment.
	if (m_fNeedAjustBolt) {
		m_iBoltState = BOLTSTATE_ADJUST;
	}

	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, Vector(0, 0, 0), 8192, BULLET_PLAYER_762, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usSniper, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, m_fNeedAjustBolt, 0, 0, 0);

	m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + CalculateWeaponTime((int)M40A1_FIRE::frames, (int)M40A1_FIRE::fps);
	m_flShellDelay = m_pPlayer->m_flNextAttack = m_flNextSecondaryAttack = m_flNextPrimaryAttack;
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}
}

//=========================================================
// SecondaryAttack * Only in "weapon_sniperrifle.h"
//=========================================================

//=========================================================
// Deploy
//=========================================================
BOOL CSniperrifle::Deploy(void) {
	if (m_fNeedAjustBolt) {
		m_iBoltState = BOLTSTATE_ADJUST;
	}

	return DefaultDeploy("models/v_m40a1.mdl", "models/p_m40a1.mdl", (int)M40A1_DRAW::sequence,
		"bow", CalculateWeaponTime((int)M40A1_DRAW::frames, (int)M40A1_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CSniperrifle::Holster(void) {
	m_fInReload = FALSE;// cancel any reload in progress.
	ZoomReset();

	if (m_fNeedAjustBolt) {
		m_iBoltState = BOLTSTATE_ADJUST;
	}

	SendWeaponAnim((int)M40A1_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)M40A1_HOLSTER::frames, (int)M40A1_HOLSTER::fps);
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
}

//=========================================================
// Reload
//=========================================================
void CSniperrifle::Reload(void) {
	if (m_pPlayer->ammo_762 <= 0)
		return;

	if (m_iChargeLevel) {
		ZoomReset();
	}

	// Select the appropriate sequence for reload.
	// One has bolt adjusted, the other does not.
	int iReloadAnim = (m_iClip > 0)
		? (int)M40A1_RELOAD_START::sequence	// Regular reload.
		: (int)M40A1_RELOAD_START_EMPTY::sequence;	// No ammo in current clip.

	int iReloadTime = (m_iClip > 0)
		? CalculateWeaponTime((int)M40A1_RELOAD_START::frames, (int)M40A1_RELOAD_START::fps)	// Regular reload.
		: CalculateWeaponTime((int)M40A1_RELOAD_START_EMPTY::frames, (int)M40A1_RELOAD_START_EMPTY::fps);	// No ammo in current clip.

	DefaultReload(SNIPERRIFLE_MAX_CLIP, iReloadAnim, iReloadTime);
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
}

//=========================================================
// ZoomUpdate
//=========================================================
void CSniperrifle::ZoomUpdate(void) {
	if (m_pPlayer->pev->button & IN_ATTACK2) {
		if (m_iChargeLevel == 0) {
			if (m_flShockTime > UTIL_GlobalTimeBase()) return;
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/sniper_zoom.wav", VOL_NORM, ATTN_NORM);
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

		if (m_iChargeLevel == 2 && m_pPlayer->m_iFOV > 18) {
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
void CSniperrifle::ZoomReset(void) {
	m_flShockTime = UTIL_GlobalTimeBase() + 0.5;
	m_pPlayer->m_iFOV = 90;
	m_iChargeLevel = 0;//clear zoom
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CSniperrifle::WeaponIdle(void) {
	ZoomUpdate();
	ResetEmptySound();

	if (m_flShellDelay != 0 && m_flShellDelay <= UTIL_GlobalTimeBase()) { //Finsh Fire
		m_flShellDelay = 0;
		Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40, 90) + gpGlobals->v_up * RANDOM_FLOAT(75, 200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
		EjectBrass(m_pPlayer->pev->origin, vecShellVelocity, m_pPlayer->pev->angles.y, m_iShell, TE_BOUNCE_SHELL);
	}

	if (m_iChargeLevel != 0) {
		return;
	}

	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
		return;
	}

	// only idle if the slid isn't back
	if (m_iClip) {
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5) {
			iAnim = (m_iClip >= 1) ? (int)M40A1_SLOWIDLE::sequence : (int)M40A1_SLOWIDLE2::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				((m_iClip >= 1) ? CalculateWeaponTime((int)M40A1_SLOWIDLE::frames, (int)M40A1_SLOWIDLE::fps)
					: CalculateWeaponTime((int)M40A1_SLOWIDLE2::frames, (int)M40A1_SLOWIDLE2::fps));
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(5, 10);
			SendWeaponAnim(iAnim);
		}
		else {
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
		}
	}
}

//=========================================================
// ItemPostFrame
//=========================================================
void CSniperrifle::ItemPostFrame(void) {
	if ((m_fInReload) && (m_pPlayer->m_flNextAttack <= UTIL_GlobalTimeBase())) {
		if (m_fNeedAjustBolt) {
			switch (m_iBoltState) {
			case BOLTSTATE_ADJUST: {
				ALERT(at_aiconsole, "BOLTSTATE_ADJUST!\n");
				m_iBoltState = BOLTSTATE_ADJUSTING;
				SendWeaponAnim((int)M40A1_RELOAD_END::sequence);
				m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() + CalculateWeaponTime((int)M40A1_RELOAD_END::frames, (int)M40A1_RELOAD_END::fps);
			}
								   break;
			case BOLTSTATE_ADJUSTING: {
				ALERT(at_aiconsole, "BOLTSTATE_ADJUSTING!\n");
				m_fNeedAjustBolt = FALSE;
				m_iBoltState = BOLTSTATE_FINE;
			}
									  break;
			default:
				ALERT(at_aiconsole, "Warning: Unknown bolt state!\n");
				break;
			}

			return;
		}
		else {
			int j = V_min(iMaxClip() - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);
			m_iClip += j;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= j;
			m_fInReload = FALSE;
		}
	}

	CBasePlayerWeapon::ItemPostFrame();
}