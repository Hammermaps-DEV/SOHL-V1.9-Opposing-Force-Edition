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
// NPC: Barney * http://half-life.wikia.com/wiki/Barney_Calhoun
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_BARNEY_H
#define MONSTER_BARNEY_H

// include
#include "talkmonster.h"

// class definition
class CBarney : public CTalkMonster {
	public:
		virtual void Spawn(void);
		virtual void Precache(void);
		virtual void SetYawSpeed(void);
		virtual int  ISoundMask(void);
		virtual void Fire9mmPistol(void);
		virtual void AlertSound(void);
		virtual int  Classify(void);
		virtual void HandleAnimEvent(MonsterEvent_t *pEvent);

		virtual void RunTask(Task_t *pTask);
		virtual void StartTask(Task_t *pTask);
		virtual int	ObjectCaps(void) { return CTalkMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
		virtual int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
		virtual BOOL CheckRangeAttack1(float flDot, float flDist);

		virtual void DeclineFollowing(void);

		virtual Schedule_t *GetScheduleOfType(int Type);
		virtual Schedule_t *GetSchedule(void);
		virtual MONSTERSTATE GetIdealState(void);

		virtual void DeathSound(void);
		virtual void PainSound(void);

		virtual void TalkInit(void);

		virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
		virtual void Killed(entvars_t *pevAttacker, int iGib);

		virtual int	Save(CSave &save);
		virtual int	Restore(CRestore &restore);

		static TYPEDESCRIPTION m_SaveData[];

		static const char *pPainSounds[];
		static const char *pDeathSounds[];
		static const char *pAttackSounds[];

		int		m_iBaseBody;
		BOOL	m_fGunDrawn;
		float	m_painTime;
		float	m_checkAttackTime;
		BOOL	m_lastAttackCheck;
		float	m_flPlayerDamage;

		CUSTOM_SCHEDULES;
	protected:
		BOOL m_flDebug = false;
		float m_flHitgroupHead;
		float m_flHitgroupChest;
		float m_flHitgroupStomach;
		float m_flHitgroupArm;
		float m_flHitgroupLeg;
};

#endif // MONSTER_BARNEY_H