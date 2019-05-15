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

#ifndef MONSTER_OSPREY_H
#define MONSTER_OSPREY_H

#define MAX_CARRY	24

class COsprey : public CBaseMonster
{
public:
	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];
	int		ObjectCaps() { return CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	void Spawn();
	void Precache();
	int  Classify() { return CLASS_MACHINE; };
	int  BloodColor() { return DONT_BLEED; }
	void Killed(entvars_t *pevAttacker, int iGib);

	void UpdateGoal();
	BOOL HasDead();
	void EXPORT FlyThink();
	void EXPORT DeployThink();
	void Flight();
	void EXPORT HitTouch(CBaseEntity *pOther);
	void EXPORT FindAllThink();
	void EXPORT HoverThink();
	CBaseMonster *MakeGrunt(Vector vecSrc);
	void EXPORT CrashTouch(CBaseEntity *pOther);
	void EXPORT DyingThink();
	void EXPORT CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	// int  TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void ShowDamage();

	CBaseEntity *m_pGoalEnt;
	Vector m_vel1;
	Vector m_vel2;
	Vector m_pos1;
	Vector m_pos2;
	Vector m_ang1;
	Vector m_ang2;
	float m_startTime;
	float m_dTime;

	Vector m_velocity;

	float m_flIdealtilt;
	float m_flRotortilt;

	float m_flRightHealth;
	float m_flLeftHealth;

	int	m_iUnits;
	EHANDLE m_hGrunt[MAX_CARRY];
	Vector m_vecOrigin[MAX_CARRY];
	EHANDLE m_hRepel[4];

	int m_iSoundState;
	int m_iSpriteTexture;

	int m_iPitch;

	int m_iExplode;
	int	m_iTailGibs;
	int	m_iBodyGibs;
	int	m_iEngineGibs;

	int m_iDoLeftSmokePuff;
	int m_iDoRightSmokePuff;
};

#endif // MONSTER_OSPREY_H