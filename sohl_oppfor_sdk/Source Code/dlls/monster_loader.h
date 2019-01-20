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
#ifndef MONSTER_LOADER_H
#define MONSTER_LOADER_H

#include "soundent.h"

#define	SF_LOADER_NOTSOLID	4 

class CLoader : public CBaseMonster
{
public:
	void Spawn(void) override;
	void Precache(void) override;
	void SetYawSpeed() override;
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) override;
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override;
	void StartTask(Task_t* pTask) override;
	void SetTurnActivity() override;
	int Classify() override { return CLASS_PLAYER_ALLY; }
	int ISoundMask() override { return bits_SOUND_NONE; }
};

#endif // MONSTER_LOADER_H