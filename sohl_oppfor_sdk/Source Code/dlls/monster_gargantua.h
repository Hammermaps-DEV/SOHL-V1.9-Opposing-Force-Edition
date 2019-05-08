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

#ifndef MONSTER_GARGANTUA_H
#define MONSTER_GARGANTUA_H

class CSpiral : public CBaseEntity
{
public:
	void Spawn(void);
	void Think(void);
	int ObjectCaps(void) { return FCAP_DONT_SAVE; }
	static CSpiral *Create(const Vector &origin, float height, float radius, float duration);
};

class CStomp : public CBaseEntity
{
public:
	void Spawn(void);
	void Think(void);
	static CStomp *StompCreate(const Vector &origin, const Vector &end, float speed);

private:
	// UNDONE: re-use this sprite list instead of creating new ones all the time
	//	CSprite		*m_pSprites[ STOMP_SPRITE_COUNT ];
};

class CGargantua : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	void SetYawSpeed(void);
	int  Classify(void);
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void HandleAnimEvent(MonsterEvent_t *pEvent);

	BOOL CheckMeleeAttack1(float flDot, float flDist);		// Swipe
	BOOL CheckMeleeAttack2(float flDot, float flDist);		// Flames
	BOOL CheckRangeAttack1(float flDot, float flDist);		// Stomp attack
	void SetObjectCollisionBox(void)
	{
		pev->absmin = pev->origin + Vector(-80, -80, 0);
		pev->absmax = pev->origin + Vector(80, 80, 214);
	}

	Schedule_t *GetScheduleOfType(int Type);
	void StartTask(Task_t *pTask);
	void RunTask(Task_t *pTask);

	void PrescheduleThink(void);

	void Killed(entvars_t *pevAttacker, int iGib);
	void DeathEffect(void);

	void EyeOff(void);
	void EyeOn(int level);
	void EyeUpdate(void);
	void Leap(void);
	void StompAttack(void);
	void FlameCreate(void);
	void FlameUpdate(void);
	void FlameControls(float angleX, float angleY);
	void FlameDestroy(void);
	inline BOOL FlameIsOn(void) { return m_pFlame[0] != NULL; }

	void FlameDamage(Vector vecStart, Vector vecEnd, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType);

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	CUSTOM_SCHEDULES;

private:
	static const char *pAttackHitSounds[];
	static const char *pBeamAttackSounds[];
	static const char *pAttackMissSounds[];
	static const char *pRicSounds[];
	static const char *pFootSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pAttackSounds[];
	static const char *pStompSounds[];
	static const char *pBreatheSounds[];

	CBaseEntity* GargantuaCheckTraceHullAttack(float flDist, int iDamage, int iDmgType);

	CSprite		*m_pEyeGlow;		// Glow around the eyes
	CBeam		*m_pFlame[4];		// Flame beams

	int			m_eyeBrightness;	// Brightness target
	float		m_seeTime;			// Time to attack (when I see the enemy, I set this)
	float		m_flameTime;		// Time of next flame attack
	float		m_painSoundTime;	// Time of next pain sound
	float		m_streakTime;		// streak timer (don't send too many)
	float		m_flameX;			// Flame thrower aim
	float		m_flameY;
};

#endif // MONSTER_GARGANTUA_H