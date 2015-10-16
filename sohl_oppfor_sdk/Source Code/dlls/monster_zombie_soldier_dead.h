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

#ifndef MONSTER_ZOMBIE_SOLDIER_DEAD_H
#define MONSTER_ZOMBIE_SOLDIER_DEAD_H

#include "monster_zombie_dead.h"

class CDeadZombieSoldier : public CDeadZombie
{
public:
	void Spawn(void);
	int	Classify(void) { return	CLASS_HUMAN_MILITARY; }
	void KeyValue(KeyValueData *pkvd);

	static char *m_szPoses[2];
};

#endif // MONSTER_ZOMBIE_SOLDIER_DEAD_