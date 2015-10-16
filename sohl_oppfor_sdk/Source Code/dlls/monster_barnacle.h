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

#ifndef MONSTER_BARNACLE_H
#define MONSTER_BARNACLE_H

class CBarnacle : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	CBaseEntity *TongueTouchEnt(float *pflLength);
	int  Classify(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void EXPORT BarnacleThink(void);
	void EXPORT WaitTillDead(void);
	void Killed(entvars_t *pevAttacker, int iGib);
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	float m_flAltitude;
	float m_flKillVictimTime;
	int	  m_cGibs;// barnacle loads up on gibs each time it kills something.
	BOOL  m_fTongueExtended;
	BOOL  m_fLiftingPrey;
	float m_flTongueAdj;
};

#endif // MONSTER_BARNACLE_H