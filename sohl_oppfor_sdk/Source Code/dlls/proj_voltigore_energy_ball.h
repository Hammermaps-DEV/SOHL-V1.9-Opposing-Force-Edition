/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
//=========================================================
// Projectile: Energy Ball for Voltigore
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef VOLTIGORE_ENERGY_BALL_H
#define VOLTIGORE_ENERGY_BALL_H

class CVoltigoreEnergyBall : public CBaseEntity
{
public:
	void Spawn(void);

	static void Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void EXPORT Touch(CBaseEntity *pOther);
	void EXPORT Think(void);
	void Precache(void);
	void Glow(void);
	void EXPORT Animate(void);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	CSprite *m_pSprite;
	int m_maxFrame;
};

#endif // VOLTIGORE_ENERGY_BALL_H
