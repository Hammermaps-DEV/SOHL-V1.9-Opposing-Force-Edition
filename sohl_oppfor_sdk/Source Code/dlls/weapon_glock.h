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
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);
	void SecondaryAttack(void);
	void GlockFire(float flSpread, float flCycleTime, bool fUseAutoAim);

	//Animations
	BOOL Deploy(void);
	void Holster(void);
	void Reload(void);
	void WeaponIdle(void);
private:
	//Events
	unsigned short m_usFireGlock;
};
#endif

#endif // WEAPON_GLOCK_H