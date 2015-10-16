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

#ifndef MONSTER_PITDRONE_H
#define MONSTER_PITDRONE_H

class CPitDroneSpit : public CBaseMonster
{
	void Spawn(void);
	void Precache(void);
	int  Classify(void);
	void EXPORT Touch(CBaseEntity *pOther);

	Vector m_vecForward;

public:
	static CPitDroneSpit *SpitCreate(void);
};

class CPitDrone : public CBaseMonster
{
	void Spawn(void);
	void Precache(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void SetYawSpeed(void);
	void KeyValue(KeyValueData *pkvd);

	int Classify(void);

	BOOL CheckRangeAttack1(float flDot, float flDist);
	void IdleSound(void);
	void PainSound(void);
	void AlertSound(void);
	void DeathSound(void);
	void AttackSound(void);
	void BodyChange(float horns);
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	int IRelationship(CBaseEntity *pTarget);
	int IgnoreConditions(void);
	void StopTalking(void);
	BOOL ShouldSpeak(void);
	Schedule_t* GetSchedule(void);
	Schedule_t* GetScheduleOfType(int Type);
	void StartTask(Task_t *pTask);
	void RunTask(Task_t *pTask);
	void RunAI(void);
	CUSTOM_SCHEDULES;
	MONSTERSTATE GetIdealState(void);

	BOOL m_fCanThreatDisplay;// this is so the squid only does the "I see a headcrab!" dance one time.
	float	m_flNextSpitTime;// last time the PitDrone used the spit attack.
	float	m_flLastHurtTime;
	float	m_flNextSpeakTime;
	float	m_flNextWordTime;
	float	m_flNextFlinch;
	int		m_iLastWord;

	float m_ammo;
	float horns;

	static const char *pAttackSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pDieSounds[];
	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];
};

#endif // MONSTER_PITDRONE_H