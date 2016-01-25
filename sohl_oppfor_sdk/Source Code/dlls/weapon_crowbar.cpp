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
// Weapon: Crowbar * http://half-life.wikia.com/wiki/Crowbar
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00001 / Date: 23.01.2016
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "weapon_crowbar.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(weapon_crowbar, CCrowbar);

//=========================================================
// Spawn Crowbar
//=========================================================
void CCrowbar::Spawn(void) {
	Precache( );
	m_iId = WEAPON_CROWBAR;

	SET_MODEL(ENT(pev), "models/w_crowbar.mdl");
	m_iClip = -1;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CCrowbar::Precache(void) {
	PRECACHE_MODEL("models/v_crowbar.mdl");
	PRECACHE_MODEL("models/w_crowbar.mdl");
	PRECACHE_MODEL("models/p_crowbar.mdl");

	PRECACHE_SOUND("weapons/cbar_hit1.wav");
	PRECACHE_SOUND("weapons/cbar_hit2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod1.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod3.wav");
	PRECACHE_SOUND("weapons/cbar_miss1.wav");

	m_usCrowbar = PRECACHE_EVENT ( 1, "events/crowbar.sc" );
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CCrowbar::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 0;
	p->iId = WEAPON_CROWBAR;
	p->iWeight = CROWBAR_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack - Call the frist Attack
//=========================================================
void CCrowbar::PrimaryAttack(void) {
	if (!CCrowbar::Swing(TRUE)) {
		CCrowbar::Swing(FALSE);
	}

	m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.2;
}

//=========================================================
// SecondaryAttack - Call the second Attack
//=========================================================
void CCrowbar::SecondaryAttack(void) {
	CCrowbar::PrimaryAttack(); //Call PrimaryAttack
}

//=========================================================
// Swing - This used by Primary & SecondaryAttack
//=========================================================
int CCrowbar::Swing( int fFirst ) {
	int fDidHit = FALSE;
	bHit = FALSE;
	TraceResult tr;

	if (m_flTimeUpdate > UTIL_WeaponTimeBase()) {
		return fDidHit;
	}
          
	UTIL_MakeVectors (m_pPlayer->pev->v_angle);
	Vector vecSrc	= m_pPlayer->GetGunPosition( );
	Vector vecEnd	= vecSrc + gpGlobals->v_forward * 32;
	int AttackAnimation = RANDOM_LONG(0, 2);

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	if ( tr.flFraction >= 1.0 ) {
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 ) {
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( !pHit || pHit->IsBSPModel() )
				FindHullIntersection( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict() );

			vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}

	if ( tr.flFraction >= 1.0 && fFirst) 
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	else {
		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		
		// hit
		fDidHit = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		ClearMultiDamage( );
		pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgCrowbar, gpGlobals->v_forward, &tr, DMG_CLUB | DMG_NEVERGIB );
		ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		if (pEntity) {
			if ( pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE ) {
				bHit = TRUE;//play hitbody sound on client
				m_pPlayer->m_iWeaponVolume = CROWBAR_BODYHIT_VOLUME;
				if (!pEntity->IsAlive()) {
					m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
				} else
					flVol = 0.1;

				fHitWorld = FALSE;
			}
		}

		m_pPlayer->m_iWeaponVolume = flVol * CROWBAR_WALLHIT_VOLUME;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + RANDOM_FLOAT(10, 15);
		switch (AttackAnimation) {
			case 0:
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 
					(CalculateWeaponTime((int)CROWBAR_ATTACK1::frames, (int)CROWBAR_ATTACK1::fps) / 100 * CROWBAR_ATTACK_BOOST);
				break;
			case 1:
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() +
					(CalculateWeaponTime((int)CROWBAR_ATTACK2::frames, (int)CROWBAR_ATTACK2::fps) / 100 * CROWBAR_ATTACK_BOOST);
				break;
			case 2:
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() +
					(CalculateWeaponTime((int)CROWBAR_ATTACK3::frames, (int)CROWBAR_ATTACK3::fps) / 100 * CROWBAR_ATTACK_BOOST);
				break;
		}

		m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.2;	
	}

	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usCrowbar, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, pev->body, fFirst, bHit, AttackAnimation);
	return fDidHit;
}

//=========================================================
// Deploy
//=========================================================
BOOL CCrowbar::Deploy(void) {
	return DefaultDeploy("models/v_crowbar.mdl", "models/p_crowbar.mdl", (int)CROWBAR_DRAW::sequence,
		"crowbar", CalculateWeaponTime((int)CROWBAR_DRAW::frames, (int)CROWBAR_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CCrowbar::Holster(void) {
	SendWeaponAnim((int)CROWBAR_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 
		CalculateWeaponTime((int)CROWBAR_HOLSTER::frames, (int)CROWBAR_HOLSTER::fps);
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CCrowbar:: WeaponIdle(void) {
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_WeaponTimeBase()) {
		return;
	}

	int iAnim;
	float flRand = RANDOM_FLOAT(0, 1);
	if (flRand <= 0.5) {
		iAnim = (int)CROWBAR_IDLE1::sequence;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 
			CalculateWeaponTime((int)CROWBAR_IDLE1::frames, (int)CROWBAR_IDLE1::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	} else if (flRand <= 0.7) {
		iAnim = (int)CROWBAR_IDLE2::sequence;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 
			CalculateWeaponTime((int)CROWBAR_IDLE2::frames, (int)CROWBAR_IDLE2::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	} else if (flRand <= 0.9) {
		iAnim = (int)CROWBAR_IDLE3::sequence;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 
			CalculateWeaponTime((int)CROWBAR_IDLE3::frames, (int)CROWBAR_IDLE3::fps);
		m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
	} else {
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + RANDOM_FLOAT(10, 15);
		m_flTimeWeaponIdleLock = UTIL_WeaponTimeBase();
	}

	SendWeaponAnim(iAnim);
}



