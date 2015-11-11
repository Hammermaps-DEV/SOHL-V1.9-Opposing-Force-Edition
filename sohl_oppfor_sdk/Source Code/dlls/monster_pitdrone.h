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
//=========================================================
#ifndef MONSTER_PITDRONE_H
#define MONSTER_PITDRONE_H

// class definition
class CPitDrone : public CBaseMonster {
	public:
		void Spawn(void);
		void Precache(void);
		void HandleAnimEvent(MonsterEvent_t *pEvent);
		void IdleSound(void);
		void PainSound(void);
		void AlertSound(void);
		void DeathSound(void);
		void AttackSound(void);
		void AttackSoundSpike(void);
		void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
		void UpdateHorns(void);
		void SetYawSpeed(void);
		void KeyValue(KeyValueData *pkvd);
		void StopTalking(void);
		void StartTask(Task_t *pTask);
		void RunTask(Task_t *pTask);
		void RunAI(void);

		BOOL CheckRangeAttack1(float flDot, float flDist);
		BOOL ShouldSpeak(void);

		int Classify(void);
		int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
		int IRelationship(CBaseEntity *pTarget);
		int IgnoreConditions(void);

		virtual int	Save(CSave &save);
		virtual int	Restore(CRestore &restore);

		static	TYPEDESCRIPTION m_SaveData[];

		static const char *pIdleSounds[];
		static const char *pAlertSounds[];
		static const char *pPainSounds[];
		static const char *pDieSounds[];
		static const char *pAttackHitSounds[];
		static const char *pAttackHitStrikeSounds[];
		static const char *pAttackMissSounds[];
		static const char *pAttackSoundsSpike[];

		Schedule_t* GetSchedule(void);
		Schedule_t* GetScheduleOfType(int Type);
		MONSTERSTATE GetIdealState(void);

		CUSTOM_SCHEDULES;

	protected:
		BOOL    m_fCanThreatDisplay;
		BOOL    m_flDebug = false;
		int		m_iLastWord;
		int		m_iSpitSprite;
		float	m_flNextSpitTime;
		float	m_flLastHurtTime;
		float	m_flNextSpeakTime;
		float	m_flNextWordTime;
		float	m_flNextFlinch;
		float	m_flammo = 6;
		float	m_flhorns;
};

#endif // MONSTER_PITDRONE_H