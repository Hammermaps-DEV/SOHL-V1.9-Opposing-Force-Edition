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
// Weapon: HECU Rocket Propelled Grenade
// http://half-life.wikia.com/wiki/HECU_RPG
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_RPG_H
#define WEAPON_RPG_H

//Model Animations | Sequence-ID | Frames | FPS
enum class RPG_IDLE1 { sequence = 0, frames = 91, fps = 30 };
enum class RPG_IDLE2 { sequence = 1, frames = 91, fps = 30 };
enum class RPG_FIDGET { sequence = 2, frames = 91, fps = 15 };
enum class RPG_DRAW { sequence = 3, frames = 16, fps = 30 };
enum class RPG_HOLSTER { sequence = 4, frames = 16, fps = 30 };
enum class RPG_FIRE { sequence = 5, frames = 25, fps = 30 };
enum class RPG_RELOAD { sequence = 6, frames = 61, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//RPG Base-Class | Base | Attack | Animations | Vars | Events
class CRpg : public CBasePlayerWeapon {
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
	void Holster();
	void Reload(void);
	void UpdateScreen(void);
	void ShutdownScreen(void);
	void UpdateSpot(void);
	void WeaponIdle(void);

	int AddDuplicate(CBasePlayerItem *pOriginal) { return FALSE; };//don't give second launcher!
	BOOL ShouldWeaponIdle(void) { return TRUE; };

	CLaserSpot *m_pSpot;
	CLaserSpot *m_pMirSpot;
	bool m_pActiveRocket;
};
#endif

#endif // WEAPON_RPG_H