/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Spirit of Half-Life and their logos are the property of their respective owners.
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
*   All Rights Reserved.
*
*	Base Source-Code written by Raven City and Marc-Antoine Lortie (https://github.com/malortie).
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/
//=========================================================
// Weapon: M40A1 Sniper Rifle * 
// http://half-life.wikia.com/wiki/M40A1_Sniper_Rifle
//=========================================================

#ifndef WEAPON_M40A1_H
#define WEAPON_M40A1_H

//Model Animations | Sequence-ID | Frames | FPS
enum class M40A1_DRAW { sequence = 0, frames = 31, fps = 30 };
enum class M40A1_SLOWIDLE { sequence = 1, frames = 101, fps = 23 };
enum class M40A1_FIRE { sequence = 2, frames = 68, fps = 38 };
enum class M40A1_FIRELASTROUND { sequence = 3, frames = 68, fps = 38 };
enum class M40A1_RELOAD_START_EMPTY { sequence = 4, frames = 80, fps = 34 };
enum class M40A1_RELOAD_END { sequence = 5, frames = 49, fps = 27 };
enum class M40A1_RELOAD_START { sequence = 6, frames = 80, fps = 34 };
enum class M40A1_SLOWIDLE2 { sequence = 7, frames = 101, fps = 23 };
enum class M40A1_HOLSTER { sequence = 8, frames = 24, fps = 20 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Sniper Rifle Base-Class | Base | Attack | Animations | Vars | Events
class CSniperrifle : public CBasePlayerWeapon {
public:
	//Base
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	//Attack
	void PrimaryAttack(void);
	void SecondaryAttack(void) { m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 1E6; };
	void ZoomUpdate(void);
	void ZoomReset(void);

	//Animations
	BOOL Deploy(void);
	void Holster(void);
	void Reload(void);
	void WeaponIdle(void);
	void ItemPostFrame(void);

	//Vars
	BOOL ShouldWeaponIdle(void) { return TRUE; };
	BOOL m_fInZoom;
	BOOL m_fNeedAjustBolt;
	int	 m_flShellDelay;
	int	 m_iBoltState;
	int  m_iShell;

	enum SNIPER_BOLTSTATE { BOLTSTATE_FINE = 0, BOLTSTATE_ADJUST, BOLTSTATE_ADJUSTING, };
private:
	//Events
	unsigned short m_usSniper;
};
#endif

#endif // WEAPON_M40A1_H