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
// Weapon: The Heckler & Koch MP5
// http://half-life.wikia.com/wiki/MP5
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_MP5_H
#define WEAPON_MP5_H

//Model Animations | Sequence-ID | Frames | FPS
enum class MP5_IDLE    { sequence = 0, frames = 41, fps = 8 };
enum class MP5_DRAW    { sequence = 1, frames = 12, fps = 30 };
enum class MP5_HOLSTER { sequence = 2, frames = 12, fps = 25 };
enum class MP5_SHOOT_1 { sequence = 3, frames = 7,  fps = 10 };
enum class MP5_SHOOT_2 { sequence = 4, frames = 7,  fps = 10 };
enum class MP5_GRENADE { sequence = 5, frames = 34, fps = 30 };
enum class MP5_RELOAD  { sequence = 6, frames = 47, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//MP5 Base-Class | Base | Attack | Animations | Vars | Events
class CMP5 : public CBasePlayerWeapon {
	public:
		//Base
		void Spawn(void);
		void Precache(void);
		int GetItemInfo(ItemInfo *p);

		//Attack
		void PrimaryAttack(void);
		void SecondaryAttack(void);

		//Animations
		int SecondaryAmmoIndex(void) { return m_iSecondaryAmmoType; };
		BOOL Deploy(void);
		void CMP5::Holster(void);
		void Reload(void);
		void WeaponIdle(void);
	private:
		//Events
		unsigned short m_usMP5;
};
#endif

#endif // WEAPON_MP5_H