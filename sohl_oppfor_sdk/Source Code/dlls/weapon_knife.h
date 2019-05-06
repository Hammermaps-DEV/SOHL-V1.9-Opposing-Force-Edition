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
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);
	void SecondaryAttack(void);
	void Charge(bool Swing);
	int Swing(int fFirst);

	//Animations
	BOOL Deploy(void);
	void Holster();
	void WeaponIdle(void);

	//Vars
	BOOL fHitWorld;
	int m_iChargeLevel;
private:
	//Events
	unsigned int m_usKnife;
};
#endif

#endif // WEAPON_KNIFE_H