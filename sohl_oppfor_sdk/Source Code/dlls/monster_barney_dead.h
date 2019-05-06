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

#ifndef MONSTER_BARNEY_DEAD_H
#define MONSTER_BARNEY_DEAD_H

class CDeadBarney : public CBaseMonster
{
public:
	void Spawn(void);
	int	Classify(void) { return	CLASS_PLAYER_ALLY; }
	void KeyValue(KeyValueData *pkvd);
	static char *m_szPoses[3];

protected:
	int	m_iPose;// which sequence to display	-- temporary, don't need to save
};

#endif // MONSTER_BARNEY_DEAD_H