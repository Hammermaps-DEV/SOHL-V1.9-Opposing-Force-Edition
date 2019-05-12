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
// NPC: Diablo
//=========================================================

#ifndef MONSTER_DIABLO_H
#define MONSTER_DIABLO_H

class CDiablo : public CBaseMonster {
public:
	void Spawn();
	void Precache();
	void SetYawSpeed();
	int  Classify();
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	int IgnoreConditions();
	int  ISoundMask();

	float m_flNextFlinch;

	void PainSound();
	void AlertSound();
	void IdleSound();
	void AttackSound();

	bool CheckRangeAttack1(float flDot, float flDist);
	bool CheckRangeAttack2(float flDot, float flDist);
	bool CheckMeleeAttack1(float flDot, float flDist);
	bool CheckMeleeAttack2(float flDot, float flDist);

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

	Schedule_t	*GetSchedule();
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