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
// Weapon: Knife * http://half-life.wikia.com/wiki/Combat_Knife
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_KNIFE_H
#define WEAPON_KNIFE_H

//Crowbar Hit-Volume
#define	KNIFE_BODYHIT_VOLUME 128
#define	KNIFE_WALLHIT_VOLUME 512

#define	KNIFE_ATTACK_BOOST   60

//Model Animations | Sequence-ID | Frames | FPS
enum class KNIFE_IDLE1 { sequence = 0, frames = 36, fps = 13 };
enum class KNIFE_IDLE2 { sequence = 1, frames = 81, fps = 15 };
enum class KNIFE_IDLE3 { sequence = 2, frames = 81, fps = 15 };
enum class KNIFE_DRAW { sequence = 3, frames = 13, fps = 24 };
enum class KNIFE_HOLSTER { sequence = 4, frames = 13, fps = 24 };
enum class KNIFE_ATTACK1 { sequence = 5, frames = 11, fps = 22 };
enum class KNIFE_ATTACK1MISS { sequence = 6, frames = 11, fps = 22 };
enum class KNIFE_ATTACK2 { sequence = 7, frames = 14, fps = 22 };
enum class KNIFE_ATTACK2MISS { sequence = 8, frames = 14, fps = 22 };
enum class KNIFE_ATTACK3 { sequence = 9, frames = 19, fps = 24 };
enum class KNIFE_ATTACK3MISS { sequence = 10, frames = 19, fps = 24 };
enum class KNIFE_CHARGE { sequence = 11, frames = 21, fps = 30 };
enum class KNIFE_STAB { sequence = 12, frames = 21, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Knife Base-Class | Base | Attack | Animations | Vars | Events
class CKnife : public CBasePlayerWeapon {
public:
	//Base
	void Spawn();
	void Precache();
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack();
	void SecondaryAttack();
	void Charge(bool Swing);
	int Swing(int fFirst);

	//Animations
	BOOL Deploy();
	void Holster();
	void WeaponIdle();

	//Vars
	BOOL fHitWorld;
	int m_iChargeLevel;
private:
	//Events
	unsigned int m_usKnife;
};
#endif

#endif // WEAPON_KNIFE_H