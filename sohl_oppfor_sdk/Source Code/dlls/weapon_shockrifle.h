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
// Weapon: Shockrifle * http://half-life.wikia.com/wiki/Shock_Roach
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_SHOCKRIFLE_H
#define WEAPON_SHOCKRIFLE_H

//Model Animations | Sequence-ID | Frames | FPS
enum class SHOCKRIFLE_IDLE1   { sequence = 0, frames = 101, fps = 30 };
enum class SHOCKRIFLE_IDLE2   { sequence = 4, frames = 51, fps = 15 };
enum class SHOCKRIFLE_SHOOT   { sequence = 1, frames = 11, fps = 30 };
enum class SHOCKRIFLE_DRAW    { sequence = 2, frames = 31, fps = 30 };
enum class SHOCKRIFLE_HOLSTER { sequence = 3, frames = 26, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Shockrifle Base-Class | Base | Attack | Animations | Vars | Events
class CShockrifle : public CHgun {
	public:
		//Base
		void Spawn(void);
		void Precache(void);
		int GetItemInfo(ItemInfo *p);
		int AddToPlayer(CBasePlayer *pPlayer);

		//Attack
		void PrimaryAttack(void);
		void SecondaryAttack(void) { PrimaryAttack(); };

		//Animations
		BOOL Deploy(void);
		void Holster(void);
		void Reload(void);
		void WeaponIdle(void);
		void ItemPostFrame(void);;
		void UpdateEffects();
	private:
		//Sounds
		static const char *pIdleSounds[];

		//Vars
		int m_fShouldUpdateEffects;
		int m_flBeamLifeTime;

		//Events
		unsigned short m_usShockFire;
};
#endif

#endif // WEAPON_SHOCKRIFLE_H