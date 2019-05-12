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
// Weapon: Gluon Gun * http://half-life.wikia.com/wiki/Gluon_Gun
//=========================================================

#ifndef WEAPON_EGON_H
#define WEAPON_EGON_H

//Model Animations | Sequence-ID | Frames | FPS
enum class EGON_IDLE1 { sequence = 0, frames = 61, fps = 30 };
enum class EGON_FIDGET1 { sequence = 1, frames = 81, fps = 30 };
enum class EGON_ALTFIREON { sequence = 2, frames = 46, fps = 30 };
enum class EGON_ALTFIREOFF { sequence = 3, frames = 21, fps = 30 };
enum class EGON_FIRESTOP { sequence = 4, frames = 2, fps = 10 };
enum class EGON_FIRECYCLE { sequence = 5, frames = 16, fps = 20 };
enum class EGON_DRAW { sequence = 6, frames = 16, fps = 30 };
enum class EGON_HOLSTER { sequence = 7, frames = 16, fps = 30 };

#ifndef CLIENT_DLL //Only in Server-DLL
//Gluon Gun Base-Class | Base | Attack | Animations | Vars | Events
class CEgon : public CBasePlayerWeapon {
public:
	int	Save(CSave &save);
	int	Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	void Spawn();
	void Precache();
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy();
	void Holster();

	void CreateEffect();
	void UpdateEffect(const Vector &startPoint, const Vector &endPoint, float timeBlend);
	void DestroyEffect();

	void EndAttack();
	void UpdateScreen();
	void ShutdownScreen();
	void PrimaryAttack();
	void SecondaryAttack();
	void WeaponIdle();

	float m_flAmmoUseTime;// since we use < 1 point of ammo per update, we subtract ammo on a timer.

	void Fire(const Vector &vecOrigSrc, const Vector &vecDir);

	BOOL HasAmmo();
	void UseAmmo(int count);

	int flags() { return ENGINE_CANSKIP(m_pPlayer->edict()) ? NULL : FEV_NOTHOST; };

	enum EGON_FIRESTATE { FIRE_OFF = 0, FIRE_CHARGE };
	enum EGON_FIREMODE { FIRE_NARROW = 0, FIRE_WIDE };

private:
	float				m_shootTime;
	CBeam				*m_pBeam;
	CBeam				*m_pNoise;
	CSprite				*m_pSprite;
	EGON_FIRESTATE		m_fireState;
	EGON_FIREMODE		m_fireMode;
	float				m_shakeTime;
	unsigned int 		m_usEgonFire;
	unsigned int		m_usEgonStop;
};
#endif

#endif // WEAPON_EGON_H