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
// NPC: Scientist Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_ZOMBIE_H
#define MONSTER_ZOMBIE_H

// class definition
class CZombie : public CBaseMonster {
	public:
		// void
		void Spawn(void);
		void Precache(void);
		void HandleAnimEvent(MonsterEvent_t *pEvent);
		void PainSound(void);
		void DeathSound(void);
		void AlertSound(void);
		void IdleSound(void);
		void AttackSound(void);
		void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

		// bool void
		BOOL CheckRangeAttack1(float flDot, float flDist) { return FALSE; }
		BOOL CheckRangeAttack2(float flDot, float flDist) { return FALSE; }

		// int void
		int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
		int IgnoreConditions(void);
		int Classify(void);

		// virtual int
		virtual int	Save(CSave &save);
		virtual int	Restore(CRestore &restore);
		virtual int GetVoicePitch(long random) { return m_flPitch + random; }

		// static
		static TYPEDESCRIPTION m_SaveData[];

	protected:
		static const char *pAttackSounds[];
		static const char *pIdleSounds[];
		static const char *pAlertSounds[];
		static const char *pPainSounds[];
		static const char *pDeathSounds[];
		static const char *pAttackHitSounds[];
		static const char *pAttackMissSounds[];

		// vars
		int m_flPitch = 100;
		float m_flBulletDR = 0.0;
		float m_flNextFlinch;

		float m_flHitgroupHead = gSkillData.zombieHead;
		float m_flHitgroupChest = gSkillData.zombieChest;
		float m_flHitgroupStomach = gSkillData.zombieStomach;
		float m_flHitgroupArm = gSkillData.zombieArm;
		float m_flHitgroupLeg = gSkillData.zombieLeg;
		float m_flDmgOneSlash = gSkillData.zombieDmgOneSlash;
		float m_flDmgBothSlash = gSkillData.zombieDmgBothSlash;
};

#endif // MONSTER_ZOMBIE_H