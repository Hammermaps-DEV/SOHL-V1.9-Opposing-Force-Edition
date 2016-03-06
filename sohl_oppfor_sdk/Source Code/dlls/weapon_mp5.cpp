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
// Weapon: The Heckler & Koch MP5
// http://half-life.wikia.com/wiki/MP5
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
#include "proj_grenade.h"
#include "weapon_mp5.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS( weapon_mp5, CMP5 );
LINK_ENTITY_TO_CLASS( weapon_9mmAR, CMP5 );

//=========================================================
// Spawn The Heckler & Koch MP5
//=========================================================
void CMP5::Spawn( ) {
	Precache();

	SET_MODEL(ENT(pev), "models/w_9mmAR.mdl");
	m_iDefaultAmmo = MP5_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CMP5::Precache( void ) {
	PRECACHE_MODEL("models/v_9mmAR.mdl");
	PRECACHE_MODEL("models/w_9mmAR.mdl");
	PRECACHE_MODEL("models/p_9mmAR.mdl");

	PRECACHE_MODEL("models/shell.mdl");// brass shell
	PRECACHE_MODEL("models/grenade.mdl"); // grenade

	PRECACHE_SOUND("weapons/hks1.wav");
	PRECACHE_SOUND("weapons/hks2.wav");
	PRECACHE_SOUND("weapons/glauncher.wav"); //by model
	PRECACHE_SOUND("items/cliprelease1.wav"); //by model
	PRECACHE_SOUND("items/clipinsert1.wav"); //by model

	m_usMP5 = PRECACHE_EVENT( 1, "events/mp5.sc" );
}

//=========================================================
// GetItemInfo - give all Infos for this weapon
//=========================================================
int CMP5::GetItemInfo(ItemInfo *p) {
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo2 = "ARgrenades";
	p->iMaxAmmo2 = M203_GRENADE_MAX_CARRY;
	p->iMaxClip = MP5_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_MP5;
	p->iWeight = MP5_WEIGHT;
	return 1;
}

//=========================================================
// PrimaryAttack
//=========================================================
void CMP5::PrimaryAttack() {
	// don't fire underwater
	if ( m_iClip && m_pPlayer->pev->waterlevel != 3) {
		m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

		m_iClip--;

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		Vector vecSrc	 = m_pPlayer->GetGunPosition( );
		Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
		Vector vecDir;

		if (!IsMultiplayer()) {
			vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, 8192, BULLET_PLAYER_MP5, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);
		} else {
			vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);
		}

		PLAYBACK_EVENT_FULL( 0, m_pPlayer->edict(), m_usMP5, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, pev->body, 0, 0, 0 );

		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.1;
		if ( m_flNextPrimaryAttack < UTIL_GlobalTimeBase() )
			m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.2;

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);

		if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
			// HEV suit - indicate out of ammo condition
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
		}
	} else {
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.5;
	}
}

//=========================================================
// SecondaryAttack
//=========================================================
void CMP5::SecondaryAttack( void ) {
	// don't fire underwater
	if ( m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] > 0 && m_pPlayer->pev->waterlevel != 3) {
		m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

		m_pPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;
		m_pPlayer->m_flStopExtraSoundTime = UTIL_GlobalTimeBase() + 0.2;
			
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		
 		UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

		// we don't add in player velocity anymore.
		CGrenade::ShootContact( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16, gpGlobals->v_forward * 800 );

		SendWeaponAnim((int)MP5_GRENADE::sequence);
		m_pPlayer->pev->punchangle.x -= 10;
	
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 
			CalculateWeaponTime((int)MP5_GRENADE::frames, (int)MP5_GRENADE::fps);

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);

		if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0) {
			// HEV suit - indicate out of ammo condition
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
		}
	} else {
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.5;
	}
}

//=========================================================
// Deploy
//=========================================================
BOOL CMP5::Deploy() {
	return DefaultDeploy("models/v_9mmAR.mdl", "models/p_9mmAR.mdl", (int)MP5_DRAW::sequence,
		"mp5", CalculateWeaponTime((int)MP5_DRAW::frames, (int)MP5_DRAW::fps));
}

//=========================================================
// Holster
//=========================================================
void CMP5::Holster( void ) {
	m_fInReload = FALSE;// cancel any reload in progress.
	SendWeaponAnim((int)MP5_HOLSTER::sequence);
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() +
		CalculateWeaponTime((int)MP5_HOLSTER::frames, (int)MP5_HOLSTER::fps);
}

//=========================================================
// Reload
//=========================================================
void CMP5::Reload( void ) {
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0) {
		return;
	}

	DefaultReload(GLOCK_MAX_CLIP, (int)MP5_RELOAD::sequence,
		CalculateWeaponTime((int)MP5_RELOAD::frames, (int)MP5_RELOAD::fps));

	m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
}

//=========================================================
// WeaponIdle Animation
//=========================================================
void CMP5::WeaponIdle( void ) {
	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase() ||
		m_flTimeWeaponIdleLock > UTIL_GlobalTimeBase()) {
		return;
	}

	// only idle if the slid isn't back
	if (m_iClip) {
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.3) {
			iAnim = (int)MP5_IDLE::sequence;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() +
				CalculateWeaponTime((int)MP5_IDLE::frames, (int)MP5_IDLE::fps);
			m_flTimeWeaponIdleLock = m_flTimeWeaponIdle + RANDOM_FLOAT(2, 10);
		} else {
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + RANDOM_FLOAT(10, 15);
			m_flTimeWeaponIdleLock = UTIL_GlobalTimeBase();
		}

		SendWeaponAnim(iAnim);
	}
}