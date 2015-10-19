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
// NPC: Pit Drone * http://half-life.wikia.com/wiki/Pit_Drone
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00001 / Date: 19.10.2015
//=========================================================

#ifndef MONSTER_PITDRONE_H
#define MONSTER_PITDRONE_H

class CPitDrone : public CBaseMonster {
	void Spawn(void);
	void Precache(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void CPitDrone::SetYawSpeed(void);
	void KeyValue(KeyValueData *pkvd);

	int Classify(void);

	BOOL CheckRangeAttack1(float flDot, float flDist);
	void IdleSound(void);
	void PainSound(void);
	void AlertSound(void);
	void DeathSound(void);
	void AttackSound(void);
	void AttackSoundMiss(void);
	void AttackSoundSpike(void);
	void UpdateHorns(void);
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
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
	virtual int GetVoicePitch(long random) { return m_flPitch + random; }

	BOOL m_fCanThreatDisplay;// this is so the squid only does the "I see a headcrab!" dance one time.
	BOOL m_flDebug = false;
	float	m_flNextSpitTime;// last time the PitDrone used the spit attack.
	float	m_flLastHurtTime;
	float	m_flNextSpeakTime;
	float	m_flNextWordTime;
	float	m_flNextFlinch;
	float	m_flammo = 6;
	float	m_flhorns;
	int		m_iLastWord;
	int		m_flPitch = 100;

	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pDieSounds[];
	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];
	static const char *pAttackSoundsSpike[];

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];
};

#endif // MONSTER_PITDRONE_H