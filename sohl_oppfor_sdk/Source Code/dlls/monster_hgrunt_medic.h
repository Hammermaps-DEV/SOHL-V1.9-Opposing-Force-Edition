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