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
*	Base Source-Code written by Half-Life Update MOD (https://github.com/Fograin/hl-subsmod-ex)
*   and Marc-Antoine Lortie (https://github.com/malortie).
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/
//=========================================================
// Weapon: Barnacle Grapple * 
// http://half-life.wikia.com/wiki/Barnacle_Grapple
//=========================================================

#include "extdll.h" 
#include "util.h" 
#include "cbase.h" 
#include "monsters.h" 
#include "weapons.h" 
#include "nodes.h" 
#include "player.h" 
#include "soundent.h" 
#include "shake.h" 
#include "gamerules.h"
#include "vector.h"
#include "proj_grapple_tonguetip.h"
#include "weapon_grapple.h"
#include "particle_defs.h"

extern int gmsgParticles;

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_grapple, CGrapple);

//=========================================================
// Spawn Grapple
//=========================================================
void CGrapple::Spawn(void) {
	Precache();

	SET_MODEL(ENT(pev), "models/w_bgrap.mdl");
	m_iDefaultAmmo = 1;

	FallInit();
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CGrapple::Precache(void) {
	PRECACHE_MODEL("models/v_bgrap.mdl");
	PRECACHE_MODEL("models/p_bgrap.mdl");
	PRECACHE_MODEL("models/w_bgrap.mdl");

	UTIL_PrecacheOther("proj_hook");

	PRECACHE_SOUND("weapons/bgrapple_pull.wav");
	PRECACHE_SOUND("weapons/bgrapple_fire.wav");
	PRECACHE_SOUND("weapons/bgrapple_release.wav");
	PRECACHE_SOUND("weapons/bgrapple_wait.wav");
	PRECACHE_SOUND("weapons/bgrapple_cough.wav");
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CGrapple::GetItemInfo(ItemInfo* p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 1;
	p->iSlot = 0;
	p->iPosition = 3;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_GRAPPLE;
	p->iWeight = GRAPPLE_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CGrapple::PrimaryAttack() {
	// Fograin92: If player already have a tongue
	if (m_pPlayer->m_pGrappleExists) //if player already has a grapple
		return;

	// Fograin92: If weapon is still on cooldown
	if (m_flNextPrimaryAttack > UTIL_GlobalTimeBase())
		return;

	SendWeaponAnim((int)GRAPPLE_FIRE::sequence);
	SetThink(&CGrapple::FlyThink);
	pev->nextthink = m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)GRAPPLE_FIRE::frames, (int)GRAPPLE_FIRE::fps);
	//0.58;

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_fire.wav", VOL_NORM, ATTN_NORM);
	m_pPlayer->m_pGrappleExists = 1;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector AimingDir = gpGlobals->v_forward;
	Vector GunPosition = m_pPlayer->GetGunPosition();

	GunPosition = GunPosition + gpGlobals->v_up * -4 + gpGlobals->v_right * 3 + gpGlobals->v_forward * 16;
	m_pPlayer->m_MyGrapple = CGrappleHook::Create(GunPosition, m_pPlayer->pev->v_angle, m_pPlayer);
	PrimaryAttackEnd = false; StartIdle = false;
}

//=========================================================
// FlyThink
//=========================================================
void CGrapple::FlyThink(void) {
	if (m_pPlayer->m_afPhysicsFlags & PFLAG_ON_GRAPPLE) {
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_pull.wav", VOL_NORM, ATTN_NORM);
		SendWeaponAnim((int)GRAPPLE_FIRETRAVEL::sequence);
		pev->nextthink = m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.6;
	}
	else if (!m_pPlayer->m_pGrappleExists && !PrimaryAttackEnd) {
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_release.wav", VOL_NORM, ATTN_NORM);

		SendWeaponAnim((int)GRAPPLE_FIRERELEASE::sequence);
		pev->nextthink = m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.98;

		PrimaryAttackEnd = true; StartIdle = true;
	}
	else if (!m_pPlayer->m_pGrappleExists && PrimaryAttackEnd && StartIdle) {
		SendWeaponAnim((int)GRAPPLE_BREATHE::sequence);
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)GRAPPLE_BREATHE::frames, (int)GRAPPLE_BREATHE::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);

		m_flNextPrimaryAttack = m_flNextPrimaryAttack + 0.5;
		if (m_flNextPrimaryAttack < UTIL_GlobalTimeBase())
			m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.25;
	}
	else if (m_pPlayer->m_pGrapplePullBack && !PrimaryAttackEnd) {
		SendWeaponAnim((int)GRAPPLE_FIREREACHED::sequence);
		pev->nextthink = m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.1;
	}
	else if (!m_pPlayer->m_pGrapplePullBack && !PrimaryAttackEnd) {
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_pull.wav", VOL_NORM, ATTN_NORM);
		SendWeaponAnim((int)GRAPPLE_FIREWAITING::sequence);
		pev->nextthink = m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 0.56;
	}
}

//=========================================================
// Deploy
//=========================================================
BOOL CGrapple::Deploy(void) {
	return DefaultDeploy("models/v_bgrap.mdl", "models/p_bgrap.mdl", (int)GRAPPLE_DRAW::sequence,
		"hive", CalculateWeaponTime((int)GRAPPLE_DRAW::frames, (int)GRAPPLE_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CGrapple::Holster(void) {
	StopSounds();	// Fograin92: Stop looped sounds
	SendWeaponAnim((int)GRAPPLE_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)GRAPPLE_HOLSTER::frames, (int)GRAPPLE_HOLSTER::fps);
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CGrapple::WeaponIdle(void) {
	float flTime = 0.0;
	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase()) {
		return;
	}

	StopSounds();
	if (m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
		return;
	}

	int iAnim = 0;
	float flRand = RANDOM_FLOAT(0, 1);
	if (flRand <= 0.3) {
		iAnim = (int)GRAPPLE_BREATHE::sequence;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)GRAPPLE_BREATHE::frames, (int)GRAPPLE_BREATHE::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	}
	else if (flRand <= 0.5) {
		iAnim = (int)GRAPPLE_LONGIDLE::sequence;
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)GRAPPLE_LONGIDLE::frames, (int)GRAPPLE_LONGIDLE::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	}
	else if (flRand <= 0.7) {
		iAnim = (int)GRAPPLE_SHORTIDLE::sequence;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_wait.wav", VOL_NORM, ATTN_NORM);
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)GRAPPLE_SHORTIDLE::frames, (int)GRAPPLE_SHORTIDLE::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	}
	else if (flRand <= 0.9) {
		iAnim = (int)GRAPPLE_COUGH::sequence;
		SetThink(&CGrapple::PukeGibs); // Fograin92: Second part of "cough" animation
		pev->nextthink = m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
			CalculateWeaponTime((int)100, (int)GRAPPLE_COUGH::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	}
	else {
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
		m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
	}

	SendWeaponAnim(iAnim);
}

//=========================================================
// StopSounds
//=========================================================
void CGrapple::StopSounds(void) {
	STOP_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_wait.wav");
	STOP_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_pull.wav");
}

//=========================================================
// PukeGibs
//=========================================================
void CGrapple::PukeGibs(void) {
	// Fograin92: Get proper XYZ values
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector GunPosition = m_pPlayer->GetGunPosition();
	GunPosition = GunPosition + gpGlobals->v_up * -15 + gpGlobals->v_right * 10 + gpGlobals->v_forward * 15;

	CGib *pGib = GetClassPtr((CGib *)NULL);
	pGib->Spawn("models/hgibs.mdl");
	pGib->pev->body = RANDOM_LONG(0, 10);
	pGib->m_bloodColor = BLOOD_COLOR_RED;
	pGib->pev->body = 0;
	pGib->pev->origin = GunPosition + gpGlobals->v_forward * 40;
	pGib->pev->velocity = gpGlobals->v_forward * 100;

	// Fograin92: Some spin variations
	pGib->pev->avelocity.x = RANDOM_LONG(200, 600);
	pGib->pev->avelocity.y = RANDOM_LONG(200, 600);
	pGib->pev->avelocity.z = RANDOM_LONG(100, 200);

	pGib->pev->nextthink = UTIL_GlobalTimeBase() + 10.0;
	pGib->SetThink(&CBaseEntity::SUB_FadeOut);

	// Fograin92: Cough some blood
	if (CVAR_GET_FLOAT("r_particles") != 0) {
		MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
		WRITE_SHORT(0);
		WRITE_BYTE(0);
		WRITE_COORD(pGib->pev->origin.x);
		WRITE_COORD(pGib->pev->origin.y);
		WRITE_COORD(pGib->pev->origin.z);
		WRITE_COORD(pGib->pev->avelocity.x);
		WRITE_COORD(pGib->pev->avelocity.y);
		WRITE_COORD(pGib->pev->avelocity.z);
		WRITE_SHORT(iImpactBloodRed);
		MESSAGE_END();
	}
}

