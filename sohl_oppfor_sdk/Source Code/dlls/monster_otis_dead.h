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

#ifndef MONSTER_OTIS_H
#define MONSTER_OTIS_H

#include "monster_barney_dead.h"

class CDeadOtis : public CDeadBarney
{
public:
	void Spawn(void);
	int	Classify(void) { return	CLASS_PLAYER_ALLY; }
	void KeyValue(KeyValueData *pkvd);
	static char *m_szPoses[5];

	int	head;
	int	m_iBaseBody;
};

#endif // MONSTER_OTIS_H