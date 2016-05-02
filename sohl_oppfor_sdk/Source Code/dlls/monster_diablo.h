/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Spirit of Half-Life and their logos are the property of their respective owners.
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
*   All Rights Reserved.
*
*	Base Source-Code written by Marc-Antoine Lortie (https://github.com/malortie).
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/
//=========================================================
// NPC: Diablo
//=========================================================

#ifndef MONSTER_DIABLO_H
#define MONSTER_DIABLO_H

class CDiablo : public CBaseMonster {
	public:
		void Spawn(void);
		void Precache(void);
		void SetYawSpeed(void);
		int  Classify(void);
		void HandleAnimEvent(MonsterEvent_t *pEvent);
		int IgnoreConditions(void);
		int  ISoundMask(void);

		float m_flNextFlinch;

		void PainSound(void);
		void AlertSound(void);
		void IdleSound(void);
		void AttackSound(void);

		BOOL CheckRangeAttack1(float flDot, float flDist);
		BOOL CheckRangeAttack2(float flDot, float flDist);
		BOOL CheckMeleeAttack1(float flDot, float flDist);
		BOOL CheckMeleeAttack2(float flDot, float flDist);

		static const char *pAttackSounds[];
		static const char *pStepSounds[];
		static const char *pIdleSounds[];
		static const char *pAlertSounds[];
		static const char *pPainSounds[];
		static const char *pAttackHitSounds[];
		static const char *pAttackMissSounds[];
		static const char *pAttackHitSoundsEXT[];
		static const char *pAttackMissSoundsEXT[];

		int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
		void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

		void SetActivity(Activity NewActivity);

		Schedule_t	*GetSchedule(void);
		Schedule_t  *GetScheduleOfType(int Type);

		CUSTOM_SCHEDULES;

	protected:
		BOOL m_flDebug = false;
		float m_flBulletDR = 0.0;
		float m_flHitgroupHead;
		float m_flHitgroupChest;
		float m_flHitgroupStomach;
		float m_flHitgroupLeg;
};

#endif // MONSTER_DIABLO_H