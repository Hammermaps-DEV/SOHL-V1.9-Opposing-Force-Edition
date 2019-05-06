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
// Weapon: Knife * http://half-life.wikia.com/wiki/Combat_Knife
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "weapon_knife.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_knife, CKnife);

//=========================================================
// Spawn Crowbar
//=========================================================
void CKnife::Spawn() {
	Precache();
	m_iId = WEAPON_KNIFE;

	SET_MODEL(ENT(pev), "models/w_knife.mdl");
	m_iClip = -1;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CKnife::Precache(void) {
	PRECACHE_MODEL("models/v_knife.mdl");
	PRECACHE_MODEL("models/w_knife.mdl");
	PRECACHE_MODEL("models/p_knife.mdl");

	PRECACHE_SOUND("weapons/knife_hit_flesh1.wav");
	PRECACHE_SOUND("weapons/knife_hit_flesh2.wav");
	PRECACHE_SOUND("weapons/knife_hit_wall1.wav");
	PRECACHE_SOUND("weapons/knife_hit_wall2.wav");
	PRECACHE_SOUND("weapons/knife1.wav");
	PRECACHE_SOUND("weapons/knife2.wav");
	PRECACHE_SOUND("weapons/knife3.wav");

	m_usKnife = PRECACHE_EVENT(1, "events/knife.sc");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CKnife::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 2;
	p->iId = WEAPON_KNIFE;
	p->iWeight = KNIFE_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack - Call the frist Attack
//=========================================================
void CKnife::PrimaryAttack() {
	if (m_fInAttack == 0) {
		if (!Swing(TRUE)) {
			Swing(FALSE);
		}

		m_flTimeUpdate = UTIL_GlobalTimeBase() + 0.2;
	}
}

//=========================================================
// SecondaryAttack - Call the second attack
//=========================================================
void CKnife::SecondaryAttack() {
	Charge(false);
}

//=========================================================
// Charge - This used by PrimaryAttack
//=========================================================
void CKnife::Charge(bool Swing) {
	if (!Swing) {
		if (m_fInAttack == 0) {
			PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usKnife, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 1, 0, pev->body, 0, 0, 0);
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + CalculateWeaponTime((int)KNIFE_CHARGE::frames, (int)KNIFE_CHARGE::fps);
			m_fInAttack = 1;
		}
		else if (m_fInAttack == 1) {
			if (m_flTimeWeaponIdle < UTIL_GlobalTimeBase()) {
				m_fInAttack = 2;
			}
		}
	}
	else {
		if (m_flTimeWeaponIdle < UTIL_GlobalTimeBase()) {
			TraceResult tr;
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

			// player "shoot" animation
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);

			// hit
			CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
			ClearMultiDamage();
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgKnifeCharge, gpGlobals->v_forward, &tr, DMG_CLUB | DMG_NEVERGIB);
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

			// play thwack, smack, or dong sound
			float flVol = 1.0;
			int fHitWorld = FALSE;

			if (pEntity) {
				if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE) {
					m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;
					if (!pEntity->IsAlive()) {
						m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() + 0.5;
					}
					else
						flVol = 0.1;

					fHitWorld = TRUE;
				}
			}

			m_pPlayer->m_iWeaponVolume = flVol * KNIFE_WALLHIT_VOLUME;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);

			PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usKnife, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 1, 0, pev->body, 1, fHitWorld, 0);
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + CalculateWeaponTime((int)KNIFE_STAB::frames, (int)KNIFE_STAB::fps);
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)KNIFE_STAB::frames, (int)KNIFE_STAB::fps);

			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 3.0;
		}
	}
}

//=========================================================
// Swing - This used by PrimaryAttack
//=========================================================
int CKnife::Swing(int fFirst) {
	int fDidHit = FALSE;
	TraceResult tr;

	if (m_flTimeUpdate > UTIL_GlobalTimeBase()) {
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
		pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgKnife, gpGlobals->v_forward, &tr, DMG_CLUB | DMG_NEVERGIB);
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		fHitWorld = FALSE;

		if (pEntity) {
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE) {
				m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;
				if (!pEntity->IsAlive()) {
					m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() + 0.5;
				}
				else
					flVol = 0.1;

				fHitWorld = TRUE;
			}
		}

		m_pPlayer->m_iWeaponVolume = flVol * KNIFE_WALLHIT_VOLUME;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
		switch (AttackAnimation) {
		case 0:
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() +
				(CalculateWeaponTime((int)KNIFE_ATTACK1::frames, (int)KNIFE_ATTACK1::fps) / 100 * KNIFE_ATTACK_BOOST);
			break;
		case 1:
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() +
				(CalculateWeaponTime((int)KNIFE_ATTACK2::frames, (int)KNIFE_ATTACK2::fps) / 100 * KNIFE_ATTACK_BOOST);
			break;
		case 2:
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() +
				(CalculateWeaponTime((int)KNIFE_ATTACK3::frames, (int)KNIFE_ATTACK3::fps) / 100 * KNIFE_ATTACK_BOOST);
			break;
		}

		m_flTimeUpdate = UTIL_GlobalTimeBase() + 0.2;
	}

	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usKnife, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, pev->body, fFirst, fHitWorld, AttackAnimation);
	return fDidHit;
}

//=========================================================
// Deploy
//=========================================================
BOOL CKnife::Deploy() {
	return DefaultDeploy("models/v_knife.mdl", "models/p_knife.mdl", (int)KNIFE_DRAW::sequence,
		"crowbar", CalculateWeaponTime((int)KNIFE_DRAW::frames, (int)KNIFE_DRAW::fps));
}

//=========================================================
// Holster

//=========================================================
void CKnife::Holster() {
	SendWeaponAnim((int)KNIFE_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)KNIFE_HOLSTER::frames, (int)KNIFE_HOLSTER::fps);
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CKnife::WeaponIdle(void) {
	if ((m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) && !m_fInAttack) {
		return;
	}

	if (m_fInAttack != 0) {
		Charge(true);
	}
	else {
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5) {
			iAnim = (int)KNIFE_IDLE1::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)KNIFE_IDLE1::frames, (int)KNIFE_IDLE1::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.7) {
			iAnim = (int)KNIFE_IDLE2::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)KNIFE_IDLE2::frames, (int)KNIFE_IDLE2::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.9) {
			iAnim = (int)KNIFE_IDLE3::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)KNIFE_IDLE3::frames, (int)KNIFE_IDLE3::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else {
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
		}

		SendWeaponAnim(iAnim);
	}
}



