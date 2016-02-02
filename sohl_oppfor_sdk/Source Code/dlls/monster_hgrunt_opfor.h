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
		int  ISoundMask(void);
		int  Classify(void);
		void HandleAnimEvent(MonsterEvent_t *pEvent);
		void CheckAmmo(void);
		void SetActivity(Activity NewActivity);
		void RunTask(Task_t *pTask);
		void StartTask(Task_t *pTask);
		void KeyValue(KeyValueData *pkvd);
		virtual int	ObjectCaps(void) { return CRCAllyMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
		BOOL FCanCheckAttacks(void);
		BOOL CheckRangeAttack1(float flDot, float flDist);
		BOOL CheckRangeAttack2(float flDot, float flDist);
		BOOL CheckMeleeAttack1(float flDot, float flDist);
		void DeclineFollowing(void);
		void PrescheduleThink(void);
		Vector GetGunPosition(void);

		// Shoots
		void Shoot(void);
		void Shotgun(void);
		void M249(void);

		// Override these to set behavior
		CBaseEntity	*Kick(void);
		Schedule_t *GetScheduleOfType(int Type);
		Schedule_t *GetSchedule(void);
		MONSTERSTATE GetIdealState(void);

		void DeathSound(void);
		void PainSound(void);
		void GibMonster(void);
		void TalkInit(void);

		BOOL FOkToSpeak(void);
		void JustSpoke(void);

		void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
		int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
		void Killed(entvars_t *pevAttacker, int iGib);
		int IRelationship(CBaseEntity *pTarget);

		virtual int		Save(CSave &save);
		virtual int		Restore(CRestore &restore);
		static	TYPEDESCRIPTION m_SaveData[];

		// UNDONE: What is this for?  It isn't used?
		float	m_flPlayerDamage;// how much pain has the player inflicted on me?

								 // checking the feasibility of a grenade toss is kind of costly, so we do it every couple of seconds,
								 // not every server frame.
		float m_flNextGrenadeCheck;
		float m_flNextPainTime;
		float m_flLastEnemySightTime;
		float m_flMedicWaitTime;

		float	m_flLinkToggle;// how much pain has the player inflicted on me?

		Vector	m_vecTossVelocity;

		BOOL	m_fThrowGrenade;
		BOOL	m_fStanding;
		BOOL	m_fFirstEncounter;// only put on the handsign show in the squad's first encounter.
		BOOL	m_fImmortal;
		int		m_cClipSize;

		int		m_iBrassShell;
		int		m_iShotgunShell;

		int		m_iSentence;
		int		m_iHead;

		int		m_iM249Shell;
		int		m_iM249Link;

		static const char *pGruntSentences[];
		static const char *pPainSounds[];
		static const char *pDeathSounds[];
		static const char *pAttackSoundsSAW[];
		static const char *pAttackSounds9MM[];

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