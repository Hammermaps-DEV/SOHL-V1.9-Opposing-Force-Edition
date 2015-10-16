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

#ifndef MONSTER_ROACH_H
#define MONSTER_ROACH_H

class CRoach : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	void SetYawSpeed(void);
	void EXPORT MonsterThink(void);
	void Move(float flInterval);
	void PickNewDest(int iCondition);
	void EXPORT Touch(CBaseEntity *pOther);
	void Killed(entvars_t *pevAttacker, int iGib);

	float	m_flLastLightLevel;
	float	m_flNextSmellTime;
	int		Classify(void);
	void	Look(int iDistance);
	int		ISoundMask(void);

	// UNDONE: These don't necessarily need to be save/restored, but if we add more data, it may
	BOOL	m_fLightHacked;
	int		m_iMode;
	// -----------------------------
};

#endif // MONSTER_ROACH_H