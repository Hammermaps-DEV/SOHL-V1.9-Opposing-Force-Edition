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
// Weapon: RPG
//=========================================================

enum rpg_e {
	RPG_IDLE = 0,
	RPG_FIDGET,
	RPG_RELOAD,
	RPG_FIRE,
	RPG_HOLSTER,
	RPG_DRAW,
	RPG_HOLSTER_EMPTY,
	RPG_DRAW_EMPTY,
	RPG_IDLE2
};

class CRpg : public CBasePlayerWeapon {
	public:
		void Spawn(void);
		void Precache(void);
		void Reload(void);
		int GetItemInfo(ItemInfo *p);

		BOOL Deploy(void);
		void Holster();
		void UpdateScreen(void);
		void ShutdownScreen(void);
		void PrimaryAttack(void);
		void SecondaryAttack(void);
		void WeaponIdle(void);
		void UpdateSpot(void);
		int AddDuplicate(CBasePlayerItem *pOriginal) { return FALSE; };//don't give second launcher!
		BOOL ShouldWeaponIdle(void) { return TRUE; };

		CLaserSpot *m_pSpot;
		CLaserSpot *m_pMirSpot;
};