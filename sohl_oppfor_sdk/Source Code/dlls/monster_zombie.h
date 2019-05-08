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
// NPC: Scientist Zombie * http://half-life.wikia.com/wiki/Standard_Zombie
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_ZOMBIE_H
#define MONSTER_ZOMBIE_H

// class definition
class CZombie : public CBaseMonster {
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void HandleAnimEvent(MonsterEvent_t *pEvent);
	virtual void PainSound(void);
	virtual void DeathSound(void);
	virtual void AlertSound(void);
	virtual void IdleSound(void);
	virtual void AttackSound(void);
	virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	virtual BOOL CheckRangeAttack1(float flDot, float flDist) { return FALSE; }
	virtual BOOL CheckRangeAttack2(float flDot, float flDist) { return FALSE; }

	virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual int IgnoreConditions(void);
	virtual int Classify(void);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	static const char *pAttackSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pDeathSounds[];
	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];

protected:
	BOOL m_flDebug = false;
	float m_flBulletDR = 0.0;
	float m_flNextFlinch;
	float m_flHitgroupHead;
	float m_flHitgroupChest;
	float m_flHitgroupStomach;
	float m_flHitgroupArm;
	float m_flHitgroupLeg;
	float m_flDmgOneSlash;
	float m_flDmgBothSlash;
};

#endif // MONSTER_ZOMBIE_H