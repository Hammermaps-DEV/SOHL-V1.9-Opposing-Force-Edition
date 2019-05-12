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
// Weapon: Spore Launcher * http://half-life.wikia.com/wiki/Spore_Launcher
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_SPORELAUNCHER_H
#define WEAPON_SPORELAUNCHER_H

//Model Animations | Sequence-ID | Frames | FPS
enum class SPLAUNCHER_IDLE1 { sequence = 0, frames = 61, fps = 30 };
enum class SPLAUNCHER_FIDGET { sequence = 1, frames = 121, fps = 30 };
enum class SPLAUNCHER_RELOAD_REACH { sequence = 2, frames = 21, fps = 30 };
enum class SPLAUNCHER_RELOAD_LOAD { sequence = 3, frames = 31, fps = 30 };
enum class SPLAUNCHER_RELOAD_AIM { sequence = 4, frames = 26, fps = 30 };
enum class SPLAUNCHER_SHOOT { sequence = 5, frames = 16, fps = 30 };
enum class SPLAUNCHER_HOLSTER { sequence = 6, frames = 31, fps = 30 };
enum class SPLAUNCHER_DRAW { sequence = 7, frames = 31, fps = 30 };
enum class SPLAUNCHER_IDLE2 { sequence = 8, frames = 121, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Spore Launcher Base-Class | Base | Attack | Animations | Vars | Events
class CSporelauncher : public CBasePlayerWeapon {
public:
	//Base
	void Spawn();
	void Precache();
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack();
	void SecondaryAttack();

	//Animations
	BOOL Deploy();
	void Holster();
	void Reload();
	void Charge(bool m_BeginAttack);
	void WeaponIdle();
private:
	unsigned short m_usSporeFire;
};
#endif

#endif // WEAPON_SPORELAUNCHER_H