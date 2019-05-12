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

#ifndef MONSTER_ALIEN_SLAVE_H
#define MONSTER_ALIEN_SLAVE_H

#define	ISLAVE_MAX_BEAMS	8

class CISlave : public CSquadMonster
{
public:
	void Spawn();
	void Precache();
	void SetYawSpeed();
	int	 ISoundMask();
	int  Classify();
	int  IRelationship(CBaseEntity *pTarget);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	bool CheckRangeAttack1(float flDot, float flDist);
	bool CheckRangeAttack2(float flDot, float flDist);
	void CallForHelp(char *szClassname, float flDist, EHANDLE hEnemy, Vector &vecLocation);
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);

	void DeathSound();
	void PainSound();
	void AlertSound();
	void IdleSound();

	void Killed(entvars_t *pevAttacker, int iGib);

	void StartTask(Task_t *pTask);
	Schedule_t *GetSchedule();
	Schedule_t *GetScheduleOfType(int Type);
	CUSTOM_SCHEDULES;

	int	Save(CSave &save);
	int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	void ClearBeams();
	void ArmBeam(int side);
	void WackBeam(int side, CBaseEntity *pEntity);
	void ZapBeam(int side);
	void BeamGlow();

	int m_iBravery;

	CBeam *m_pBeam[ISLAVE_MAX_BEAMS];

	int m_iBeams;
	float m_flNextAttack;

	int	m_voicePitch;

	EHANDLE m_hDead;

	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];
	static const char *pPainSounds[];
	static const char *pDeathSounds[];

protected:
	BOOL m_flDebug = false;
	float m_flHitgroupHead;
	float m_flHitgroupChest;
	float m_flHitgroupStomach;
	float m_flHitgroupArm;
	float m_flHitgroupLeg;
	float m_flDmgOneSlash;
	float m_flDmgBothSlash;
};

#endif // MONSTER_ALIEN_SLAVE_H