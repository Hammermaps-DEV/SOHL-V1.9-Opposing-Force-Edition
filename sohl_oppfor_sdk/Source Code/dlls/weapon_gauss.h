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
// Weapon: Tau Cannon * http://half-life.wikia.com/wiki/Tau_Cannon
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_GAUSS_H
#define WEAPON_GAUSS_H

#define PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define PRIMARY_FIRE_VOLUME		450// how loud gauss is when discharged

//Model Animations | Sequence-ID | Frames | FPS
enum class GAUSS_IDLE { sequence = 0, frames = 61, fps = 15 };
enum class GAUSS_IDLE2 { sequence = 1, frames = 61, fps = 15 };
enum class GAUSS_FIDGET { sequence = 2, frames = 91, fps = 30 };
enum class GAUSS_SPINUP { sequence = 3, frames = 31, fps = 30 };
enum class GAUSS_SPIN { sequence = 4, frames = 16, fps = 35 };
enum class GAUSS_FIRE { sequence = 5, frames = 31, fps = 30 };
enum class GAUSS_FIRE2 { sequence = 6, frames = 41, fps = 30 };
enum class GAUSS_HOLSTER { sequence = 7, frames = 31, fps = 45 };
enum class GAUSS_DRAW { sequence = 8, frames = 36, fps = 60 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Gauss Base-Class | Base | Attack | Animations | Vars | Events
class CGauss : public CBasePlayerWeapon {
public:
	//Base
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);
	void SecondaryAttack(void);
	void StartFire(void);
	void Fire(Vector vecOrigSrc, Vector vecDirShooting, float flDamage);

	//Animations
	BOOL Deploy(void);
	void Holster();
	void WeaponIdle(void);

	//Vars
	int m_iSoundState;
private:
	//Events
	unsigned short m_usGaussFire;
	unsigned short m_usGaussSpin;
};
#endif

#endif // WEAPON_GAUSS_H