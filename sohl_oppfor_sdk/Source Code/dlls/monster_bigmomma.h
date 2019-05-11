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

#ifndef MONSTER_BIGMOMMA_H
#define MONSTER_BIGMOMMA_H

// AI Nodes for Big Momma
class CInfoBM : public CPointEntity
{
public:
	void Spawn(void);
	void KeyValue(KeyValueData* pkvd);

	// name in pev->targetname
	// next in pev->target
	// radius in pev->scale
	// health in pev->health
	// Reach target in pev->message
	// Reach delay in pev->speed
	// Reach sequence in pev->netname

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int		m_preSequence;
};

class CBMortar : public CBaseEntity
{
public:
	void Spawn(void);

	static CBMortar *Shoot(edict_t *pOwner, Vector vecStart, Vector vecVelocity);
	void Touch(CBaseEntity *pOther);
	void EXPORT Animate(void);

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int  m_maxFrame;
};

#define BIG_MAXCHILDREN		20			// Max # of live headcrab children

class CBigMomma : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	void KeyValue(KeyValueData *pkvd);
	void Activate(void);
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);

	void		RunTask(Task_t *pTask);
	void		StartTask(Task_t *pTask);
	Schedule_t	*GetSchedule(void);
	Schedule_t	*GetScheduleOfType(int Type);
	void		TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void		SetActivity(Activity NewActivity);

	void NodeStart(int iszNextNode);
	void NodeReach(void);
	BOOL ShouldGoToNode(void);

	void SetYawSpeed(void);
	int  Classify(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void LayHeadcrab(void);

	int GetNodeSequence(void)
	{
		CBaseEntity *pTarget = m_hTargetEnt;
		if (pTarget)
		{
			return pTarget->pev->netname;	// netname holds node sequence
		}
		return 0;
	}


	int GetNodePresequence(void)
	{
		CInfoBM *pTarget = (CInfoBM *)(CBaseEntity *)m_hTargetEnt;
		if (pTarget)
		{
			return pTarget->m_preSequence;
		}
		return 0;
	}

	float GetNodeDelay(void)
	{
		CBaseEntity *pTarget = m_hTargetEnt;
		if (pTarget)
		{
			return pTarget->pev->speed;	// Speed holds node delay
		}
		return 0;
	}

	float GetNodeRange(void)
	{
		CBaseEntity *pTarget = m_hTargetEnt;
		if (pTarget)
		{
			return pTarget->pev->scale;	// Scale holds node delay
		}
		return 1e6;
	}

	float GetNodeYaw(void)
	{
		CBaseEntity *pTarget = m_hTargetEnt;
		if (pTarget)
		{
			if (pTarget->pev->angles.y != 0)
				return pTarget->pev->angles.y;
		}
		return pev->angles.y;
	}

	// Restart the crab count on each new level
	void OverrideReset(void)
	{
		m_crabCount = 0;
	}

	void DeathNotice(entvars_t *pevChild);

	BOOL CanLayCrab(void)
	{
		if (m_crabTime < UTIL_GlobalTimeBase() && m_crabCount < BIG_MAXCHILDREN)
		{
			// Don't spawn crabs inside each other
			Vector mins = pev->origin - Vector(32, 32, 0);
			Vector maxs = pev->origin + Vector(32, 32, 0);

			CBaseEntity *pList[2];
			int count = UTIL_EntitiesInBox(pList, 2, mins, maxs, FL_MONSTER);
			for (int i = 0; i < count; i++)
			{
				if (pList[i] != this)	// Don't hurt yourself!
					return FALSE;
			}
			return TRUE;
		}

		return FALSE;
	}

	void LaunchMortar(void);

	void SetObjectCollisionBox(void)
	{
		pev->absmin = pev->origin + Vector(-95, -95, 0);
		pev->absmax = pev->origin + Vector(95, 95, 190);
	}

	bool CheckMeleeAttack1(float flDot, float flDist);	// Slash
	bool CheckMeleeAttack2(float flDot, float flDist) { return CanLayCrab(); };	// Lay a crab
	bool CheckRangeAttack1(float flDot, float flDist);	// Mortar launch

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	static const char *pChildDieSounds[];
	static const char *pSackSounds[];
	static const char *pDeathSounds[];
	static const char *pAttackSounds[];
	static const char *pAttackHitSounds[];
	static const char *pBirthSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pFootSounds[];

	CUSTOM_SCHEDULES;

private:
	float	m_nodeTime;
	float	m_crabTime;
	float	m_mortarTime;
	float	m_painSoundTime;
	int		m_crabCount;
};

#endif // MONSTER_BIGMOMMA_H