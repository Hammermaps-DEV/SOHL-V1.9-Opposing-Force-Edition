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

#ifndef MONSTER_NIHILANTH_H
#define MONSTER_NIHILANTH_H

#define N_SCALE		15
#define N_SPHERES	20

class CNihilanth : public CBaseMonster
{
public:
	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn();
	void Precache();
	int  Classify() { return CLASS_ALIEN_MILITARY; };
	int  BloodColor() { return BLOOD_COLOR_YELLOW; }
	void Killed(entvars_t *pevAttacker, int iGib);
	void GibMonster();

	void SetObjectCollisionBox()
	{
		pev->absmin = pev->origin + Vector(-16 * N_SCALE, -16 * N_SCALE, -48 * N_SCALE);
		pev->absmax = pev->origin + Vector(16 * N_SCALE, 16 * N_SCALE, 28 * N_SCALE);
	}

	void HandleAnimEvent(MonsterEvent_t *pEvent);

	void DLLEXPORT StartupThink();
	void DLLEXPORT HuntThink();
	void DLLEXPORT CrashTouch(CBaseEntity *pOther);
	void DLLEXPORT DyingThink();
	void DLLEXPORT StartupUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void DLLEXPORT NullThink();
	void DLLEXPORT CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void FloatSequence();
	void NextActivity();

	void Flight();

	BOOL AbsorbSphere();
	BOOL EmitSphere();
	void TargetSphere(USE_TYPE useType, float value);
	CBaseEntity *RandomTargetname(const char *szName);
	void ShootBalls();
	void MakeFriend(Vector vecPos);

	int  TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	void PainSound();
	void DeathSound();

	static const char *pAttackSounds[];	// vocalization: play sometimes when he launches an attack
	static const char *pBallSounds[];	// the sound of the lightening ball launch
	static const char *pShootSounds[];	// grunting vocalization: play sometimes when he launches an attack
	static const char *pRechargeSounds[];	// vocalization: play when he recharges
	static const char *pLaughSounds[];	// vocalization: play sometimes when hit and still has lots of health
	static const char *pPainSounds[];	// vocalization: play sometimes when hit and has much less health and no more chargers
	static const char *pDeathSounds[];	// vocalization: play as he dies

										// x_teleattack1.wav	the looping sound of the teleport attack ball.

	float m_flForce;

	float m_flNextPainSound;

	Vector m_velocity;
	Vector m_avelocity;

	Vector m_vecTarget;
	Vector m_posTarget;

	Vector m_vecDesired;
	Vector m_posDesired;

	float  m_flMinZ;
	float  m_flMaxZ;

	Vector m_vecGoal;

	float m_flLastSeen;
	float m_flPrevSeen;

	int m_irritation;

	int m_iLevel;
	int m_iTeleport;

	EHANDLE m_hRecharger;

	EHANDLE m_hSphere[N_SPHERES];
	int	m_iActiveSpheres;

	float m_flAdj;

	CSprite *m_pBall;

	char m_szRechargerTarget[64];
	char m_szDrawUse[64];
	char m_szTeleportUse[64];
	char m_szTeleportTouch[64];
	char m_szDeadUse[64];
	char m_szDeadTouch[64];

	float m_flShootEnd;
	float m_flShootTime;

	EHANDLE m_hFriend[3];
};

class CNihilanthHVR : public CBaseMonster
{
public:
	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn();
	void Precache();

	void CircleInit(CBaseEntity *pTarget);
	void AbsorbInit();
	void TeleportInit(CNihilanth *pOwner, CBaseEntity *pEnemy, CBaseEntity *pTarget, CBaseEntity *pTouch);
	void GreenBallInit();
	void ZapInit(CBaseEntity *pEnemy);

	void DLLEXPORT HoverThink();
	BOOL CircleTarget(Vector vecTarget);
	void DLLEXPORT DissipateThink();

	void DLLEXPORT ZapThink();
	void DLLEXPORT TeleportThink();
	void DLLEXPORT TeleportTouch(CBaseEntity *pOther);

	void DLLEXPORT RemoveTouch(CBaseEntity *pOther);
	void DLLEXPORT BounceTouch(CBaseEntity *pOther);
	void DLLEXPORT ZapTouch(CBaseEntity *pOther);

	CBaseEntity *RandomClassname(const char *szName);

	// void DLLEXPORT SphereUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void MovetoTarget(Vector vecTarget);
	virtual void Crawl();

	void Zap();
	void Teleport();

	float m_flIdealVel;
	Vector m_vecIdeal;
	CNihilanth *m_pNihilanth;
	EHANDLE m_hTouch;
	int m_nFrames;
};

#endif // MONSTER_NIHILANTH_H