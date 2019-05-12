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
// Weapon: Glock 17 * http://half-life.wikia.com/wiki/Glock_17
//=========================================================

#ifndef WEAPON_GLOCK_H
#define WEAPON_GLOCK_H

//Model Animations | Sequence-ID | Frames | FPS
enum class GLOCK_IDLE1 { sequence = 0, frames = 61, fps = 16 };
enum class GLOCK_IDLE2 { sequence = 1, frames = 41, fps = 16 };
enum class GLOCK_IDLE3 { sequence = 2, frames = 50, fps = 14 };
enum class GLOCK_DRAW { sequence = 3, frames = 16, fps = 30 };
enum class GLOCK_HOLSTER { sequence = 4, frames = 16, fps = 30 };
enum class GLOCK_SHOOT { sequence = 5, frames = 15, fps = 25 };
enum class GLOCK_SHOOT_EMPTY { sequence = 6, frames = 17, fps = 25 };
enum class GLOCK_RELOAD { sequence = 7, frames = 42, fps = 18 };
enum class GLOCK_RELOAD_NOT_EMPTY { sequence = 8, frames = 42, fps = 18 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Glock 17 Base-Class | Base | Attack | Animations | Vars | Events
class CGlock : public CBasePlayerWeapon {
public:
	//Base
	void Spawn();
	void Precache();
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack();
	void SecondaryAttack();
	void GlockFire(float flSpread, float flCycleTime, bool fUseAutoAim);

	//Animations
	BOOL Deploy();
	void Holster();
	void Reload();
	void WeaponIdle();
private:
	//Events
	unsigned short m_usFireGlock;
};
#endif

#endif // WEAPON_GLOCK_H