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
// Weapon: Barnacle * http://half-life.wikia.com/wiki/Barnacle_Grapple
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_GRAPPLE_H
#define WEAPON_GRAPPLE_H

//Model Animations | Sequence-ID | Frames | FPS
enum class GRAPPLE_BREATHE { sequence = 0, frames = 78, fps = 30 };
enum class GRAPPLE_LONGIDLE { sequence = 1, frames = 301, fps = 30 };
enum class GRAPPLE_SHORTIDLE { sequence = 2, frames = 41, fps = 30 };
enum class GRAPPLE_COUGH { sequence = 3, frames = 139, fps = 30 };
enum class GRAPPLE_HOLSTER { sequence = 4, frames = 41, fps = 30 };
enum class GRAPPLE_DRAW { sequence = 5, frames = 31, fps = 30 };
enum class GRAPPLE_FIRE { sequence = 6, frames = 17, fps = 30 };
enum class GRAPPLE_FIREWAITING { sequence = 7, frames = 17, fps = 30 };
enum class GRAPPLE_FIREREACHED { sequence = 8, frames = 141, fps = 30 };
enum class GRAPPLE_FIRETRAVEL { sequence = 9, frames = 31, fps = 45 };
enum class GRAPPLE_FIRERELEASE { sequence = 10, frames = 31, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Barnacle Base-Class | Base | Attack | Animations | Vars | Events
class CGrapple : public CBasePlayerWeapon {
public:
	//Base
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);
	void SecondaryAttack(void) { PrimaryAttack(); };

	//Animations
	BOOL Deploy();
	void Holster(void);
	void WeaponIdle(void);
	BOOL ShouldWeaponIdle(void) { return TRUE; }; //Call WeaponIdle Loop
	void StopSounds(void);
	void EXPORT FlyThink(void);
	void EXPORT PukeGibs(void);
private:
	//Vars
	bool StartIdle;
	bool PrimaryAttackEnd;
};
#endif

#endif // WEAPON_GRAPPLE_H