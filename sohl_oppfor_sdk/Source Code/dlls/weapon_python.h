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
// Weapon: Colt Python 357 Magnum Revolver * 
// http://half-life.wikia.com/wiki/Colt_Python
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_PYTHON_H
#define WEAPON_PYTHON_H

//Model Animations | Sequence-ID | Frames | FPS
enum class PYTHON_IDLE1 { sequence = 0, frames = 71, fps = 30 };
enum class PYTHON_IDLE2 { sequence = 1, frames = 71, fps = 30 };
enum class PYTHON_IDLE3 { sequence = 2, frames = 89, fps = 30 };
enum class PYTHON_DRAW { sequence = 3, frames = 16, fps = 30 };
enum class PYTHON_HOLSTER { sequence = 4, frames = 16, fps = 30 };
enum class PYTHON_SHOOT { sequence = 5, frames = 31, fps = 30 };
enum class PYTHON_FIDGET { sequence = 6, frames = 171, fps = 30 };
enum class PYTHON_RELOAD { sequence = 7, frames = 111, fps = 37 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Colt Python Base-Class | Base | Attack | Animations | Vars | Events
class CPython : public CBasePlayerWeapon {
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
	void WeaponIdle();

	//Multiplayer
	void UpdateSpot();

	//Vars
	int m_fSpotActive;
	int m_iShell;
	float m_flSoundDelay;
	CLaserSpot *m_pSpot;
private:
	//Events
	unsigned short m_usFirePython;
};
#endif

#endif // WEAPON_PYTHON_H