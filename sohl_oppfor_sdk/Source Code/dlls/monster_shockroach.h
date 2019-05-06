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
// NPC: Shock-Roach * http://half-life.wikia.com/wiki/Shock_Roach
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_SHOCKROACH_H
#define MONSTER_SHOCKROACH_H

// include
#include "monster_headcrab.h"

// class definition
class CShockRoach : public CHeadCrab {
public:
	// void
	void Spawn(void);
	void Precache(void);
	void EXPORT LeapTouch(CBaseEntity *pOther);
	void PainSound(void);
	void DeathSound(void);
	void IdleSound(void);
	void AlertSound(void);
	void AttackSound(void);
	void PrescheduleThink(void);
	void StartTask(Task_t* pTask);
	void KeyValue(KeyValueData *pkvd);
	void HandleAnimEvent(MonsterEvent_t *pEvent);

	// int void
	int Classify(void);

	// virtual int
	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	virtual int GetVoicePitch(long random) { return m_flPitch + random; }

	// static
	static TYPEDESCRIPTION m_SaveData[];

protected:
	static const char *pIdleSounds[];
	static const char *pAttackSounds[];

	// vars
	float m_flDie;
	int m_flPitch = 100;
	bool m_iDies = true;
	bool m_iWeapon = true;
};

#endif // MONSTER_SHOCKROACH_H