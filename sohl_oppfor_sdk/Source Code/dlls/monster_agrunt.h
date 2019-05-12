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

#ifndef MONSTER_AGRUNT_H
#define MONSTER_AGRUNT_H

class CAGrunt : public CSquadMonster
{
public:
	void Spawn() override;
	void Precache() override;
	void SetYawSpeed() override;
	int  Classify() override;
	int  ISoundMask() override;
	void HandleAnimEvent(MonsterEvent_t *pEvent) override;
	void SetObjectCollisionBox() override
	{
		pev->absmin = pev->origin + Vector(-32, -32, 0);
		pev->absmax = pev->origin + Vector(32, 32, 85);
	}

	Schedule_t* GetSchedule() override;
	Schedule_t* GetScheduleOfType(int Type) override;
	bool FCanCheckAttacks() override;
	bool CheckMeleeAttack1(float flDot, float flDist) override;
	bool CheckRangeAttack1(float flDot, float flDist) override;
	void StartTask(Task_t *pTask) override;
	void AlertSound() override;
	void DeathSound() override;
	void PainSound() override;
	void AttackSound() override;
	void PrescheduleThink() override;
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) override;
	int IRelationship(CBaseEntity *pTarget) override;
	void StopTalking();
	bool ShouldSpeak();
	void Killed(entvars_t *pevAttacker, int iGib) override;

	CUSTOM_SCHEDULES;

	int	Save(CSave &save) override;
	int	Restore(CRestore &restore) override;
	static	TYPEDESCRIPTION m_SaveData[];

	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];
	static const char *pAttackSounds[];
	static const char *pDieSounds[];
	static const char *pPainSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];

	bool	m_fCanHornetAttack;
	float	m_flNextHornetAttackCheck;

	float m_flNextPainTime;

	// three hacky fields for speech stuff. These don't really need to be saved.
	float	m_flNextSpeakTime;
	float	m_flNextWordTime;
	int		m_iLastWord;
};

#endif // MONSTER_AGRUNT_H