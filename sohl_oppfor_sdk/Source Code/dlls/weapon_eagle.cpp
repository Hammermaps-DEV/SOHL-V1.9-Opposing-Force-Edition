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
// Weapon: Desert Eagle * http://half-life.wikia.com/wiki/Desert_Eagle
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "weapon_eagle.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_eagle, CEagle);

//=========================================================
// Spawn Desert Eagle
//=========================================================
void CEagle::Spawn() {
	Precache();

	SET_MODEL(ENT(pev), "models/w_desert_eagle.mdl");
	m_iDefaultAmmo = EAGLE_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CEagle::Precache(void) {
	PRECACHE_MODEL("models/v_desert_eagle.mdl");
	PRECACHE_MODEL("models/w_desert_eagle.mdl");
	PRECACHE_MODEL("models/p_desert_eagle.mdl");

	PRECACHE_MODEL("models/shell.mdl");// brass shell

	PRECACHE_SOUND("weapons/desert_eagle_fire.wav");
	PRECACHE_SOUND("weapons/desert_eagle_reload.wav");
	PRECACHE_SOUND("weapons/desert_eagle_sight.wav");
	PRECACHE_SOUND("weapons/desert_eagle_sight2.wav");

	m_usFireEagle = PRECACHE_EVENT(1, "events/eagle.sc");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CEagle::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = _357_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = EAGLE_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 2;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_EAGLE;
	p->iWeight = EAGLE_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CEagle::PrimaryAttack(void) {
	if (m_iClip <= 0) {
		if (m_fFireOnEmpty) {
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.2;
		}

		return;
	}

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	m_iClip--;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming;

	const Vector& vecSpread = VECTOR_CONE_2DEGREES;

	float flCycleTime;
	if (!m_fSpotActive) {
		vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);
		flCycleTime = 0.25f;
	}
	else {
		vecAiming = gpGlobals->v_forward;
		flCycleTime = 0.5f;
	}

	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, (m_fSpotActive) ? VECTOR_CONE_1DEGREES : VECTOR_CONE_10DEGREES, 8192, BULLET_PLAYER_357, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireEagle, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, m_iClip ? 0 : 1, 0);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}

	UpdateSpot();
}

//=========================================================
// SecondaryAttack
//=========================================================
void CEagle::SecondaryAttack(void) {
	if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && !m_iClip) {
		return;
	}

	m_fSpotActive = !m_fSpotActive;
	if (!m_fSpotActive && m_pSpot) {
		ShutdownSpot();
	}

	m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.3;
}

//=========================================================
// Deploy
//=========================================================
BOOL CEagle::Deploy() {
	return DefaultDeploy("models/v_desert_eagle.mdl", "models/p_desert_eagle.mdl", (int)EAGLE_DRAW::sequence,
		"eagle", CalculateWeaponTime((int)EAGLE_DRAW::frames, (int)EAGLE_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CEagle::Holster(void) {
	ShutdownSpot();
	m_fInReload = FALSE;// cancel any reload in progress.
	SendWeaponAnim((int)EAGLE_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)EAGLE_HOLSTER::frames, (int)EAGLE_HOLSTER::fps);
}

//=========================================================
// Reload
//=========================================================
void CEagle::Reload(void) {
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0) {
		return;
	}

	if (m_fSpotActive) {
		ShutdownSpot();
	}

	if (m_iClip == 0) {
		DefaultReload(EAGLE_MAX_CLIP, (int)EAGLE_RELOAD::sequence,
			CalculateWeaponTime((int)EAGLE_RELOAD::frames, (int)EAGLE_RELOAD::fps));
	}
	else {
		DefaultReload(EAGLE_MAX_CLIP, (int)EAGLE_RELOAD_NOT_EMPTY::sequence,
			CalculateWeaponTime((int)EAGLE_RELOAD_NOT_EMPTY::frames, (int)EAGLE_RELOAD_NOT_EMPTY::fps));
	}

	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CEagle::WeaponIdle(void) {
	UpdateSpot();
	float flTime = 0.0;
	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
		return;
	}

	// only idle if the slid isn't back
	if (m_iClip) {
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.2) {
			iAnim = (int)EAGLE_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)EAGLE_IDLE1::frames, (int)EAGLE_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.3) {
			iAnim = (int)EAGLE_IDLE2::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)EAGLE_IDLE2::frames, (int)EAGLE_IDLE2::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.5) {
			iAnim = (int)EAGLE_IDLE3::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)EAGLE_IDLE3::frames, (int)EAGLE_IDLE3::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.7) {
			iAnim = (int)EAGLE_IDLE4::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)EAGLE_IDLE4::frames, (int)EAGLE_IDLE4::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.9) {
			iAnim = (int)EAGLE_IDLE5::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)EAGLE_IDLE5::frames, (int)EAGLE_IDLE5::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else {
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
		}

		SendWeaponAnim(iAnim);
	}
}

//=========================================================
// ShutdownScreen
//=========================================================
void CEagle::ShutdownSpot(void) {
	if (m_pSpot) {
		m_pSpot->Killed(NULL, GIB_NEVER);
		m_pSpot = NULL;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/desert_eagle_sight2.wav", VOL_LOW, ATTN_NORM);
		m_pPlayer->m_fSpotActive = false;
	}

	if (m_pMirSpot) {
		m_pMirSpot->Killed(NULL, GIB_NEVER);
		m_pMirSpot = NULL;
	}
}

//=========================================================
// UpdateSpot
//=========================================================
void CEagle::UpdateSpot(void) {
	if (m_fSpotActive) {
		if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && !m_iClip) {
			ShutdownSpot();
			return;
		}

		if (!m_pSpot) {
			m_pSpot = CLaserSpot::CreateSpot();
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/desert_eagle_sight.wav", VOL_LOW, ATTN_NORM);
			m_pPlayer->m_fSpotActive = true;
		}

		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = gpGlobals->v_forward;

		TraceResult tr;
		UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 8192, dont_ignore_monsters, ignore_glass, ENT(m_pPlayer->pev), &tr);
		float flLength = (tr.vecEndPos - vecSrc).Length();

		int m_iSpotBright = (1 / log(flLength / 0.3)) * 1700;
		if (m_iSpotBright > 255) m_iSpotBright = 255;

		m_iSpotBright = m_iSpotBright + RANDOM_LONG(1, flLength / 200);
		m_pSpot->pev->renderamt = m_iSpotBright;

		UTIL_SetOrigin(m_pSpot, tr.vecEndPos + tr.vecPlaneNormal * 0.1);

		Vector mirpos = UTIL_MirrorPos(tr.vecEndPos + tr.vecPlaneNormal * 0.1);
		if (mirpos != Vector(0, 0, 0)) {
			if (!m_pMirSpot)m_pMirSpot = CLaserSpot::CreateSpot();
			UTIL_SetOrigin(m_pMirSpot, mirpos);
			m_pMirSpot->pev->renderamt = m_iSpotBright;
		}
	}
}