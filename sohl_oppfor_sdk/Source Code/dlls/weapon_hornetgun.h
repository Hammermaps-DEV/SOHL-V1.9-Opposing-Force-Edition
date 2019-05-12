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
// Weapon: Hivehand * http://half-life.wikia.com/wiki/Hivehand
//=========================================================

#ifndef WEAPON_HGUN_H
#define WEAPON_HGUN_H

//Model Animations | Sequence-ID | Frames | FPS
enum class HGUN_IDLE1 { sequence = 0, frames = 31, fps = 16 };
enum class HGUN_FIDGETSWAY { sequence = 1, frames = 41, fps = 16 };
enum class HGUN_FIDGETSHAKE { sequence = 2, frames = 36, fps = 16 };
enum class HGUN_DOWN { sequence = 3, frames = 19, fps = 16 };
enum class HGUN_UP { sequence = 4, frames = 31, fps = 30 };
enum class HGUN_SHOOT { sequence = 5, frames = 11, fps = 24 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Hivehand Base-Class | Base | Attack | Animations | Vars | Events
class CHgun : public CBasePlayerWeapon
{
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
	BOOL ShouldWeaponIdle() { return TRUE; }; //Call WeaponIdle Loop
	void Holster();
	void Reload();
	void WeaponIdle();

	//VAR
	float m_flNextAnimTime;
	float m_flRechargeTime;
	int m_iFirePhase;// don't save me.
};
#endif

#endif // WEAPON_HGUN_H