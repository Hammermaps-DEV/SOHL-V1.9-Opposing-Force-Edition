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
// Weapon: HECU Rocket Propelled Grenade
// http://half-life.wikia.com/wiki/HECU_RPG
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "weapon_rpg.h"
#include "proj_grenade.h"
#include "proj_plrocket.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_rpg, CRpg);

//m_iOverloadLevel is LTD trigger like m_fSpotActive in original HL
//m_iChargeLevel is indicator what indicates how many missiles in air
//fired from this laubcher

//=========================================================
// Spawn RPG
//=========================================================
void CRpg::Spawn() {
	Precache();
	m_iId = WEAPON_RPG;

	SET_MODEL(ENT(pev), "models/w_rpg.mdl");
	m_iOverloadLevel = TRUE;//turn on LTD

	if (IsMultiplayer())
		m_iDefaultAmmo = RPG_DEFAULT_GIVE * 2; //MP give 2 RPGs
	else
		m_iDefaultAmmo = RPG_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CRpg::Precache(void) {
	PRECACHE_MODEL("models/w_rpg.mdl");
	PRECACHE_MODEL("models/v_rpg.mdl");
	PRECACHE_MODEL("models/p_rpg.mdl");

	PRECACHE_SOUND("items/9mmclip1.wav");

	UTIL_PrecacheOther("laser_spot");
	UTIL_PrecacheOther("rpg_rocket");

	PRECACHE_SOUND("weapons/rocketfire1.wav");
	PRECACHE_SOUND("weapons/glauncher.wav"); // alternative fire sound
	PRECACHE_SOUND("weapons/beep.wav");
	PRECACHE_SOUND("weapons/spot_on.wav");
	PRECACHE_SOUND("weapons/spot_off.wav");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CRpg::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "rockets";
	p->iMaxAmmo1 = ROCKET_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = RPG_MAX_CLIP;
	p->iSlot = 3;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_RPG;
	p->iFlags = 0;
	p->iWeight = RPG_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CRpg::PrimaryAttack() {
	if (m_iClip) {
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

		pev->body = 1; //hide rocket in launcher
		SendWeaponAnim((int)RPG_FIRE::sequence);

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -8;

		CRpgRocket *pRocket = CRpgRocket::Create(vecSrc, m_pPlayer->pev->v_angle, m_pPlayer, this);
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);// RpgRocket::Create stomps on globals, so remake.
		pRocket->pev->velocity = pRocket->pev->velocity + gpGlobals->v_forward * DotProduct(m_pPlayer->pev->velocity, gpGlobals->v_forward);

		m_iClip--;
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)RPG_FIRE::frames, (int)RPG_FIRE::fps);
		m_flTimeWeaponIdle = m_flNextPrimaryAttack;
	}
	else {
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.7;//no longer indicate fps :)
	}

	UpdateSpot();
}

//=========================================================
// SecondaryAttack
//=========================================================
void CRpg::SecondaryAttack() {
	if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && !m_iClip) {
		return;
	}

	m_iOverloadLevel = !m_iOverloadLevel;
	if (!m_iOverloadLevel && m_pSpot) {
		ShutdownScreen();//simply call shutdown function
	}

	m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.3;
}

//=========================================================
// Deploy
//=========================================================
BOOL CRpg::Deploy() {
	return DefaultDeploy("models/v_rpg.mdl", "models/p_rpg.mdl", (int)RPG_DRAW::sequence,
		"rpg", CalculateWeaponTime((int)RPG_DRAW::frames, (int)RPG_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CRpg::Holster() {
	pev->body = (m_iClip ? 0 : 1);
	ShutdownScreen();//set skin to 0 manually
	m_fInReload = FALSE;// cancel any reload in progress.
	SendWeaponAnim((int)RPG_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)RPG_HOLSTER::frames, (int)RPG_HOLSTER::fps);
}

//=========================================================
// Reload
//=========================================================
void CRpg::Reload(void) {
	if ((m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0) ||
		(m_iClip == 1) ||
		(m_iChargeLevel && m_iOverloadLevel)) {
		return;
	}

	if (m_iClip == 0) {
		if (m_pSpot && m_iOverloadLevel)
			m_pSpot->Suspend(2.1);

		if (m_pMirSpot && m_iOverloadLevel)
			m_pMirSpot->Suspend(2.1);

		pev->body = 0;//show rocket
		DefaultReload(RPG_MAX_CLIP, (int)RPG_RELOAD::sequence,
			CalculateWeaponTime((int)RPG_RELOAD::frames, (int)RPG_RELOAD::fps));

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
	}
}

//=========================================================
// UpdateSpot
//=========================================================
void CRpg::UpdateSpot(void) {
	if (m_iOverloadLevel) {
		if (!m_pSpot) {
			m_pSpot = CLaserSpot::CreateSpot();
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_on.wav", VOL_NORM, ATTN_NORM);
			m_pPlayer->m_fSpotActive = true;
		}

		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		Vector vecSrc = m_pPlayer->GetGunPosition();;
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

	UpdateScreen();// update rocket screen
}

//=========================================================
// UpdateScreen
//=========================================================
void CRpg::UpdateScreen(void) {
	if (m_flTimeUpdate > UTIL_GlobalTimeBase()) return;

	if (m_pSpot) {
		if (m_iChargeLevel) {
			if (pev->skin >= 4)
				pev->skin = 1;

			pev->skin++;
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/beep.wav", VOL_LOW, ATTN_NORM);
		}
		else {
			pev->skin = 5;
			if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && !m_iClip) {
				m_iOverloadLevel = false;
				if (!m_iOverloadLevel && m_pSpot) {
					ShutdownScreen();//simply call shutdown function
				}
			}
		}
	}
	else
		pev->skin = 0;

	m_flTimeUpdate = UTIL_GlobalTimeBase() + 0.3;
}

//=========================================================
// ShutdownScreen
//=========================================================
void CRpg::ShutdownScreen(void) {
	pev->skin = 0;
	if (m_pSpot) {
		m_pSpot->Killed(NULL, GIB_NEVER);
		m_pSpot = NULL;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_off.wav", VOL_LOW, ATTN_NORM);
		m_pPlayer->m_fSpotActive = false;
	}

	if (m_pMirSpot) {
		m_pMirSpot->Killed(NULL, GIB_NEVER);
		m_pMirSpot = NULL;
	}
}

//=========================================================
// WeaponIdle
//=========================================================
void CRpg::WeaponIdle(void) {
	UpdateSpot(); //m_iSkin;
	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase()) return;

	if (m_iClip && !m_iOverloadLevel) {
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5) {
			iAnim = (int)RPG_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)RPG_IDLE1::frames, (int)RPG_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.7) {
			iAnim = (int)RPG_IDLE2::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)RPG_IDLE2::frames, (int)RPG_IDLE2::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.9) {
			iAnim = (int)RPG_FIDGET::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)RPG_FIDGET::frames, (int)RPG_FIDGET::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else {
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
		}

		SendWeaponAnim(iAnim);
	}
}