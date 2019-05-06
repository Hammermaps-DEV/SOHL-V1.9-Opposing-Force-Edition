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
// Weapon: Pipe Wrench * http://half-life.wikia.com/wiki/Pipe_Wrench
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_PIPE_WRENCH_H
#define WEAPON_PIPE_WRENCH_H

//Pipe Wrench Hit-Volume
#define	PIPE_WRENCH_BODYHIT_VOLUME 128
#define	PIPE_WRENCH_WALLHIT_VOLUME 2048

#define	PIPE_WRENCH_ATTACK_BOOST   90

//Model Animations | Sequence-ID | Frames | FPS
enum class PIPE_WRENCH_IDLE1 { sequence = 0, frames = 61, fps = 30 };
enum class PIPE_WRENCH_IDLE2 { sequence = 1, frames = 91, fps = 30 };
enum class PIPE_WRENCH_IDLE3 { sequence = 2, frames = 91, fps = 30 };
enum class PIPE_WRENCH_DRAW { sequence = 3, frames = 21, fps = 30 };
enum class PIPE_WRENCH_HOLSTER { sequence = 4, frames = 21, fps = 30 };
enum class PIPE_WRENCH_ATTACK1 { sequence = 5, frames = 23, fps = 30 };
enum class PIPE_WRENCH_ATTACK1MISS { sequence = 6, frames = 28, fps = 30 };
enum class PIPE_WRENCH_ATTACK2 { sequence = 7, frames = 23, fps = 30 };
enum class PIPE_WRENCH_ATTACK2MISS { sequence = 8, frames = 23, fps = 30 };
enum class PIPE_WRENCH_ATTACK3 { sequence = 9, frames = 18, fps = 30 };
enum class PIPE_WRENCH_ATTACK3MISS { sequence = 10, frames = 23, fps = 30 };
enum class PIPE_WRENCH_ATTACKBIGWIND { sequence = 11, frames = 27, fps = 30 };
enum class PIPE_WRENCH_ATTACKBIGHIT { sequence = 12, frames = 29, fps = 30 };
enum class PIPE_WRENCH_ATTACKBIGMISS { sequence = 13, frames = 30, fps = 30 };
enum class PIPE_WRENCH_ATTACKBIGLOOP { sequence = 14, frames = 3, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Crowbar Base-Class | Base | Attack | Animations | Vars | Events
class CPipeWrench : public CBasePlayerWeapon {
public:
	//Base
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);
	void SecondaryAttack(void);
	void StartLargeSwing(void);
	int Swing(int fFirst);
	int LargeSwing(float flDamage);

	//Animations
	BOOL Deploy(void);
	void Holster(void);
	void WeaponIdle(void);

	//Vars
	int m_flAmmoStartCharge;
	int m_fullcharge;
	int m_iSwing;
	int m_iHit;
	TraceResult m_trHit;
private:
	BOOL m_fPrimaryFire;
	BOOL bHit;

	//Events
	unsigned short m_usWrenchSmall;
	unsigned short m_usWrenchLarge;
};
#endif

#endif // WEAPON_PIPE_WRENCH_H