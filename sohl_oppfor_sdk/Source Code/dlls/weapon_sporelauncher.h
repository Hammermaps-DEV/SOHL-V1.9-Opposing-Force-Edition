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
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);
	void SecondaryAttack(void);

	//Animations
	BOOL Deploy(void);
	void Holster(void);
	void Reload(void);
	void Charge(bool m_BeginAttack);
	void WeaponIdle(void);
private:
	unsigned short m_usSporeFire;
};
#endif

#endif // WEAPON_SPORELAUNCHER_H