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

#ifndef MONSTER_HEADCRAB_H
#define MONSTER_HEADCRAB_H

class CHeadCrab : public CBaseMonster {
public:
	void Spawn();
	void Precache();
	void RunTask(Task_t *pTask);
	void StartTask(Task_t *pTask);
	void SetYawSpeed();
	void EXPORT LeapTouch(CBaseEntity *pOther);
	void PainSound();
	void DeathSound();
	void IdleSound();
	void AlertSound();
	void PrescheduleThink();
	int  Classify();
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	bool CheckRangeAttack1(float flDot, float flDist);
	bool CheckRangeAttack2(float flDot, float flDist) { return FALSE; };
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);

	virtual float GetDamageAmount() { return gSkillData.headcrabDmgBite; }
	virtual int GetVoicePitch() { return 100; }
	virtual float GetSoundVolue() { return 1.0; }
	Schedule_t* GetScheduleOfType(int Type);

	CUSTOM_SCHEDULES;

	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pAttackSounds[];
	static const char *pDeathSounds[];
	static const char *pBiteSounds[];
};

#endif // MONSTER_HEADCRAB_H