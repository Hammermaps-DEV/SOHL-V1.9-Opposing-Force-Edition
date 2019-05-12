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
// NPC: Pit Drone * http://half-life.wikia.com/wiki/Pit_Drone
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_PITDRONE_H
#define MONSTER_PITDRONE_H

// class definition
class CPitDrone : public CBaseMonster {
public:
	void Spawn();
	void Precache();
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void IdleSound();
	void PainSound();
	void AlertSound();
	void DeathSound();
	void AttackSound();
	void AttackSoundSpike();
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void UpdateHorns();
	void SetYawSpeed();
	void KeyValue(KeyValueData *pkvd);
	void StopTalking();
	void StartTask(Task_t *pTask);
	void RunTask(Task_t *pTask);
	void RunAI();

	bool CheckRangeAttack1(float flDot, float flDist);
	bool ShouldSpeak();

	int Classify();
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	int IRelationship(CBaseEntity *pTarget);
	int IgnoreConditions();

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

	Schedule_t* GetSchedule();
	Schedule_t* GetScheduleOfType(int Type);
	MONSTERSTATE GetIdealState();

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