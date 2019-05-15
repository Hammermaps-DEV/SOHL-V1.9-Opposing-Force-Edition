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

#ifndef MONSTER_BULLSQUID_H
#define MONSTER_BULLSQUID_H

class CSquidSpit : public CBaseEntity
{
public:
	void Spawn();

	static void Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void Touch(CBaseEntity *pOther);
	void DLLEXPORT Animate();

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int  m_maxFrame;
};

class CBullsquid : public CBaseMonster
{
public:
	void Spawn();
	void Precache();
	void SetYawSpeed();
	int  ISoundMask();
	int  Classify();
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void IdleSound();
	void PainSound();
	void DeathSound();
	void AlertSound();
	void AttackSound();
	void StartTask(Task_t *pTask);
	void RunTask(Task_t *pTask);
	bool CheckMeleeAttack1(float flDot, float flDist);
	bool CheckMeleeAttack2(float flDot, float flDist);
	bool CheckRangeAttack1(float flDot, float flDist);
	void RunAI();
	BOOL FValidateHintType(short sHint);
	Schedule_t *GetSchedule();
	Schedule_t *GetScheduleOfType(int Type);
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	int IRelationship(CBaseEntity *pTarget);
	int IgnoreConditions();
	MONSTERSTATE GetIdealState();

	int	Save(CSave &save);
	int Restore(CRestore &restore);

	CUSTOM_SCHEDULES;
	static TYPEDESCRIPTION m_SaveData[];

	BOOL m_fCanThreatDisplay;// this is so the squid only does the "I see a headcrab!" dance one time. 

	float m_flLastHurtTime;// we keep track of this, because if something hurts a squid, it will forget about its love of headcrabs for a while.
	float m_flNextSpitTime;// last time the bullsquid used the spit attack.
};

#endif // MONSTER_BULLSQUID_H