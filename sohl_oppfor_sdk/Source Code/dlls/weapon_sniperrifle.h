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
// Weapon: M40A1 Sniper Rifle * 
// http://half-life.wikia.com/wiki/M40A1_Sniper_Rifle
//=========================================================

#ifndef WEAPON_M40A1_H
#define WEAPON_M40A1_H

//Model Animations | Sequence-ID | Frames | FPS
enum class M40A1_DRAW { sequence = 0, frames = 31, fps = 30 };
enum class M40A1_SLOWIDLE { sequence = 1, frames = 101, fps = 23 };
enum class M40A1_FIRE { sequence = 2, frames = 68, fps = 38 };
enum class M40A1_FIRELASTROUND { sequence = 3, frames = 68, fps = 38 };
enum class M40A1_RELOAD_START_EMPTY { sequence = 4, frames = 80, fps = 34 };
enum class M40A1_RELOAD_END { sequence = 5, frames = 49, fps = 27 };
enum class M40A1_RELOAD_START { sequence = 6, frames = 80, fps = 34 };
enum class M40A1_SLOWIDLE2 { sequence = 7, frames = 101, fps = 23 };
enum class M40A1_HOLSTER { sequence = 8, frames = 24, fps = 20 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Sniper Rifle Base-Class | Base | Attack | Animations | Vars | Events
class CSniperrifle : public CBasePlayerWeapon {
public:
	//Base
	void Spawn();
	void Precache();
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack();
	void SecondaryAttack() { m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 1E6; };
	void ZoomUpdate();
	void ZoomReset();

	//Animations
	BOOL Deploy();
	void Holster();
	void Reload();
	void WeaponIdle();
	void ItemPostFrame();

	//Vars
	BOOL ShouldWeaponIdle() { return TRUE; };
	BOOL m_fInZoom;
	BOOL m_fNeedAjustBolt;
	int	 m_flShellDelay;
	int	 m_iBoltState;
	int  m_iShell;

	enum SNIPER_BOLTSTATE { BOLTSTATE_FINE = 0, BOLTSTATE_ADJUST, BOLTSTATE_ADJUSTING, };
private:
	//Events
	unsigned short m_usSniper;
};
#endif

#endif // WEAPON_M40A1_H