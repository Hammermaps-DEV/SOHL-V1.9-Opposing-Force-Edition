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
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/

#ifndef MONSTER_HEADCRAB_H
#define MONSTER_HEADCRAB_H

class CHeadCrab : public CBaseMonster {
	public:
		void Spawn(void);
		void Precache(void);
		void RunTask(Task_t *pTask);
		void StartTask(Task_t *pTask);
		void SetYawSpeed(void);
		void EXPORT LeapTouch(CBaseEntity *pOther);
		void PainSound(void);
		void DeathSound(void);
		void IdleSound(void);
		void AlertSound(void);
		void PrescheduleThink(void);
		int  Classify(void);
		void HandleAnimEvent(MonsterEvent_t *pEvent);
		BOOL CheckRangeAttack1(float flDot, float flDist);
		BOOL CheckRangeAttack2(float flDot, float flDist) { return FALSE; };
		int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);

		virtual float GetDamageAmount(void) { return gSkillData.headcrabDmgBite; }
		virtual int GetVoicePitch(void) { return 100; }
		virtual float GetSoundVolue(void) { return 1.0; }
		Schedule_t* GetScheduleOfType(int Type);

		CUSTOM_SCHEDULES;

		static const char *pIdleSounds[];
		static const char *pAlertSounds[];
		static const char *pPainSounds[];
		static const char *pAttackSounds[];
		static const char *pDeathSounds[];
		static const char *pBiteSounds[];
};

#endif // MONSTER_HEADCRAB_H