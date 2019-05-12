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
// Weapon: Desert Eagle * http://half-life.wikia.com/wiki/Desert_Eagle
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_EAGLE_H
#define WEAPON_EAGLE_H

//Model Animations | Sequence-ID | Frames | FPS
enum class EAGLE_IDLE1 { sequence = 0, frames = 76, fps = 30 };
enum class EAGLE_IDLE2 { sequence = 1, frames = 61, fps = 24 };
enum class EAGLE_IDLE3 { sequence = 2, frames = 50, fps = 30 };
enum class EAGLE_IDLE4 { sequence = 3, frames = 76, fps = 30 };
enum class EAGLE_IDLE5 { sequence = 4, frames = 61, fps = 30 };
enum class EAGLE_SHOOT { sequence = 5, frames = 20, fps = 30 };
enum class EAGLE_SHOOT_EMPTY { sequence = 6, frames = 20, fps = 27 };
enum class EAGLE_RELOAD { sequence = 7, frames = 42, fps = 25 };
enum class EAGLE_RELOAD_NOT_EMPTY { sequence = 8, frames = 42, fps = 25 };
enum class EAGLE_DRAW { sequence = 9, frames = 16, fps = 18 };
enum class EAGLE_HOLSTER { sequence = 10, frames = 16, fps = 22 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Desert Eagle Base-Class | Base | Attack | Animations | Vars | Events
class CEagle : public CBasePlayerWeapon {
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
	void ShutdownSpot();
	void UpdateSpot();

	CLaserSpot *m_pMirSpot;
	CLaserSpot *m_pSpot;
	int m_fSpotActive;
private:
	//Events
	unsigned short m_usFireEagle;
};
#endif

#endif // WEAPON_EAGLE_H