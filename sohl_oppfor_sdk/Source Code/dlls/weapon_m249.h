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
// Weapon: M249 Squad Automatic Weapon
// http://half-life.wikia.com/wiki/M249_Squad_Automatic_Weapon
//=========================================================

#ifndef WEAPON_M249_H
#define WEAPON_M249_H

//Model Animations | Sequence-ID | Frames | FPS
enum class SAW_SLOWIDLE { sequence = 0, frames = 101, fps = 20 };
enum class SAW_IDLE { sequence = 1, frames = 155, fps = 25 };
enum class SAW_RELOAD_START { sequence = 2, frames = 61, fps = 40 };
enum class SAW_RELOAD_END { sequence = 3, frames = 111, fps = 45 };
enum class SAW_HOLSTER { sequence = 4, frames = 29, fps = 30 };
enum class SAW_DRAW { sequence = 5, frames = 31, fps = 30 };
enum class SAW_SHOOT1 { sequence = 6, frames = 7, fps = 30 };
enum class SAW_SHOOT2 { sequence = 7, frames = 7, fps = 30 };
enum class SAW_SHOOT3 { sequence = 8, frames = 7, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//M249 Base-Class | Base | Attack | Animations | Vars | Events
class CM249 : public CBasePlayerWeapon {
public:
	//Base
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);

	//Animations
	BOOL Deploy(void);
	void Holster(void);
	void Reload(void);
	void WeaponIdle(void);
	void UpdateClip(void);

private:
	BOOL m_iReloadStep = 0;
	unsigned short m_usM249;
};
#endif

#endif // WEAPON_M249_H