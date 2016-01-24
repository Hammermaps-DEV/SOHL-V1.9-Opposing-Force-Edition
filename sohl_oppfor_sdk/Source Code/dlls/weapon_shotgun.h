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
// Weapon: Shotgun
//=========================================================

enum shotgun_e {
	SHOTGUN_SM_IDLE = 0,
	SHOTGUN_SHOOT,
	SHOTGUN_SHOOT_BIG,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_REHOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_DEEPIDLE
};

class CShotgun : public CBasePlayerWeapon {
	public:
		void Spawn(void);
		void Precache(void);
		int GetItemInfo(ItemInfo *p);

		void PrimaryAttack(void);
		void SecondaryAttack(void);
		BOOL Deploy();
		void Holster();
		void Reload(void);
		void WeaponIdle(void);
	private:
		int m_iShell;
		unsigned short m_usShotgunFire;
};