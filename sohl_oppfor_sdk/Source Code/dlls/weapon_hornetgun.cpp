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
// Weapon: Hivehand * http://half-life.wikia.com/wiki/Hivehand
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "proj_hornet.h"
#include "gamerules.h"
#include "weapon_hornetgun.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_hornetgun, CHgun);

//=========================================================
// Spawn Glock 17
//=========================================================
void CHgun::Spawn(void) {
	Precache();
	SET_MODEL(ENT(pev), "models/w_hgun.mdl");

	m_iDefaultAmmo = HIVEHAND_DEFAULT_GIVE;
	m_iFirePhase = 0;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CHgun::Precache(void) {
	PRECACHE_MODEL("models/v_hgun.mdl");
	PRECACHE_MODEL("models/w_hgun.mdl");
	PRECACHE_MODEL("models/p_hgun.mdl");

	UTIL_PrecacheOther("hornet");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CHgun::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hornets";
	p->iMaxAmmo1 = HORNET_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_HORNETGUN;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iWeight = HORNETGUN_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CHgun::PrimaryAttack(void) {
	Reload();
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
		return;
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	CBaseEntity *pHornet = CBaseEntity::Create("hornet", m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict());
	pHornet->pev->velocity = gpGlobals->v_forward * 300;

	m_flRechargeTime = UTIL_GlobalTimeBase() + 0.5;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	SendWeaponAnim((int)HGUN_SHOOT::sequence);

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	m_flNextPrimaryAttack = m_flNextPrimaryAttack + 0.25;

	if (m_flNextPrimaryAttack < UTIL_GlobalTimeBase()) {
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.25;
	}

	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_LONG(10, 15);
}

//=========================================================
// SecondaryAttack
//=========================================================
void CHgun::SecondaryAttack(void) {
	Reload();
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
		return;
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12;

	m_iFirePhase++;
	switch (m_iFirePhase) {
	case 1:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		break;
	case 2:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 3:
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 4:
		vecSrc = vecSrc + gpGlobals->v_up * -8;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 5:
		vecSrc = vecSrc + gpGlobals->v_up * -8;
		break;
	case 6:
		vecSrc = vecSrc + gpGlobals->v_up * -8;
		vecSrc = vecSrc + gpGlobals->v_right * -8;
		break;
	case 7:
		vecSrc = vecSrc + gpGlobals->v_right * -8;
		break;
	case 8:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		vecSrc = vecSrc + gpGlobals->v_right * -8;
		m_iFirePhase = 0;
		break;
	}

	CBaseEntity *pHornet;
	pHornet = CBaseEntity::Create("hornet", vecSrc, m_pPlayer->pev->v_angle, m_pPlayer->edict());
	pHornet->pev->velocity = gpGlobals->v_forward * 1200;
	pHornet->pev->angles = UTIL_VecToAngles(pHornet->pev->velocity);

	pHornet->SetThink(&CHornet::StartDart);
	m_flRechargeTime = UTIL_GlobalTimeBase() + 0.5;

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	SendWeaponAnim((int)HGUN_SHOOT::sequence);

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.1;
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_LONG(10, 15);
	m_pPlayer->pev->punchangle.x = RANDOM_FLOAT(0, 2);
}

//=========================================================
// Deploy
//=========================================================
BOOL CHgun::Deploy(void) {
	return DefaultDeploy("models/v_hgun.mdl", "models/p_hgun.mdl", (int)HGUN_UP::sequence,
		"hive", CalculateWeaponTime((int)HGUN_UP::frames, (int)HGUN_UP::fps));
}

//=========================================================
// Holster
//=========================================================
void CHgun::Holster(void) {
	SendWeaponAnim((int)HGUN_DOWN::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)HGUN_DOWN::frames, (int)HGUN_DOWN::fps);

	if (!m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()])
		m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] = 1;
}

//=========================================================
// Reload
//=========================================================
void CHgun::Reload(void) {
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == HORNET_MAX_CARRY) {
		return;
	}

	while (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < HORNET_MAX_CARRY && m_flRechargeTime < UTIL_GlobalTimeBase()) {
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		m_flRechargeTime += 0.5;
	}
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CHgun::WeaponIdle(void) {
	Reload();
	float flTime = 0.0;
	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
		return;
	}

	// only idle if the slid isn't back
	int iAnim;
	float flRand = RANDOM_FLOAT(0, 1);
	if (flRand <= 0.5) {
		iAnim = (int)HGUN_IDLE1::sequence;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)HGUN_IDLE1::frames, (int)HGUN_IDLE1::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	}
	else if (flRand <= 0.7) {
		iAnim = (int)HGUN_FIDGETSWAY::sequence;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)HGUN_FIDGETSWAY::frames, (int)HGUN_FIDGETSWAY::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	}
	else if (flRand <= 0.9) {
		iAnim = (int)HGUN_FIDGETSHAKE::sequence;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)HGUN_FIDGETSHAKE::frames, (int)HGUN_FIDGETSHAKE::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	}
	else {
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
		m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
	}

	SendWeaponAnim(iAnim);
}
