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

#ifndef MONSTER_TENTACLE_H
#define MONSTER_TENTACLE_H

class CTentacle : public CBaseMonster
{
public:
	CTentacle(void);

	void Spawn();
	void Precache();
	void KeyValue(KeyValueData *pkvd);

	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	// Don't allow the tentacle to go across transitions!!!
	virtual int	ObjectCaps(void) { return CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	void SetObjectCollisionBox(void)
	{
		pev->absmin = pev->origin + Vector(-400, -400, 0);
		pev->absmax = pev->origin + Vector(400, 400, 850);
	}

	void EXPORT Cycle(void);
	void EXPORT CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT Start(void);
	void EXPORT DieThink(void);

	void EXPORT HitTouch(CBaseEntity *pOther);

	float HearingSensitivity(void) { return 2.0; };

	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void Killed(entvars_t *pevAttacker, int iGib);

	MONSTERSTATE GetIdealState(void) { return MONSTERSTATE_IDLE; };
	int CanPlaySequence(BOOL fDisregardState) { return TRUE; };

	int Classify(void);

	int Level(float dz);
	int MyLevel(void);
	float MyHeight(void);

	float m_flInitialYaw;
	int m_iGoalAnim;
	int m_iLevel;
	int m_iDir;
	float m_flFramerateAdj;
	float m_flSoundYaw;
	int m_iSoundLevel;
	float m_flSoundTime;
	float m_flSoundRadius;
	int m_iHitDmg;
	float m_flHitTime;

	float m_flTapRadius;

	float m_flNextSong;
	static int g_fFlySound;
	static int g_fSquirmSound;

	float m_flMaxYaw;
	int m_iTapSound;

	Vector m_vecPrevSound;
	float m_flPrevSoundTime;

	static const char *pHitSilo[];
	static const char *pHitDirt[];
	static const char *pHitWater[];
};

class CTentacleMaw : public CBaseMonster
{
public:
	void Spawn();
	void Precache();
};

#endif // MONSTER_TENTACLE_H