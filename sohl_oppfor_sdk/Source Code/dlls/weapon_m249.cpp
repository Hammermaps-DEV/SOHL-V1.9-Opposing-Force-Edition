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
//SOHL - Opposing-Force

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

enum m249_e
{
	SAW_SLOWIDLE = 0,
	SAW_IDLE,
	SAW_RELOAD_START,
	SAW_RELOAD_END,
	SAW_HOLSTER,
	SAW_DRAW,
	SAW_SHOOT1,
	SAW_SHOOT2,
	SAW_SHOOT3
};

class CM249 : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack(void);
	BOOL Deploy(void);
	void Holster();
	void Reload(void);
	void WeaponIdle(void);
	void UpdateClip(void);

	int m_iReloadStep = 0;
private:
	unsigned short m_usM249;
};

LINK_ENTITY_TO_CLASS(weapon_m249, CM249);

void CM249::Spawn()
{
	pev->classname = MAKE_STRING("weapon_m249");
	Precache();
	SET_MODEL(ENT(pev), "models/w_saw.mdl");
	m_iDefaultAmmo = M249_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

void CM249::Precache(void)
{
	PRECACHE_MODEL("models/v_saw.mdl");
	PRECACHE_MODEL("models/w_saw.mdl");
	PRECACHE_MODEL("models/p_saw.mdl");

	PRECACHE_SOUND("weapons/saw_fire1.wav");
	PRECACHE_SOUND("weapons/saw_fire2.wav");
	PRECACHE_SOUND("weapons/saw_fire3.wav");

	PRECACHE_SOUND("weapons/saw_reload.wav");
	PRECACHE_SOUND("weapons/saw_reload2.wav");

	int m_iShell = PRECACHE_MODEL("models/saw_shell.mdl");// brass shell
	int m_iShell_link = PRECACHE_MODEL("models/saw_link.mdl");// brass shell link

	m_usM249 = PRECACHE_EVENT(1, "events/m249.sc");
}

int CM249::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556";
	p->iMaxAmmo1 = _556_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = M249_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_M249;
	p->iWeight = M249_WEIGHT;

	return 1;
}

BOOL CM249::Deploy()
{
	return DefaultDeploy("models/v_saw.mdl", "models/p_saw.mdl", SAW_DRAW, "saw", 0.3);
}

void CM249::Holster()
{
	m_fInReload = FALSE;// cancel any reload in progress.
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() + 0.8;
	SendWeaponAnim(SAW_HOLSTER);
}

void CM249::PrimaryAttack()
{
	if (m_iReloadStep)
		return;

	// don't fire underwater
	if (m_iClip && m_pPlayer->pev->waterlevel != 3)
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

		m_iClip--;
		UpdateClip();

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
		Vector vecDir;

		if (!IsMultiplayer())
			vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_5DEGREES, 8192, BULLET_PLAYER_556, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);
		else	
			vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_4DEGREES, 8192, BULLET_PLAYER_556, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);

		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usM249, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, pev->body, 0, 0, 0);

		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.0675;

		if (m_flNextPrimaryAttack < UTIL_GlobalTimeBase())
			m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.0675;

		if (!FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		{
			float flOldPlayerVel = m_pPlayer->pev->velocity.z;
			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity + (50 * -gpGlobals->v_forward);
			m_pPlayer->pev->velocity.z = flOldPlayerVel;
		}

		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 7.0 / 30.0;
	}
	else
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.5;
	}
}

void CM249::Reload(void)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == M249_MAX_CLIP)
		return;

	if (m_iClip != 50 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0)
	{
		UpdateClip();
		m_iReloadStep = 1;
		DefaultReload(M249_MAX_CLIP, SAW_RELOAD_START, 1.5);
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + (61 / 40) + (111 / 45);
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 61 / 40;
	}
}

void CM249::WeaponIdle(void)
{
	if (m_flTimeWeaponIdle > UTIL_GlobalTimeBase()) return;

	if (m_iReloadStep)
	{
		UpdateClip();
		m_iReloadStep = 0;
		SendWeaponAnim(SAW_RELOAD_END);
		m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + 111.0 / 45.0;
		m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 111.0 / 45.0;
	}
	else
	{
		UpdateClip();
		int iAnim;
		float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);
		if (flRand <= 0.8)
		{
			iAnim = SAW_SLOWIDLE;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + (45.0 / 9.0);
		}
		else
		{
			iAnim = SAW_IDLE;
			m_flTimeWeaponIdle = UTIL_GlobalTimeBase() + (74.0 / 12.0);// * RANDOM_LONG(2, 5);
		}

		SendWeaponAnim(iAnim);
	}
}

void CM249::UpdateClip(void)
{
	if (m_iClip <= 8)
		pev->body = 1;
	if (m_iClip <= 7)
		pev->body = 2;
	if (m_iClip <= 6)
		pev->body = 3;
	if (m_iClip <= 5)
		pev->body = 4;
	if (m_iClip <= 4)
		pev->body = 5;
	if (m_iClip <= 3)
		pev->body = 6;
	if (m_iClip <= 2)
		pev->body = 7;
	if (m_iClip <= 1)
		pev->body = 8;
	if (m_iClip == 0)
		pev->body = 8;

	if (m_iClip != 0
		&& m_iClip != 1
		&& m_iClip != 2
		&& m_iClip != 3
		&& m_iClip != 4
		&& m_iClip != 5
		&& m_iClip != 6
		&& m_iClip != 7
		&& m_iClip != 8
		&& m_iClip != 9)
		pev->body = 0;
}

class CM249AmmoClip : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_saw_clip.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_saw_clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo(CBaseEntity *pOther)
	{
		int bResult = (pOther->GiveAmmo(AMMO_556_GIVE, "556", _556_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS(ammo_556, CM249AmmoClip)