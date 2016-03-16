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

		static const char *pAttackSounds[];
		static const char *pIdleSounds[];
		static const char *pAlertSounds[];
		static const char *pPainSounds[];
		static const char *pAttackHitSounds[];
		static const char *pAttackMissSounds[];

		// No range attacks
		BOOL CheckRangeAttack1(float flDot, float flDist) { return FALSE; }
		BOOL CheckRangeAttack2(float flDot, float flDist) { return FALSE; }
		int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);

		void SetActivity(Activity NewActivity);
};

#endif // MONSTER_DIABLO_H