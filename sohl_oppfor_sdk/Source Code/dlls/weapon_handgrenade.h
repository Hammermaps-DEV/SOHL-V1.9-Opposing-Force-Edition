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
// Weapon: Mk 2 Grenade * http://half-life.wikia.com/wiki/Mk_2_Grenade
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_HANDGRENADE_H
#define WEAPON_HANDGRENADE_H

#define	HANDGRENADE_PRIMARY_VOLUME	450

//Model Animations | Sequence-ID | Frames | FPS
enum class HANDGRENADE_IDLE { sequence = 0, frames = 91, fps = 30 };
enum class HANDGRENADE_FIDGET { sequence = 1, frames = 76, fps = 30 };
enum class HANDGRENADE_PINPULL { sequence = 2, frames = 25, fps = 45 };
enum class HANDGRENADE_THROW1 { sequence = 3, frames = 19, fps = 13 };
enum class HANDGRENADE_THROW2 { sequence = 4, frames = 19, fps = 20 };
enum class HANDGRENADE_THROW3 { sequence = 5, frames = 19, fps = 30 };
enum class HANDGRENADE_HOLSTER { sequence = 6, frames = 16, fps = 30 };
enum class HANDGRENADE_DRAW { sequence = 7, frames = 16, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Mk 2 Grenade Base-Class | Base | Attack | Animations | Vars | Events
class CHandGrenade : public CBasePlayerWeapon {
public:
	//Base
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);

	//Animations
	BOOL Deploy(void);
	BOOL CanHolster(void) { return (m_flStartThrow == 0); }
	void Holster();
	void WeaponIdle(void);
};
#endif

#endif // WEAPON_HANDGRENADE_H