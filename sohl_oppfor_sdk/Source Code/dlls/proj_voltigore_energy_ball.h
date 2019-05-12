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
// Projectile: Energy Ball for Voltigore
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef VOLTIGORE_ENERGY_BALL_H
#define VOLTIGORE_ENERGY_BALL_H

class CVoltigoreEnergyBall : public CBaseEntity
{
public:
	void Spawn();

	static void Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void EXPORT Touch(CBaseEntity *pOther);
	void EXPORT Think();
	void Precache();
	void Glow();
	void EXPORT Animate();

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	CSprite *m_pSprite;
	int m_maxFrame;
};

#endif // VOLTIGORE_ENERGY_BALL_H
