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
// Weapon: Desert Eagle * http://half-life.wikia.com/wiki/Desert_Eagle
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef WEAPON_EAGLE_H
#define WEAPON_EAGLE_H

//Model Animations | Sequence-ID | Frames | FPS
enum class EAGLE_IDLE1				{ sequence = 0,  frames = 76,  fps = 30 };
enum class EAGLE_IDLE2				{ sequence = 1,  frames = 61,  fps = 24 };
enum class EAGLE_IDLE3				{ sequence = 2,  frames = 50,  fps = 30 };
enum class EAGLE_IDLE4				{ sequence = 3,  frames = 76,  fps = 30 };
enum class EAGLE_IDLE5				{ sequence = 4,  frames = 61,  fps = 30 };
enum class EAGLE_SHOOT				{ sequence = 5,  frames = 20,  fps = 30 };
enum class EAGLE_SHOOT_EMPTY		{ sequence = 6,  frames = 20,  fps = 27 };
enum class EAGLE_RELOAD				{ sequence = 7,  frames = 42,  fps = 25 };
enum class EAGLE_RELOAD_NOT_EMPTY	{ sequence = 8,  frames = 42,  fps = 25 };
enum class EAGLE_DRAW				{ sequence = 9,  frames = 16,  fps = 18 };
enum class EAGLE_HOLSTER			{ sequence = 10, frames = 16,  fps = 22 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Desert Eagle Base-Class | Base | Attack | Animations | Vars | Events
class CEagle : public CBasePlayerWeapon {
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
	void Holster(void);
	void Reload(void);
	void WeaponIdle(void);
	void ShutdownSpot(void);
	void UpdateSpot(void);

	CLaserSpot *m_pMirSpot;
	CLaserSpot *m_pSpot;
	int m_fSpotActive;
private:
	//Events
	unsigned short m_usFireEagle;
};
#endif

#endif // WEAPON_EAGLE_H