/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Half-Life and their logos are the property of their respective owners.
*   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*   This product contains software technology licensed from Id
*   Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
*	Spirit of Half-Life, by Laurie R. Cheers. (LRC)
*   Modified by Lucas Brucksch (Code merge & Effects)
*   Modified by Andrew J Hamilton (AJH)
*   Modified by XashXT Group (g-cont...)
*
*   Code used from Battle Grounds Team and Contributors.
*   Code used from SamVanheer (Opposing Force code)
*   Code used from FWGS Team (Fixes for SOHL)
*   Code used from LevShisterov (Bugfixed and improved HLSDK)
*	Code used from Fograin (Half-Life: Update MOD)
*
***/
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
	void Spawn();
	void Precache();
	void DLLEXPORT LeapTouch(CBaseEntity *pOther);
	void PainSound();
	void DeathSound();
	void IdleSound();
	void AlertSound();
	void AttackSound();
	void PrescheduleThink();
	void StartTask(Task_t* pTask);
	void KeyValue(KeyValueData *pkvd);
	void HandleAnimEvent(MonsterEvent_t *pEvent);

	// int void
	int Classify();

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