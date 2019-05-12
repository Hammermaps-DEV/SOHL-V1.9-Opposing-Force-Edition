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

#ifndef MONSTER_ICHTHYOSAUR_H
#define MONSTER_ICHTHYOSAUR_H

// UNDONE: Save/restore here
class CIchthyosaur : public CFlyingMonster
{
public:
	void  Spawn();
	void  Precache();
	void  SetYawSpeed();
	int   Classify();
	void  HandleAnimEvent(MonsterEvent_t *pEvent);
	CUSTOM_SCHEDULES;

	int	Save(CSave &save);
	int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	Schedule_t *GetSchedule();
	Schedule_t *GetScheduleOfType(int Type);

	void Killed(entvars_t *pevAttacker, int iGib);
	void BecomeDead();

	void EXPORT CombatUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT BiteTouch(CBaseEntity *pOther);

	void  StartTask(Task_t *pTask);
	void  RunTask(Task_t *pTask);

	bool  CheckMeleeAttack1(float flDot, float flDist);
	bool  CheckRangeAttack1(float flDot, float flDist);

	float ChangeYaw(int speed);
	Activity GetStoppedActivity();

	void  Move(float flInterval);
	void  MoveExecute(CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval);
	void  MonsterThink();
	void  Stop();
	void  Swim();
	Vector DoProbe(const Vector &Probe);

	float VectorToPitch(const Vector &vec);
	float FlPitchDiff();
	float ChangePitch(int speed);

	Vector m_SaveVelocity;
	float m_idealDist;

	float m_flBlink;

	float m_flEnemyTouched;
	BOOL  m_bOnAttack;

	float m_flMaxSpeed;
	float m_flMinSpeed;
	float m_flMaxDist;

	CBeam *m_pBeam;

	float m_flNextAlert;

	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pAttackSounds[];
	static const char *pBiteSounds[];
	static const char *pDieSounds[];
	static const char *pPainSounds[];

	void IdleSound();
	void AlertSound();
	void AttackSound();
	void BiteSound();
	void DeathSound();
	void PainSound();
};


#endif // MONSTER_ICHTHYOSAUR_H