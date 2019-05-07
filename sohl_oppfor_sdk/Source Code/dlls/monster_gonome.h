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
// NPC: Gonome
//=========================================================

#ifndef MONSTER_GONOME_H
#define MONSTER_GONOME_H

#include "monster_bullsquid.h"

class CGonome : public CBullsquid {
public:
	void Spawn(void);
	void Precache(void);
	void SetYawSpeed(void);
	int  ISoundMask(void);

	int  Classify(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void PainSound(void);
	void DeathSound(void);
	void AlertSound(void);
	void IdleSound(void);
	void AttackSound(void);
	void StartTask(Task_t *pTask);

	int	Save(CSave &save);
	int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	Schedule_t *GetSchedule(void);
	Schedule_t *GetScheduleOfType(int Type);

	static const char *pAttackSounds[];
	static const char *pIdleSounds[];
	static const char *pPainSounds[];
	static const char *pDeathSounds[];
	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];

	BOOL CheckMeleeAttack1(float flDot, float flDist);
	BOOL CheckMeleeAttack2(float flDot, float flDist);
	BOOL CheckRangeAttack1(float flDot, float flDist);
	void RunAI(void);

	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	int IgnoreConditions(void);
	MONSTERSTATE GetIdealState(void);
	CUSTOM_SCHEDULES;

	int iGonomeSpitSprite;
	float m_flLastHurtTime;// we keep track of this, because if something hurts a gonome, it will forget about its love of headcrabs for a while.
	float m_flNextSpitTime;// last time the Gonome used the spit attack.
	float m_flNextFlinch;
};

#endif // MONSTER_GONOME_H