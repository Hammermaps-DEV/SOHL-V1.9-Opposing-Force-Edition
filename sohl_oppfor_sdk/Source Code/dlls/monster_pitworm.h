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

#ifndef MONSTER_PITWORM_H
#define MONSTER_PITWORM_H

#define NUM_PITWORM_LEVELS		4

class CPitWorm : public CBaseMonster
{
public:
	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	virtual int	ObjectCaps() { return CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	void Spawn();
	void Precache();
	int  Classify();

	void IdleSound();
	void AlertSound();
	void DeathSound();

	void AngrySound();
	void FlinchSound();
	void SwipeSound();
	void BeamSound();

	void SetObjectCollisionBox();
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);

	int CanPlaySequence(BOOL fDisregardState);

	void EXPORT StartupThink();
	void EXPORT DyingThink();
	void EXPORT StartupUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT NullThink();
	void EXPORT CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT	HuntThink();

	void EXPORT WormTouch(CBaseEntity* pOther);

	void FloatSequence();
	void NextActivity();

	Vector m_avelocity;

	Vector m_vecTarget;
	Vector m_posTarget;

	Vector m_vecDesired;
	Vector m_posDesired;

	float  m_flMinZ;
	float  m_flMaxZ;

	float m_flLastSeen;
	float m_flPrevSeen;

	int m_iLevel;

	float m_flAdj;
	float m_distDesired;

	int Level(float dz);

	const Vector& IdealPosition(const int level) const;
	const Vector& IdealPosition(const float dz) const;
	const Vector& IdealPosition(CBaseEntity* pEnemy) const;

	void UpdateBodyControllers();

	void CreateBeam(const Vector& src, const Vector& target, int width);
	void DestroyBeam();
	void UpdateBeam(const Vector& src, const Vector& target);

	void SetupBeamPoints(CBaseEntity* pEnemy, Vector* vecleft, Vector* vecRight);
	void UpdateBeamPoints(CBaseEntity* pEnemy, Vector* vecTarget);

	void CreateGlow();
	void DestroyGlow();
	void EyeOff();
	void EyeOn(int level);
	void EyeUpdate();

	float m_flInitialYaw;
	Vector m_spawnAngles;
	Vector m_vecLevels[NUM_PITWORM_LEVELS];

	CBeam* m_pBeam;
	float m_flBeamYaw;
	float m_flBeamTime;
	BOOL m_fBeamOn;
	CSprite* m_pEyeGlow;
	int m_eyeBrightness;

	Vector m_vecGoalAngles;
	Vector m_vecCurAngles;

	float m_flNextAttackTime;
	float m_flNextIdleSoundTime;

	static const char* pHitSilo[];
	static const char* pClangSounds[];
	static const char* pAngrySounds[];
	static const char* pSwipeSounds[];
	static const char* pFlinchSounds[];

	static const char* pAlertSounds[];
	static const char* pIdleSounds[];
	static const char* pDeathSounds[];
};

#endif // MONSTER_PITWORM_H