/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Half-Life and their logos are the property of their respective owners.
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
*	Spirit of Half-Life, by Laurie R. Cheers. (LRC)
*   Modified by Lucas Brucksch (Code merge & Effects)
*   Modified by Andrew J Hamilton (AJH)
*   Modified by XashXT Group (g-cont...)
*
*   Code used from Battle Grounds Team and Contributors.
*   Code used from SamVanheer (Opposing Force code)
*   Code used from FWGS Team (Fixes for SOHL)
*   Code used from LevShisterov (Bugfixed and improved HLSDK)
*	Code used from Fograin (Half-Life: Update MOD)
*
***/
//=========================================================
// Weapon: Displacer * http://half-life.wikia.com/wiki/Displacer_Cannon
//=========================================================

#ifndef WEAPON_DISPLACER_H
#define WEAPON_DISPLACER_H

//Model Animations | Sequence-ID | Frames | FPS
enum class DISPLACER_IDLE1 { sequence = 0, frames = 91, fps = 30 };
enum class DISPLACER_IDLE2 { sequence = 1, frames = 91, fps = 30 };
enum class DISPLACER_SPINUP { sequence = 2, frames = 31, fps = 30 };
enum class DISPLACER_SPIN { sequence = 3, frames = 31, fps = 30 };
enum class DISPLACER_FIRE { sequence = 4, frames = 31, fps = 30 };
enum class DISPLACER_DRAW { sequence = 5, frames = 16, fps = 15 };
enum class DISPLACER_HOLSTER { sequence = 6, frames = 16, fps = 15 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Displacer Base-Class | Base | Attack | Animations | Vars | Events
class CDisplacer : public CBasePlayerWeapon {
public:
	void Spawn();
	void Precache();
	int GetItemInfo(ItemInfo *p);
	void PrimaryAttack();
	void SecondaryAttack();
	BOOL Deploy();
	void Holster();
	void WeaponIdle();
	void AttachBeamCreate(const Vector &vecEndPos);

	int		m_iFireState;
	int		m_iFireMode;
	CBaseEntity* m_hTargetEarth;
	CBaseEntity* m_hTargetXen;

	BOOL HasAmmo();
	void UseAmmo(int count);
	BOOL CanFireDisplacer() const;

	enum DISPLACER_FIRESTATE { FIRESTATE_NONE = 0, FIRESTATE_SPINUP, FIRESTATE_SPIN, FIRESTATE_FIRE };
	enum DISPLACER_FIREMODE { FIREMODE_NONE = 0, FIREMODE_FORWARD, FIREMODE_BACKWARD };
	enum DISPLACER_EFFECT { EFFECT_NONE = 0, EFFECT_CORE };

private:
	void ClearSpin();
	void SpinUp(int iFireMode);
	void Spin();
	void Fire(BOOL fIsPrimary);
	void Teleport();
	void Displace();

	CBeam *m_pBeam;

	BOOL ShouldUpdateEffects() const;
	unsigned short m_usDisplacer;
};
#endif

#endif // WEAPON_DISPLACER_H