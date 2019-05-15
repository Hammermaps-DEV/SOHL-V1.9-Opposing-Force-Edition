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

#ifndef MONSTER_HGRUNT_H
#define MONSTER_HGRUNT_H

class CHGrunt : public CSquadMonster
{
public:
	void Spawn();
	void Precache();
	void SetYawSpeed();
	int  Classify();
	int ISoundMask();
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	bool FCanCheckAttacks();
	bool CheckMeleeAttack1(float flDot, float flDist);
	bool CheckRangeAttack1(float flDot, float flDist);
	bool CheckRangeAttack2(float flDot, float flDist);
	void CheckAmmo();
	void SetActivity(Activity NewActivity);
	void StartTask(Task_t *pTask);
	void RunTask(Task_t *pTask);
	void DeathSound();
	void PainSound();
	void IdleSound();
	Vector GetGunPosition();
	void ShootMP5();
	void ShootShotgun();
	void ShootShotgunDouble();
	void PrescheduleThink();
	void GibMonster();
	void SpeakSentence();
	void KeyValue(KeyValueData *pkvd);

	int	Save(CSave &save);
	int Restore(CRestore &restore);

	CBaseEntity	*Kick();
	Schedule_t	*GetSchedule();
	Schedule_t  *GetScheduleOfType(int Type);
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);

	int IRelationship(CBaseEntity *pTarget);

	BOOL FOkToSpeak();
	void JustSpoke();

	CUSTOM_SCHEDULES;
	static TYPEDESCRIPTION m_SaveData[];

	// checking the feasibility of a grenade toss is kind of costly, so we do it every couple of seconds,
	// not every server frame.
	float m_flNextGrenadeCheck;
	float m_flNextPainTime;
	float m_flLastEnemySightTime;

	Vector	m_vecTossVelocity;

	BOOL	m_fThrowGrenade;
	BOOL	m_fStanding;
	BOOL	m_fFirstEncounter;// only put on the handsign show in the squad's first encounter.
	int		m_cClipSize;

	static const char *pPainSounds[];
	static const char *pDeathSounds[];
	static const char *pAttackSoundsMP5[];

	int m_voicePitch;

	int		m_iBrassShell;
	int		m_iShotgunShell;
	float	m_fLockShootTime;
	float	m_nShotgunDouble;
	int		m_iSentence;
	int		g_fGruntQuestion; // true if an idle grunt asked a question. Cleared when someone answers.
	int		m_iszSpeakAs; // Change the prefix for all this monster's speeches LRC

	static const char *pGruntSentences[];
};

class CHGruntRepel : public CBaseMonster
{
public:
	void Spawn();
	void Precache();
	void DLLEXPORT RepelUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int m_iSpriteTexture;	// Don't save, precache
};

#endif // MONSTER_HGRUNT_H