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
// Weapon: Displacer * http://half-life.wikia.com/wiki/Displacer_Cannon
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "shake.h"
#include "proj_portal.h"
#include "weapon_displacer.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(info_displacer_xen_target, CPointEntity);
LINK_ENTITY_TO_CLASS(info_displacer_earth_target, CPointEntity);
LINK_ENTITY_TO_CLASS(weapon_displacer, CDisplacer);

//=========================================================
// Spawn Displacer
//=========================================================
void CDisplacer::Spawn(void) {
	Precache();

	SET_MODEL(ENT(pev), "models/w_displacer.mdl");
	m_iDefaultAmmo = EGON_DEFAULT_GIVE;

	m_iFireState = FIRESTATE_NONE;
	m_iFireMode = FIREMODE_NONE;

	//info_displacer_earth_target
	edict_t* pEnt = NULL;
	pEnt = FIND_ENTITY_BY_CLASSNAME(pEnt, "info_displacer_earth_target");

	if (pEnt) {
		m_hTargetEarth = GetClassPtr((CBaseEntity *)VARS(pEnt));
	} else {
		ALERT(at_console,"ERROR: Couldn't find entity with classname %s\n","info_displacer_earth_target");
	}

	//info_displacer_xen_target
	pEnt = NULL;
	pEnt = FIND_ENTITY_BY_CLASSNAME(pEnt, "info_displacer_xen_target");

	if (pEnt) {
		m_hTargetXen = GetClassPtr((CBaseEntity *)VARS(pEnt));
	} else {
		ALERT(at_console,"ERROR: Couldn't find entity with classname %s\n","info_displacer_xen_target");
	}

	FallInit();// get ready to fall down.
}

//=========================================================
// GetItemInfo
//=========================================================
int CDisplacer::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = 0;
	p->iSlot = 5;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_DISPLACER;
	p->iWeight = DISPLACER_WEIGHT;
	return 1;
}

//=========================================================
// Precache
//=========================================================
void CDisplacer::Precache(void) {
	PRECACHE_MODEL("models/v_displacer.mdl");
	PRECACHE_MODEL("models/w_displacer.mdl");
	PRECACHE_MODEL("models/p_displacer.mdl");

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/displacer_fire.wav");
	PRECACHE_SOUND("weapons/displacer_self.wav");
	PRECACHE_SOUND("weapons/displacer_spin.wav");
	PRECACHE_SOUND("weapons/displacer_spin2.wav");
	PRECACHE_SOUND("weapons/displacer_start.wav");

	PRECACHE_MODEL("sprites/plasma.spr");

	m_usDisplacer = PRECACHE_EVENT(1, "events/displacer.sc");
}

//=========================================================
// Deploy
//=========================================================
BOOL CDisplacer::Deploy(void) {
	return DefaultDeploy("models/v_displacer.mdl", "models/p_displacer.mdl", (int)DISPLACER_DRAW::sequence,
		"gauss", CalculateWeaponTime((int)DISPLACER_DRAW::frames, (int)DISPLACER_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CDisplacer::Holster(void) {
	ClearSpin();
	m_fInReload = FALSE;// cancel any reload in progress.
	SendWeaponAnim((int)DISPLACER_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)DISPLACER_HOLSTER::frames, (int)DISPLACER_HOLSTER::fps);
}

//=========================================================
// SecondaryAttack
//=========================================================
void CDisplacer::SecondaryAttack(void) {
	if (m_pPlayer->pev->waterlevel == 3) {
		PlayEmptySound(2);
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.3f;
		return;
	}

	if (m_fFireOnEmpty || (!HasAmmo() || !CanFireDisplacer())) {
		PlayEmptySound(2);
		m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.3f;
		return;
	}

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	m_iFireState = FIRESTATE_SPINUP;
	m_iFireMode = FIREMODE_BACKWARD;

	m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.1f;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CDisplacer::PrimaryAttack(void) {
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3) {
		PlayEmptySound(2);
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.3f;
		return;
	}

	if (m_fFireOnEmpty || (!HasAmmo() || !CanFireDisplacer())) {
		PlayEmptySound(2);
		m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.3f;
		return;
	}

	m_iFireState = FIRESTATE_SPINUP;
	m_iFireMode = FIREMODE_FORWARD;

	m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.1f;
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CDisplacer::WeaponIdle(void) {
	ResetEmptySound();
	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase())
		return;

	if (m_iFireState != FIRESTATE_NONE) {
		switch (m_iFireState) {
			case FIRESTATE_SPINUP: {
			// Launch spinup sequence.
			SpinUp(m_iFireMode);
			}
			break;
			case FIRESTATE_SPIN: {
				Spin();
			}
			break;
			case FIRESTATE_FIRE: {
				Fire(m_iFireMode == FIREMODE_FORWARD ? TRUE : FALSE);
			}
			break;
		}

		return;
	}

	if (m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
		return;
	}

	int iAnim;
	float flRand = RANDOM_FLOAT(0, 1);
	if (flRand <= 0.5) {
		iAnim = (int)DISPLACER_IDLE1::sequence;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)DISPLACER_IDLE1::frames, (int)DISPLACER_IDLE1::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	} else if (flRand <= 0.7) {
		iAnim = (int)DISPLACER_IDLE2::sequence;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)DISPLACER_IDLE2::frames, (int)DISPLACER_IDLE2::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	} else {
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
		m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
	}

	SendWeaponAnim(iAnim);
}

//=========================================================
// ClearSpin
//=========================================================
void CDisplacer::ClearSpin(void) {

	switch (m_iFireMode) {
		case FIREMODE_FORWARD:
			STOP_SOUND(ENT(pev), CHAN_WEAPON, "weapons/displacer_spin.wav");
		break;
		case FIREMODE_BACKWARD:
			STOP_SOUND(ENT(pev), CHAN_WEAPON, "weapons/displacer_spin2.wav");
		break;
	}

	m_iFireState = FIRESTATE_NONE;
	m_iFireMode = FIREMODE_NONE;
}

//=========================================================
// SpinUp
//=========================================================
void CDisplacer::SpinUp(int iFireMode) {
	PLAYBACK_EVENT_FULL(0,m_pPlayer->edict(),m_usDisplacer,0.0,(float *)&g_vecZero,(float *)&g_vecZero,0.0,0.0,(int)DISPLACER_SPINUP::sequence,iFireMode,0,0);

	m_iFireState = FIRESTATE_FIRE;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.7f;
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.7f;
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() + 1.0f;
}

//=========================================================
// Spin
//=========================================================
void CDisplacer::Spin(void) {
	PLAYBACK_EVENT_FULL(0,m_pPlayer->edict(),m_usDisplacer,0.0,(float *)&g_vecZero,(float *)&g_vecZero,0.0,0.0,(int)DISPLACER_SPIN::sequence,0,0,0);

	m_iFireState = FIRESTATE_FIRE;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.7f;
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.7f;
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() + 1.0f;
}

//=========================================================
// Purpose:
//=========================================================
void CDisplacer::Fire(BOOL fIsPrimary)
{
	if (fIsPrimary) {
		// Use the firemode 1, which launches a portal forward.
		Displace();
	} else {
		// Use firemode 2, which teleports the current owner.
		Teleport();
	}

	ClearSpin();

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.7f;
	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.7f;
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() + 0.7f;
}

//=========================================================
// Purpose:
//=========================================================
void CDisplacer::Displace(void) {
	PLAYBACK_EVENT_FULL(0,m_pPlayer->edict(),m_usDisplacer,0.0,(float *)&g_vecZero,(float *)&g_vecZero,0.0,0.0,(int)DISPLACER_FIRE::sequence,FIREMODE_FORWARD,0,0);

	Vector vecSrc;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	vecSrc = m_pPlayer->GetGunPosition();
	vecSrc = vecSrc + gpGlobals->v_forward * 16;
	vecSrc = vecSrc + gpGlobals->v_right * 8;
	vecSrc = vecSrc + gpGlobals->v_up * -12;

	CPortal::Shoot(pev, vecSrc, gpGlobals->v_forward * 550);

	// Decrement weapon ammunition.
	UseAmmo(EGON_DEFAULT_GIVE);
}

//=========================================================
// Purpose:
//=========================================================
void CDisplacer::Teleport(void)
{
	edict_t* pEnt_earth = FIND_ENTITY_BY_CLASSNAME(NULL, "info_displacer_earth_target");
	edict_t* pEnt_xen = FIND_ENTITY_BY_CLASSNAME(NULL, "info_displacer_xen_target");
	if (FNullEnt(pEnt_earth) || FNullEnt(pEnt_xen)) {
		PlayEmptySound(2);
		m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.3f;
		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usDisplacer, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, (int)DISPLACER_FIRE::sequence, FIREMODE_BACKWARD, 1, 0);
		return;
	}

	Vector vecSrc = m_pPlayer->pev->origin;
	ASSERT(m_hTargetEarth != NULL && m_hTargetXen);

	CBaseEntity* pTarget = (!m_pPlayer->m_fInXen)
		? m_hTargetXen
		: m_hTargetEarth;

	Vector tmp = pTarget->pev->origin;

	// make origin adjustments (origin in center, not at feet)
	tmp.z -= m_pPlayer->pev->mins.z;
	tmp.z++;

	m_pPlayer->pev->flags &= ~FL_ONGROUND;

	UTIL_SetOrigin(m_pPlayer, tmp);

	m_pPlayer->pev->angles = pTarget->pev->angles;
	m_pPlayer->pev->v_angle = pTarget->pev->angles;
	m_pPlayer->pev->fixangle = TRUE;
	m_pPlayer->pev->velocity = m_pPlayer->pev->basevelocity = g_vecZero;

	UTIL_ScreenFade(m_pPlayer, Vector(0, 200, 0), 2.5, 0.5, 128, FFADE_IN);

	vecSrc = pTarget->pev->origin;
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE(24);		// radius * 0.1
		WRITE_BYTE(255);		// r
		WRITE_BYTE(180);		// g
		WRITE_BYTE(96);		// b
		WRITE_BYTE(31.66);		// time * 10
		WRITE_BYTE(1);		// decay * 0.1
	MESSAGE_END();

	m_pPlayer->m_fInXen = !m_pPlayer->m_fInXen;

	if (m_pPlayer->m_fInXen) {
		m_pPlayer->pev->gravity = 0.5;
	} else {
		m_pPlayer->pev->gravity = 1.0;
	}

	// Decrement weapon ammunition.
	UseAmmo((EGON_DEFAULT_GIVE * 3));

	// Used to play teleport sound.
	PLAYBACK_EVENT_FULL(0,m_pPlayer->edict(),m_usDisplacer,0.0,(float *)&g_vecZero,(float *)&g_vecZero,0.0,0.0,(int)DISPLACER_FIRE::sequence,FIREMODE_BACKWARD,0,0);
}

//=========================================================
// ShouldUpdateEffects
//=========================================================
BOOL CDisplacer::ShouldUpdateEffects(void) const {
	return (m_iFireState != FIRESTATE_NONE);
}

//=========================================================
// HasAmmo
//=========================================================
BOOL CDisplacer::HasAmmo(void) {
	if (m_pPlayer->ammo_uranium <= 0)
		return FALSE;

	return TRUE;
}

//=========================================================
// UseAmmo
//=========================================================
void CDisplacer::UseAmmo(int count) {
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= count)
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= count;
	else
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

//=========================================================
// CanFireDisplacer
//=========================================================
BOOL CDisplacer::CanFireDisplacer() const {
	return m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= EGON_DEFAULT_GIVE;
}