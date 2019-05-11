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
// Weapon: Colt Python 357 Magnum Revolver * 
// http://half-life.wikia.com/wiki/Colt_Python
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "monsters.h"
#include "player.h"
#include "weapon_python.h"

extern bool gInfinitelyAmmo;

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_python, CPython);
LINK_ENTITY_TO_CLASS(weapon_357, CPython);

//=========================================================
// Spawn Colt Python 357
//=========================================================
void CPython::Spawn() {
	Precache();
	m_iId = WEAPON_PYTHON;

	SET_MODEL(ENT(pev), "models/w_357.mdl");
	m_iDefaultAmmo = PYTHON_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CPython::Precache(void) {
	PRECACHE_MODEL("models/v_357.mdl");
	PRECACHE_MODEL("models/w_357.mdl");
	PRECACHE_MODEL("models/p_357.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl");// brass shellTE_MODEL

	PRECACHE_SOUND("weapons/357_reload1.wav");
	PRECACHE_SOUND("weapons/reload1.wav");
	PRECACHE_SOUND("weapons/reload2.wav");

	m_usFirePython = PRECACHE_EVENT(1, "events/python.sc");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CPython::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = _357_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = PYTHON_MAX_CLIP;
	p->iFlags = 0;
	p->iSlot = 1;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_PYTHON;
	p->iWeight = PYTHON_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CPython::PrimaryAttack() {
	//don't fire underwater
	if (m_iClip && m_pPlayer->pev->waterlevel != 3) {
		if (m_pSpot && m_fSpotActive)	m_pSpot->Suspend(1.0);

		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

		if(!gInfinitelyAmmo)
			m_iClip--;

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

		Vector vecDir;
		vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_357, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);

		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFirePython, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, pev->body, 0, 0, 0);

		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + CalculateWeaponTime((int)PYTHON_SHOOT::frames, (int)PYTHON_SHOOT::fps);
		if (!IsMultiplayer()) {
			m_flNextSecondaryAttack = m_flNextPrimaryAttack;
		}

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
		if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
			// HEV suit - indicate out of ammo condition
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
		}
	}
	else {
		PlayEmptySound(); Reload();
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.7;
	}
}

//=========================================================
// SecondaryAttack
//=========================================================
void CPython::SecondaryAttack(void) {
	if (IsMultiplayer()) {
		m_fSpotActive = !m_fSpotActive;
		if (!m_fSpotActive && m_pSpot) {
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_off.wav", VOL_NORM, ATTN_NORM);
			m_pSpot->Killed(NULL, GIB_NORMAL);
			m_pSpot = NULL;
		}

		m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.3;
	}
	else {
		PrimaryAttack();
	}
}

//=========================================================
// Deploy
//=========================================================
BOOL CPython::Deploy() {
	if (IsMultiplayer()) m_iBody = 1;//enable laser sight geometry
	return DefaultDeploy("models/v_357.mdl", "models/p_357.mdl", (int)PYTHON_DRAW::sequence,
		"python", CalculateWeaponTime((int)PYTHON_DRAW::frames, (int)PYTHON_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CPython::Holster() {
	m_fInReload = FALSE;// cancel any reload in progress.
	SendWeaponAnim((int)PYTHON_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)PYTHON_HOLSTER::frames, (int)PYTHON_HOLSTER::fps);

	if (m_pSpot) {
		m_pSpot->Killed(NULL, GIB_NEVER);
		m_pSpot = NULL;
	}
}

//=========================================================
// Reload
//=========================================================
void CPython::Reload(void) {
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0) {
		return;
	}

	if (m_pSpot && m_fSpotActive) {
		m_pSpot->Suspend(2.0);
	}

	if (DefaultReload(PYTHON_MAX_CLIP, (int)PYTHON_RELOAD::sequence,
		CalculateWeaponTime((int)PYTHON_RELOAD::frames, (int)PYTHON_RELOAD::fps))) {
		m_flSoundDelay = UTIL_GlobalTimeBase();
	}

	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CPython::WeaponIdle(void) {
	if (IsMultiplayer()) {
		UpdateSpot(); //Spot Update
	}

	if (m_flSoundDelay != 0 && m_flSoundDelay <= UTIL_GlobalTimeBase()) { //Finsh Reload
		m_flSoundDelay = 0;
		for (int i = 0; i < 6; i++) {
			EjectBrass(m_pPlayer->pev->origin, Vector(RANDOM_FLOAT(-10.0, 10.0), RANDOM_FLOAT(-10.0, 10.0), (float)0.0), m_pPlayer->pev->angles.y, m_iShell, TE_BOUNCE_SHELL);
		}
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
			iAnim = (int)PYTHON_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)PYTHON_IDLE1::frames, (int)PYTHON_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.7) {
			iAnim = (int)PYTHON_IDLE2::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)PYTHON_IDLE2::frames, (int)PYTHON_IDLE2::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.9) {
			iAnim = (int)PYTHON_IDLE3::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)PYTHON_IDLE3::frames, (int)PYTHON_IDLE3::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else {
			if (RANDOM_FLOAT(0, 1) && !m_fSpotActive && IsMultiplayer()) {
				iAnim = (int)PYTHON_FIDGET::sequence;
				m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
					CalculateWeaponTime((int)PYTHON_FIDGET::frames, (int)PYTHON_FIDGET::fps);
				m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
			}
			else {
				m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
				m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
			}
		}

		SendWeaponAnim(iAnim);
	}
}

//=========================================================
// UpdateSpot * Only in Multiplayer
//=========================================================
void CPython::UpdateSpot(void) {
	if (m_fSpotActive) {
		if (!m_pSpot) {
			m_pSpot = CLaserSpot::CreateSpot();
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_on.wav", VOL_NORM, ATTN_NORM);
		}

		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		Vector vecSrc = m_pPlayer->GetGunPosition();;
		Vector vecAiming = gpGlobals->v_forward;

		TraceResult tr;
		UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 8192, dont_ignore_monsters, ignore_glass, ENT(m_pPlayer->pev), &tr);
		float flLength = (tr.vecEndPos - vecSrc).Length();

		m_pSpot->pev->scale = flLength / 340;
		int m_iSpotBright = (1 / log(flLength / 0.3)) * 1700;
		if (m_iSpotBright > 255) m_iSpotBright = 255;

		m_iSpotBright = m_iSpotBright + RANDOM_LONG(1, flLength / 200);
		m_pSpot->pev->renderamt = m_iSpotBright;

		UTIL_SetOrigin(m_pSpot, tr.vecEndPos + tr.vecPlaneNormal * 0.1);

		//allow oriented LTD in multiplayer only, but python has LTD only in multiplayer - remove check
		Vector n = tr.vecPlaneNormal;
		n.x *= -1;
		n.y *= -1;
		m_pSpot->pev->angles = UTIL_VecToAngles(n);

	}
}