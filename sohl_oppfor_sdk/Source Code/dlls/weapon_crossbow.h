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
// Weapon: Crossbow * http://half-life.wikia.com/wiki/Black_Mesa_Crossbow
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_CROSSBOW_H
#define WEAPON_CROSSBOW_H

#define BOLT_AIR_VELOCITY		2000
#define BOLT_WATER_VELOCITY		1000

//Model Animations | Sequence-ID | Frames | FPS
enum class CROSSBOW_IDLE { sequence = 0, frames = 91, fps = 30 };
enum class CROSSBOW_IDLE_EMPTY { sequence = 1, frames = 31, fps = 30 };
enum class CROSSBOW_FIDGET { sequence = 2, frames = 81, fps = 30 };
enum class CROSSBOW_FIRE { sequence = 3, frames = 91, fps = 60 };
enum class CROSSBOW_FIRE_LAST { sequence = 4, frames = 16, fps = 30 };
enum class CROSSBOW_RELOAD { sequence = 5, frames = 136, fps = 30 };
enum class CROSSBOW_DRAW { sequence = 6, frames = 16, fps = 30 };
enum class CROSSBOW_DRAW_EMPTY { sequence = 7, frames = 16, fps = 30 };
enum class CROSSBOW_HOLSTER { sequence = 8, frames = 16, fps = 25 };
enum class CROSSBOW_HOLSTER_EMPTY { sequence = 9, frames = 16, fps = 25 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Crossbow Base-Class | Base | Attack | Animations | Vars | Events
class CCrossbow : public CBasePlayerWeapon {
public:
	//Base
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);
	void SecondaryAttack(void) { m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 1E6; };
	void FireBolt(void);
	void FireSniperBolt(void);

	//Animations
	BOOL Deploy();
	void Holster();
	void Reload(void);
	void ZoomUpdate(void);
	void ZoomReset(void);
	void WeaponIdle(void);
	BOOL ShouldWeaponIdle(void) { return TRUE; };

	//Vars
	BOOL b_setup;
};
#endif

#endif // WEAPON_CROSSBOW_H