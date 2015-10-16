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
#ifndef MONSTER_RAT_H
#define MONSTER_RAT_H

class CRat : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	void SetYawSpeed(void);
	void EXPORT MonsterThink(void);
	void Move(float flInterval);
	void PickNewDest(int iCondition);
	void Killed(entvars_t *pevAttacker, int iGib);

	float	m_flNextSmellTime;
	int		Classify(void);
	void	Look(int iDistance);
	int		ISoundMask(void);

	int		m_iMode;
};

#endif // MONSTER_RAT_H