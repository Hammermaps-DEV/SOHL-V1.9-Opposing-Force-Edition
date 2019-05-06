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
// Weapon: Mk 2 Grenade * http://half-life.wikia.com/wiki/Mk_2_Grenade
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "proj_grenade.h"
#include "weapon_handgrenade.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_handgrenade, CHandGrenade);

//=========================================================
// Spawn Mk 2 Grenade
//=========================================================
void CHandGrenade::Spawn(void) {
	Precache();

	SET_MODEL(ENT(pev), "models/w_grenade.mdl");
	m_iDefaultAmmo = HANDGRENADE_DEFAULT_GIVE;
	pev->dmg = gSkillData.plrDmgHandGrenade;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CHandGrenade::Precache(void) {
	PRECACHE_MODEL("models/w_grenade.mdl");
	PRECACHE_MODEL("models/v_grenade.mdl");
	PRECACHE_MODEL("models/p_grenade.mdl");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CHandGrenade::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hand Grenade";
	p->iMaxAmmo1 = HANDGRENADE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_HANDGRENADE;
	p->iWeight = HANDGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CHandGrenade::PrimaryAttack(void) {
	if (!m_flStartThrow && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0) {
		m_flStartThrow = UTIL_GlobalTimeBase();
		m_flReleaseThrow = 0;

		SendWeaponAnim((int)HANDGRENADE_PINPULL::sequence);
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)HANDGRENADE_PINPULL::frames, (int)HANDGRENADE_PINPULL::fps);
	}
}

//=========================================================
// Deploy
//=========================================================
BOOL CHandGrenade::Deploy(void) {
	m_iChargeLevel = 0;
	return DefaultDeploy("models/v_grenade.mdl", "models/p_grenade.mdl", (int)HANDGRENADE_DRAW::sequence,
		"crowbar", CalculateWeaponTime((int)HANDGRENADE_DRAW::frames, (int)HANDGRENADE_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CHandGrenade::Holster(void) {
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() + 0.5;
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]) {
		SendWeaponAnim((int)HANDGRENADE_HOLSTER::sequence);
		m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)HANDGRENADE_HOLSTER::frames, (int)HANDGRENADE_HOLSTER::fps);
	}
	else {
		// no more grenades!
		m_pPlayer->pev->weapons &= ~(1 << WEAPON_HANDGRENADE);
		SetThink(&CHandGrenade::DestroyItem);
		SetNextThink(0.1);
	}

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", VOL_NORM, ATTN_NORM);
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CHandGrenade::WeaponIdle(void) {
	if (m_flTimeUpdate < UTIL_GlobalTimeBase() && m_iChargeLevel) {
		// we've finished the throw, restart.
		m_flStartThrow = 0;
		m_iChargeLevel = 0;
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			SendWeaponAnim((int)HANDGRENADE_DRAW::sequence);
		else {
			RetireWeapon();
			return;
		}

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_LONG(10, 15);
		return;
	}

	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase()) return;

	if (m_flStartThrow) {
		Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if (angThrow.x < 0)
			angThrow.x = -10 + angThrow.x * ((90 - 10) / 90.0);
		else
			angThrow.x = -10 + angThrow.x * ((90 + 10) / 90.0);

		float flVel = (90 - angThrow.x) * 4;
		if (flVel > 500) flVel = 500;

		UTIL_MakeVectors(angThrow);
		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 22;
		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;

		// alway explode 3 seconds after the pin was pulled
		float time = m_flStartThrow - UTIL_GlobalTimeBase() + 3.0;
		if (time < 0) time = 0;

		CGrenade::ShootTimed(m_pPlayer->pev, vecSrc, vecThrow, time);

		if (time >= 2)
			SendWeaponAnim((int)HANDGRENADE_THROW1::sequence);
		else if (time <= 2)
			SendWeaponAnim((int)HANDGRENADE_THROW2::sequence);
		else
			SendWeaponAnim((int)HANDGRENADE_THROW3::sequence);

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		m_flStartThrow = 0;
		m_iChargeLevel = 1;
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.5;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]) {
			// just threw last grenade
			// set attack times in the future, and weapon idle in the future so we can see the whole throw
			// animation, weapon idle will automatically retire the weapon for us.
			m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.5;// ensure that the animation can finish playing
		}

		m_flTimeUpdate = m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 1.0;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 3.0;
		return;
	}

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]) {
		if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
			m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
			return;
		}

		// only idle if the slid isn't back
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5) {
			iAnim = (int)HANDGRENADE_IDLE::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)HANDGRENADE_IDLE::frames, (int)HANDGRENADE_IDLE::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else if (flRand <= 0.7) {
			iAnim = (int)HANDGRENADE_FIDGET::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)HANDGRENADE_FIDGET::frames, (int)HANDGRENADE_FIDGET::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		}
		else {
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
		}

		SendWeaponAnim(iAnim);
	}
}



