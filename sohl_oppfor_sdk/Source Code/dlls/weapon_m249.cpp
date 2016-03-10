/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//=========================================================
// Weapon: M249 Squad Automatic Weapon
// http://half-life.wikia.com/wiki/M249_Squad_Automatic_Weapon
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "weapon_m249.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_m249, CM249);
LINK_ENTITY_TO_CLASS(weapon_saw, CM249);

//=========================================================
// Spawn M249 Squad Automatic Weapon
//=========================================================
void CM249::Spawn(void) {
	Precache();

	SET_MODEL(ENT(pev), "models/w_saw.mdl");
	m_iDefaultAmmo = M249_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CM249::Precache(void) {
	PRECACHE_MODEL("models/v_saw.mdl");
	PRECACHE_MODEL("models/w_saw.mdl");
	PRECACHE_MODEL("models/p_saw.mdl");

	PRECACHE_SOUND("weapons/saw_fire1.wav");
	PRECACHE_SOUND("weapons/saw_fire2.wav");
	PRECACHE_SOUND("weapons/saw_fire3.wav");

	PRECACHE_SOUND("weapons/saw_reload.wav"); //by model
	PRECACHE_SOUND("weapons/saw_reload2.wav"); //by model

	PRECACHE_MODEL("models/saw_shell.mdl");// brass shell
	PRECACHE_MODEL("models/saw_link.mdl");// brass shell link

	m_usM249 = PRECACHE_EVENT(1, "events/m249.sc");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CM249::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556";
	p->iMaxAmmo1 = _556_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = M249_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_M249;
	p->iWeight = M249_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CM249::PrimaryAttack(void) {
	if (m_iReloadStep)
		return;

	// don't fire underwater
	if (m_iClip && m_pPlayer->pev->waterlevel != 3) {
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

		m_iClip--;
		UpdateClip();

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
		Vector vecDir;

		if (!IsMultiplayer())
			vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_5DEGREES, 8192, BULLET_PLAYER_556, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);
		else
			vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_4DEGREES, 8192, BULLET_PLAYER_556, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);

		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usM249, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, pev->body, 0, 0, 0);

		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.0675;

		if (m_flNextPrimaryAttack < UTIL_GlobalTimeBase())
			m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.0675;

		if (!FBitSet(m_pPlayer->pev->flags, FL_DUCKING)) {
			float flOldPlayerVel = m_pPlayer->pev->velocity.z;
			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity + (50 * -gpGlobals->v_forward);
			m_pPlayer->pev->velocity.z = flOldPlayerVel;
		}

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 7.0 / 30.0;
	} else {
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.5;
	}
}

//=========================================================
// Deploy
//=========================================================
BOOL CM249::Deploy(void) {
	return DefaultDeploy("models/v_saw.mdl", "models/p_saw.mdl", (int)SAW_DRAW::sequence,
		"mp5", CalculateWeaponTime((int)SAW_DRAW::frames, (int)SAW_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CM249::Holster(void) {
	m_fInReload = FALSE;// cancel any reload in progress.
	SendWeaponAnim((int)SAW_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)SAW_HOLSTER::frames, (int)SAW_HOLSTER::fps);
}

//=========================================================
// Reload
//=========================================================
void CM249::Reload(void) {
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0 || m_iClip == M249_MAX_CLIP) {
		return;
	}

	UpdateClip();
	m_iReloadStep = true;

	DefaultReload(GLOCK_MAX_CLIP, (int)SAW_RELOAD_START::sequence,
		CalculateWeaponTime((int)SAW_RELOAD_START::frames, (int)SAW_RELOAD_START::fps));

	m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 
		CalculateWeaponTime((int)SAW_RELOAD_START::frames, (int)SAW_RELOAD_START::fps) + 
		CalculateWeaponTime((int)SAW_RELOAD_END::frames, (int)SAW_RELOAD_END::fps);
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + CalculateWeaponTime((int)SAW_RELOAD_START::frames, (int)SAW_RELOAD_START::fps);
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CM249::WeaponIdle(void) {
	if (m_iReloadStep) {
		m_iReloadStep = false;
		SendWeaponAnim((int)SAW_RELOAD_END::sequence);
		m_flTimeWeaponIdle = m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 
			CalculateWeaponTime((int)SAW_RELOAD_END::frames, (int)SAW_RELOAD_END::fps);
		UpdateClip();
	} else {
		UpdateClip();

		if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
			m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
			return;
		}

		// only idle if the slid isn't back
		if (m_iClip) {
			int iAnim;
			float flRand = RANDOM_FLOAT(0, 1);
			if (flRand <= 0.5) {
				iAnim = (int)SAW_SLOWIDLE::sequence;
				m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
					CalculateWeaponTime((int)SAW_SLOWIDLE::frames, (int)SAW_SLOWIDLE::fps);
				m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
			} else if (flRand <= 0.7) {
				iAnim = (int)SAW_IDLE::sequence;
				m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
					CalculateWeaponTime((int)SAW_IDLE::frames, (int)SAW_IDLE::fps);
				m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
			} else {
				m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
				m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
			}

			SendWeaponAnim(iAnim);
		}
	}
}

//=========================================================
// UpdateClip
//=========================================================
void CM249::UpdateClip(void) {
	if (m_iClip <= 8)
		pev->body = 1;
	if (m_iClip <= 7)
		pev->body = 2;
	if (m_iClip <= 6)
		pev->body = 3;
	if (m_iClip <= 5)
		pev->body = 4;
	if (m_iClip <= 4)
		pev->body = 5;
	if (m_iClip <= 3)
		pev->body = 6;
	if (m_iClip <= 2)
		pev->body = 7;
	if (m_iClip <= 1)
		pev->body = 8;
	if (m_iClip == 0)
		pev->body = 8;

	if (m_iClip != 0
		&& m_iClip != 1
		&& m_iClip != 2
		&& m_iClip != 3
		&& m_iClip != 4
		&& m_iClip != 5
		&& m_iClip != 6
		&& m_iClip != 7
		&& m_iClip != 8
		&& m_iClip != 9)
		pev->body = 0;
}