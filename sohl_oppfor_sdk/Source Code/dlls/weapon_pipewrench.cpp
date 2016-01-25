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
// Weapon: Pipe Wrench * http://half-life.wikia.com/wiki/Pipe_Wrench
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00001 / Date: 25.01.2016
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "weapon_pipewrench.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_pipewrench, CPipeWrench);

//=========================================================
// Spawn Pipe Wrench
//=========================================================
void CPipeWrench::Spawn(void) {
	Precache();
	m_iId = WEAPON_PIPEWRENCH;

	SET_MODEL(ENT(pev), "models/w_pipe_wrench.mdl");
	m_iClip = -1;
	m_fullcharge = 10;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CPipeWrench::Precache(void) {
	PRECACHE_MODEL("models/v_pipe_wrench.mdl");
	PRECACHE_MODEL("models/w_pipe_wrench.mdl");
	PRECACHE_MODEL("models/p_pipe_wrench.mdl");

	PRECACHE_SOUND("weapons/pwrench_miss1.wav");
	PRECACHE_SOUND("weapons/pwrench_miss2.wav");
	PRECACHE_SOUND("weapons/pwrench_hitbod1.wav");
	PRECACHE_SOUND("weapons/pwrench_hitbod2.wav");
	PRECACHE_SOUND("weapons/pwrench_hitbod3.wav");
	PRECACHE_SOUND("weapons/pwrench_big_miss.wav");
	PRECACHE_SOUND("weapons/pwrench_big_hitbod1.wav");
	PRECACHE_SOUND("weapons/pwrench_big_hitbod2.wav");
	PRECACHE_SOUND("weapons/pwrench_hit1.wav");
	PRECACHE_SOUND("weapons/pwrench_hit2.wav");

	m_usWrenchSmall = PRECACHE_EVENT(1, "events/wrench_small.sc");
	m_usWrenchLarge = PRECACHE_EVENT(1, "events/wrench_large.sc");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CPipeWrench::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = WEAPON_PIPEWRENCH;
	p->iWeight = PIPEWRENCH_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack - Call the frist Attack
//=========================================================
void CPipeWrench::PrimaryAttack() {
	if (!CPipeWrench::Swing(TRUE)) {
		CPipeWrench::Swing(FALSE);
	}

	m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.2;
}

//=========================================================
// SecondaryAttack - Call the second Attack
//=========================================================
void CPipeWrench::SecondaryAttack() {
	if (m_fInAttack == 0) {
		m_fPrimaryFire = FALSE;
		SendWeaponAnim((int)PIPE_WRENCH_ATTACKBIGWIND::sequence);
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
		m_flChargeTime = UTIL_WeaponTimeBase();
		m_flAmmoStartCharge = UTIL_WeaponTimeBase() + m_fullcharge;
	} else if (m_fInAttack == 1) {
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase()) {
			SendWeaponAnim((int)PIPE_WRENCH_ATTACKBIGLOOP::sequence);
			m_fInAttack = 2;
		}
	}
}

//=========================================================
// StartFire- since all of this code has to run and then 
// call Fire(), it was easier at this point to rip it out 
// of weaponidle() and make its own function then to try to
// merge this into Fire(), which has some identical variable names 
//=========================================================
void CPipeWrench::StartLargeSwing(void) {
	float flDamage;
	if ((UTIL_WeaponTimeBase()-m_flChargeTime) >= m_fullcharge) {
		flDamage = 200;
	} else {
		flDamage = 200 * ((UTIL_WeaponTimeBase() - m_flChargeTime) / m_fullcharge);
	}

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	LargeSwing(flDamage);
}

//=========================================================
// Large Swing * SecondaryAttack
//=========================================================
int CPipeWrench::LargeSwing(float flDamage) {
	int fDidHit = FALSE;
	bHit = FALSE;
	TraceResult tr;

	if (m_flTimeUpdate > UTIL_WeaponTimeBase()) {
		return fDidHit;
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

	if (tr.flFraction >= 1.0) {
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);
		if (tr.flFraction < 1.0) {
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict());

			vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}

	if (tr.flFraction >= 1.0)
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	else {
		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		// hit
		fDidHit = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		ClearMultiDamage();
		pEntity->TraceAttack(m_pPlayer->pev, flDamage, gpGlobals->v_forward, &tr, DMG_CLUB);
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		// play thwack, smack, or dong sound
		float flVol = 1.0;

		if (pEntity) {
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE) {
				m_pPlayer->m_iWeaponVolume = PIPE_WRENCH_WALLHIT_VOLUME;
				bHit = TRUE;//play hitbody sound on client
				if (!pEntity->IsAlive()) {
					m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
				} else
					flVol = 0.1;
			}

			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() +
				(CalculateWeaponTime((int)PIPE_WRENCH_ATTACKBIGMISS::frames, (int)PIPE_WRENCH_ATTACKBIGMISS::fps));
		} else {
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() +
				(CalculateWeaponTime((int)PIPE_WRENCH_ATTACKBIGHIT::frames, (int)PIPE_WRENCH_ATTACKBIGHIT::fps));
		}

		m_pPlayer->m_iWeaponVolume = flVol * PIPE_WRENCH_BODYHIT_VOLUME;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + RANDOM_FLOAT(10, 15);
		m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.2;
	}

	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usWrenchLarge, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, pev->body, 0, bHit, 0);
	return fDidHit;
}

//=========================================================
// Easy Swing * PrimaryAttack
//=========================================================
int CPipeWrench::Swing(int fFirst) {
	int fDidHit = FALSE;
	bHit = FALSE;
	TraceResult tr;

	if (m_flTimeUpdate > UTIL_WeaponTimeBase()) {
		return fDidHit;
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;
	int AttackAnimation = RANDOM_LONG(0, 2);

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

	if (tr.flFraction >= 1.0) {
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);
		if (tr.flFraction < 1.0) {
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict());

			vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}

	if (tr.flFraction >= 1.0 && fFirst)
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	else {
		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		// hit
		fDidHit = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		ClearMultiDamage();
		if ((m_flNextPrimaryAttack + 2 < UTIL_WeaponTimeBase()) || g_pGameRules->IsMultiplayer()) {
			// first swing does full damage
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgWrench, gpGlobals->v_forward, &tr, DMG_CLUB);
		} else {
			// subsequent swings do half
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgWrench / 2, gpGlobals->v_forward, &tr, DMG_CLUB);
		}
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		if (pEntity) {
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE) {
				bHit = TRUE;//play hitbody sound on client
				m_pPlayer->m_iWeaponVolume = PIPE_WRENCH_BODYHIT_VOLUME;
				if (!pEntity->IsAlive()) {
					m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
				}
				else
					flVol = 0.1;

				fHitWorld = FALSE;
			}
		}

		m_pPlayer->m_iWeaponVolume = flVol * PIPE_WRENCH_WALLHIT_VOLUME;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + RANDOM_FLOAT(10, 15);
		switch (AttackAnimation) {
			case 0:
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() +
					(CalculateWeaponTime((int)PIPE_WRENCH_ATTACK1::frames, (int)PIPE_WRENCH_ATTACK1::fps) / 100 * PIPE_WRENCH_ATTACK_BOOST);
			break;
			case 1:
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() +
					(CalculateWeaponTime((int)PIPE_WRENCH_ATTACK2::frames, (int)PIPE_WRENCH_ATTACK2::fps) / 100 * PIPE_WRENCH_ATTACK_BOOST);
			break;
			case 2:
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() +
					(CalculateWeaponTime((int)PIPE_WRENCH_ATTACK3::frames, (int)PIPE_WRENCH_ATTACK3::fps) / 100 * PIPE_WRENCH_ATTACK_BOOST);
			break;
		}

		m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.2;
	}

	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usWrenchSmall, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, pev->body, fFirst, bHit, AttackAnimation);
	return fDidHit;
}

//=========================================================
// Deploy
//=========================================================
BOOL CPipeWrench::Deploy(void) {
	return DefaultDeploy("models/v_pipe_wrench.mdl", "models/p_pipe_wrench.mdl", (int)PIPE_WRENCH_DRAW::sequence,
		"crowbar", CalculateWeaponTime((int)PIPE_WRENCH_DRAW::frames, (int)PIPE_WRENCH_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CPipeWrench::Holster(void) {
	SendWeaponAnim((int)PIPE_WRENCH_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() +
		CalculateWeaponTime((int)PIPE_WRENCH_HOLSTER::frames, (int)PIPE_WRENCH_HOLSTER::fps);
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CPipeWrench::WeaponIdle(void) {
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase()) {
		return;
	}

	if (m_fInAttack != 0) {
		CPipeWrench::StartLargeSwing();
		m_fInAttack = 0;
	} else {
		if (m_flTimeWeaponIdleLock > UTIL_WeaponTimeBase()) {
			return;
		}

		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5) {
			iAnim = (int)PIPE_WRENCH_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() +
				CalculateWeaponTime((int)PIPE_WRENCH_IDLE1::frames, (int)PIPE_WRENCH_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.7) {
			iAnim = (int)PIPE_WRENCH_IDLE2::sequence;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() +
				CalculateWeaponTime((int)PIPE_WRENCH_IDLE2::frames, (int)PIPE_WRENCH_IDLE2::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.9) {
			iAnim = (int)PIPE_WRENCH_IDLE3::sequence;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() +
				CalculateWeaponTime((int)PIPE_WRENCH_IDLE3::frames, (int)PIPE_WRENCH_IDLE3::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else {
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_WeaponTimeBase();
		}

		SendWeaponAnim(iAnim);
	}
}