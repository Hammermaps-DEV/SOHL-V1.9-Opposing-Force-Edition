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

#ifndef MONSTER_PITWORM_H
#define MONSTER_PITWORM_H

#define NUM_PITWORM_LEVELS		4

class CPitWorm : public CBaseMonster
{
public:
	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	virtual int	ObjectCaps(void) { return CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	void Spawn(void);
	void Precache(void);
	int  Classify(void);

	void IdleSound(void);
	void AlertSound(void);
	void DeathSound(void);

	void AngrySound(void);
	void FlinchSound(void);
	void SwipeSound(void);
	void BeamSound(void);

	void SetObjectCollisionBox(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);

	int CanPlaySequence(BOOL fDisregardState);

	void EXPORT StartupThink(void);
	void EXPORT DyingThink(void);
	void EXPORT StartupUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT NullThink(void);
	void EXPORT CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT	HuntThink(void);

	void EXPORT WormTouch(CBaseEntity* pOther);

	void FloatSequence(void);
	void NextActivity(void);

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

	void UpdateBodyControllers(void);

	void CreateBeam(const Vector& src, const Vector& target, int width);
	void DestroyBeam(void);
	void UpdateBeam(const Vector& src, const Vector& target);

	void SetupBeamPoints(CBaseEntity* pEnemy, Vector* vecleft, Vector* vecRight);
	void UpdateBeamPoints(CBaseEntity* pEnemy, Vector* vecTarget);

	void CreateGlow(void);
	void DestroyGlow(void);
	void EyeOff(void);
	void EyeOn(int level);
	void EyeUpdate(void);

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