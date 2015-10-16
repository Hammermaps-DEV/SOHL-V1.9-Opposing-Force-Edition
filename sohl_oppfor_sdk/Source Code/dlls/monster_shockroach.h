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
#ifndef MONSTER_SHOCKROACH_H
#define MONSTER_SHOCKROACH_H

#include "monster_headcrab.h"

class CShockRoach : public CHeadCrab
{
	public:
		void Spawn(void);
		void Precache(void);
		void EXPORT LeapTouch(CBaseEntity *pOther);
		void PainSound(void);
		void DeathSound(void);
		void IdleSound(void);
		void AlertSound(void);
		void PrescheduleThink(void);
		void StartTask(Task_t* pTask);
		void KeyValue(KeyValueData *pkvd);
		void HandleAnimEvent(MonsterEvent_t *pEvent);

		int Classify(void);

		virtual int	Save(CSave &save);
		virtual int	Restore(CRestore &restore);

		static	TYPEDESCRIPTION m_SaveData[];

		static const char *pIdleSounds[];
		static const char *pAttackSounds[];

		float m_flDie;
		bool m_iDies = true;
		bool m_iWeapon = true;
};

#endif // MONSTER_SHOCKROACH_H