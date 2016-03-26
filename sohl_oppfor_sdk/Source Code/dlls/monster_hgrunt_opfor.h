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
		void Spawn(void);
		void Precache(void);
		void SetYawSpeed(void);
		void HandleAnimEvent(MonsterEvent_t *pEvent);
		void SetActivity(Activity NewActivity);
		void RunTask(Task_t *pTask);
		void StartTask(Task_t *pTask);
		void KeyValue(KeyValueData *pkvd);
		virtual int	ObjectCaps(void) { return CRCAllyMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
		BOOL CheckRangeAttack2(float flDot, float flDist);

		// Override these to set behavior
		Schedule_t *GetScheduleOfType(int Type);
		Schedule_t *GetSchedule(void);

		void GibMonster(void);
		void JustSpoke(void);

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
		void Spawn(void);
		void Precache(void);
		void EXPORT RepelUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
		int m_iSpriteTexture;	// Don't save, precache
};

// class definition
class CDeadFGrunt : public CBaseMonster {
	public:
		void Spawn(void);
		int	Classify(void) { return	CLASS_PLAYER_ALLY; }

		void KeyValue(KeyValueData *pkvd);

		int		m_iHead;
		int	m_iPose;// which sequence to display
		static char *m_szPoses[7];
};

#endif // MONSTER_HGRUNT_OPFOR_H