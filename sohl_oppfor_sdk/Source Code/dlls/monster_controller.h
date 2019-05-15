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
#ifndef MONSTER_CONTROLLER_H
#define MONSTER_CONTROLLER_H

class CController : public CSquadMonster
{
public:
	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn();
	void Precache();
	void SetYawSpeed();
	int  Classify();
	void HandleAnimEvent(MonsterEvent_t *pEvent);

	void RunAI();
	bool CheckRangeAttack1(float flDot, float flDist);	// balls
	bool CheckRangeAttack2(float flDot, float flDist);	// head
	bool CheckMeleeAttack1(float flDot, float flDist) { return false; };	// block, throw
	Schedule_t* GetSchedule();
	Schedule_t* GetScheduleOfType(int Type);
	void StartTask(Task_t *pTask);
	void RunTask(Task_t *pTask);
	CUSTOM_SCHEDULES;

	void Stop();
	void Move(float flInterval);
	int  CheckLocalMove(const Vector &vecStart, const Vector &vecEnd, CBaseEntity *pTarget, float *pflDist);
	void MoveExecute(CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval);
	void SetActivity(Activity NewActivity);
	BOOL ShouldAdvanceRoute(float flWaypointDist);
	int LookupFloat();

	float m_flNextFlinch;

	float m_flShootTime;
	float m_flShootEnd;

	void PainSound();
	void AlertSound();
	void IdleSound();
	void AttackSound();
	void DeathSound();

	static const char *pAttackSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pDeathSounds[];

	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	void Killed(entvars_t *pevAttacker, int iGib);
	void GibMonster();

	CSprite *m_pBall[2];	// hand balls
	int m_iBall[2];			// how bright it should be
	float m_iBallTime[2];	// when it should be that color
	int m_iBallCurrent[2];	// current brightness

	Vector m_vecEstVelocity;

	Vector m_velocity;
	int m_fInCombat;
};

class CControllerHeadBall : public CBaseMonster
{
	void Spawn();
	void Precache();
	void EXPORT HuntThink();
	void EXPORT DieThink();
	void EXPORT BounceTouch(CBaseEntity *pOther);
	void MovetoTarget(Vector vecTarget);
	void Crawl();
	int m_iTrail;
	int m_flNextAttack;
	Vector m_vecIdeal;
	EHANDLE m_hOwner;
};

class CControllerZapBall : public CBaseMonster
{
	void Spawn();
	void Precache();
	void EXPORT AnimateThink();
	void EXPORT ExplodeTouch(CBaseEntity *pOther);

	EHANDLE m_hOwner;
};

#endif // MONSTER_CONTROLLER_H