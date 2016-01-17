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

// class definition
class CBarney : public CTalkMonster {
	public:
		void Spawn(void);
		void Precache(void);
		void SetYawSpeed(void);
		int  ISoundMask(void);
		void Fire9mmPistol(void);
		void AlertSound(void);
		int  Classify(void);
		void HandleAnimEvent(MonsterEvent_t *pEvent);

		void RunTask(Task_t *pTask);
		void StartTask(Task_t *pTask);
		virtual int	ObjectCaps(void) { return CTalkMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
		int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
		BOOL CheckRangeAttack1(float flDot, float flDist);

		void DeclineFollowing(void);

		Schedule_t *GetScheduleOfType(int Type);
		Schedule_t *GetSchedule(void);
		MONSTERSTATE GetIdealState(void);

		void DeathSound(void);
		void PainSound(void);

		void TalkInit(void);

		void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
		void Killed(entvars_t *pevAttacker, int iGib);

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