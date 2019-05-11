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
// Weapon: Shockrifle * http://half-life.wikia.com/wiki/Shock_Roach
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"
#include "weapon_hornetgun.h"
#include "weapon_shockrifle.h"

extern bool gInfinitelyAmmo;

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_shockrifle, CShockrifle);

//=========================================================
// Shockrifle Sounds
//=========================================================
const char *CShockrifle::pIdleSounds[] = {
	"shockroach/shock_idle1.wav",
	"shockroach/shock_idle2.wav",
	"shockroach/shock_idle3.wav"
};

//=========================================================
// Spawn Shockrifle
//=========================================================
void CShockrifle::Spawn() {
	Precache();

	SET_MODEL(ENT(pev), "models/w_shock.mdl");
	m_iDefaultAmmo = SHOCKRIFLE_DEFAULT_GIVE;

	m_iFirePhase = 0;
	m_fShouldUpdateEffects = FALSE;
	m_flBeamLifeTime = 0.0f;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CShockrifle::Precache(void) {
	PRECACHE_MODEL("models/v_shock.mdl");
	PRECACHE_MODEL("models/w_shock.mdl");
	PRECACHE_MODEL("models/p_shock.mdl");

	PRECACHE_SOUND_ARRAY(pIdleSounds);

	PRECACHE_SOUND("weapons/shock_discharge.wav");
	PRECACHE_SOUND("weapons/shock_draw.wav");
	PRECACHE_SOUND("weapons/shock_fire.wav");
	PRECACHE_SOUND("weapons/shock_impact.wav");
	PRECACHE_SOUND("weapons/shock_recharge.wav");

	m_usShockFire = PRECACHE_EVENT(1, "events/shock.sc");

	UTIL_PrecacheOther("shock");
}

//=========================================================
// AddToPlayer
//=========================================================
int CShockrifle::AddToPlayer(CBasePlayer *pPlayer) {
	int bResult = CBasePlayerWeapon::AddToPlayer(pPlayer);
	if (g_pGameRules->IsMultiplayer()) {
		// in multiplayer, all hivehands come full. 
		pPlayer->m_rgAmmo[PrimaryAmmoIndex()] = SHOCK_MAX_CARRY;
	}

	if (bResult) return AddWeapon();
	return FALSE;
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CShockrifle::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "shocks";
	p->iMaxAmmo1 = SHOCK_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 6;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_SHOCKRIFLE;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iWeight = SHOCKRIFLE_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CShockrifle::PrimaryAttack() {
	Reload(); //Auto recharge
	if (m_pPlayer->pev->waterlevel != 3) {
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
			return;
		}

		UTIL_MakeVectors(m_pPlayer->pev->v_angle);

		Vector vecSrc = m_pPlayer->GetGunPosition();
		vecSrc = vecSrc + gpGlobals->v_forward * 8;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		vecSrc = vecSrc + gpGlobals->v_up * -12;

		CBaseEntity *pShock = CBaseEntity::Create("shock", vecSrc, m_pPlayer->pev->v_angle, m_pPlayer->edict());
		pShock->pev->velocity = gpGlobals->v_forward * 1500;

		m_flRechargeTime = UTIL_GlobalTimeBase() + 0.45;

		if(!gInfinitelyAmmo)
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usShockFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, FALSE, 0, 0, 0);

		if (!m_fShouldUpdateEffects || UTIL_GlobalTimeBase() >= m_flBeamLifeTime) {
			m_fShouldUpdateEffects = TRUE;
			m_flBeamLifeTime = UTIL_GlobalTimeBase() + CalculateWeaponTime((int)SHOCKRIFLE_SHOOT::frames, (int)SHOCKRIFLE_SHOOT::fps);
			UpdateEffects();
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.30;
		if (m_flNextPrimaryAttack < UTIL_GlobalTimeBase()) {
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.30;
		}

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
	}
	else {
		PlayEmptySound(5);
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.30;
	}
}

//=========================================================
// Deploy
//=========================================================
BOOL CShockrifle::Deploy() {
	return DefaultDeploy("models/v_shock.mdl", "models/p_shock.mdl", (int)SHOCKRIFLE_DRAW::sequence,
		"shockrifle", CalculateWeaponTime((int)SHOCKRIFLE_DRAW::frames, (int)SHOCKRIFLE_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CShockrifle::Holster() {
	SendWeaponAnim((int)SHOCKRIFLE_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)SHOCKRIFLE_HOLSTER::frames, (int)SHOCKRIFLE_HOLSTER::fps);

	if (!m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()]) {
		m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] = 1;
	}
}

//=========================================================
// Reload
//=========================================================
void CShockrifle::Reload(void) {
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= SHOCK_MAX_CARRY)
		return;

	while (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < SHOCK_MAX_CARRY && m_flRechargeTime < UTIL_GlobalTimeBase()) {
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/shock_recharge.wav", VOL_LOW, ATTN_NORM);
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		m_flRechargeTime += 0.45;
	}
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CShockrifle::WeaponIdle(void) {
	Reload(); //Auto recharge
	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
		return;
	}

	// only idle if the slid isn't back
	if (m_iClip) {
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5) {
			iAnim = (int)SHOCKRIFLE_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SHOCKRIFLE_IDLE1::frames, (int)SHOCKRIFLE_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);

			if (RANDOM_FLOAT(0, 10) >= 5)
				EMIT_SOUND_DYN(edict(), CHAN_VOICE, RANDOM_SOUND_ARRAY(pIdleSounds), VOL_LOW, ATTN_IDLE, 0, 90 + (RANDOM_LONG(-5, 5)));

			SendWeaponAnim(iAnim);
		}
		else if (flRand >= 0.7) {
			iAnim = (int)SHOCKRIFLE_IDLE2::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)SHOCKRIFLE_IDLE2::frames, (int)SHOCKRIFLE_IDLE2::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);

			if (RANDOM_FLOAT(0, 10) >= 5)
				EMIT_SOUND_DYN(edict(), CHAN_VOICE, RANDOM_SOUND_ARRAY(pIdleSounds), VOL_LOW, ATTN_IDLE, 0, 90 + (RANDOM_LONG(-5, 5)));

			SendWeaponAnim(iAnim);
		}
		else {
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
		}
	}
}

//=========================================================
// Update Effects
//=========================================================
void CShockrifle::UpdateEffects() {
	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usShockFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, TRUE, 0, 0, 0);
}

//=========================================================
// ItemPostFrame
//=========================================================
void CShockrifle::ItemPostFrame(void) {
	CBasePlayerWeapon::ItemPostFrame();
	if (!m_pPlayer->pev->button & IN_ATTACK) {
		if (m_fShouldUpdateEffects) {
			if (UTIL_GlobalTimeBase() <= m_flBeamLifeTime) {
				UpdateEffects();
			}
			else {
				m_fShouldUpdateEffects = FALSE;
				m_flBeamLifeTime = 0.0f;
			}
		}
	}
}
