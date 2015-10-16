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

#include "monster_barney.h"

class COtis : public CBarney
{
public:
	void Spawn(void);
	void Precache(void);
	void BarneyFirePistol(void);
	void KeyValue(KeyValueData *pkvd);
	void AlertSound(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);

	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);

	void DeclineFollowing(void);

	// Override these to set behavior
	Schedule_t *GetSchedule(void);

	void TalkInit(void);
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void Killed(entvars_t *pevAttacker, int iGib);

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	BOOL	m_fSuspicious;
	int		head;
};

#endif // MONSTER_OTIS_H