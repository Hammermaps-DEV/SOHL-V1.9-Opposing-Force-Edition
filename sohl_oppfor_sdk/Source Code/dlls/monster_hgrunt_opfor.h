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
// NPC: Human Grunt Ally
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_HGRUNT_OPFOR_H
#define MONSTER_HGRUNT_OPFOR_H

// include
#include "rcallymonster.h"

// class definition
class CHFGrunt : public CRCAllyMonster {
public:
	void Spawn();
	void Precache();
	void SetYawSpeed();
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void SetActivity(Activity NewActivity);
	void RunTask(Task_t *pTask);
	void StartTask(Task_t *pTask);
	void KeyValue(KeyValueData *pkvd);
	virtual int	ObjectCaps() { return CRCAllyMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
	bool CheckRangeAttack2(float flDot, float flDist);

	// Override these to set behavior
	Schedule_t *GetScheduleOfType(int Type);
	Schedule_t *GetSchedule();

	void GibMonster();
	void JustSpoke();

	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	// UNDONE: What is this for?  It isn't used?
	float m_flLastEnemySightTime;
	float m_flMedicWaitTime;

	BOOL	m_fFirstEncounter;// only put on the handsign show in the squad's first encounter.
	int		m_cClipSize;
	int		m_nShotgunDouble;

	int		m_iSentence;
	int		m_iHead;

	static const char *pGruntSentences[];

	CUSTOM_SCHEDULES;
};

// class definition
class CHFGruntRepel : public CBaseMonster {
public:
	void Spawn();
	void Precache();
	void EXPORT RepelUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int m_iSpriteTexture;	// Don't save, precache
};

// class definition
class CDeadFGrunt : public CBaseMonster {
public:
	void Spawn();
	int	Classify() { return	CLASS_PLAYER_ALLY; }

	void KeyValue(KeyValueData *pkvd);

	int		m_iHead;
	int	m_iPose;// which sequence to display
	static char *m_szPoses[7];
};

#endif // MONSTER_HGRUNT_OPFOR_H