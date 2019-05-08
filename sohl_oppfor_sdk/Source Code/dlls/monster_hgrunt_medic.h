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

#ifndef MONSTER_HGRUNT_MEDIC_H
#define MONSTER_HGRUNT_MEDIC_H

#include "monster_hgrunt_opfor.h"

class CMedic : public CRCAllyMonster
{
public:
	void Spawn(void);
	void Precache(void);
	void SetYawSpeed(void);
	void KeyValue(KeyValueData *pkvd);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void SetActivity(Activity NewActivity);
	void RunTask(Task_t *pTask);
	void StartTask(Task_t *pTask);
	virtual int	ObjectCaps(void) { return CRCAllyMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	BOOL CheckRangeAttack1(float flDot, float flDist);

	void HealerFollow(CBaseEntity *pHealTarget);

	BOOL CanHeal(void); // Can we heal the player, or the injured grunt?
	void Heal(void);// Lets apply the healing.

	// Override these to set behavior
	Schedule_t *GetScheduleOfType(int Type);
	Schedule_t *GetSchedule(void);

	void GibMonster(void);
	void JustSpoke(void);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	float m_flLastEnemySightTime;

	BOOL	m_fFirstEncounter;// only put on the handsign show in the squad's first encounter.
	int		m_cClipSize;

	BOOL	m_fDepleteLine;

	int		m_iHead;
	int		m_iSentence;
	float	m_flHealAnount;

	static const char *pGruntSentences[];

	CUSTOM_SCHEDULES;

private:
	float m_healTime;
};

class CMedicRepel : public CHFGruntRepel
{
public:
	void Spawn(void);
	void Precache(void);
	void EXPORT RepelUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int m_iSpriteTexture;	// Don't save, precache
};

#endif // MONSTER_HGRUNT_MEDIC_H