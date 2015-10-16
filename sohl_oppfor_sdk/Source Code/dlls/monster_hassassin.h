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

#ifndef MONSTER_HASSASSIN_H
#define MONSTER_HASSASSIN_H

class CHAssassin : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	void SetYawSpeed(void);
	int  Classify(void);
	int  ISoundMask(void);
	void Shoot(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	Schedule_t* GetSchedule(void);
	Schedule_t* GetScheduleOfType(int Type);
	BOOL CheckMeleeAttack1(float flDot, float flDist);	// jump
														// BOOL CheckMeleeAttack2 ( float flDot, float flDist );
	BOOL CheckRangeAttack1(float flDot, float flDist);	// shoot
	BOOL CheckRangeAttack2(float flDot, float flDist);	// throw grenade
	void StartTask(Task_t *pTask);
	void RunAI(void);
	void RunTask(Task_t *pTask);
	void DeathSound(void);
	void IdleSound(void);
	CUSTOM_SCHEDULES;

	int	Save(CSave &save);
	int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	float m_flLastShot;
	float m_flDiviation;

	float m_flNextJump;
	Vector m_vecJumpVelocity;

	float m_flNextGrenadeCheck;
	Vector	m_vecTossVelocity;
	BOOL	m_fThrowGrenade;

	int		m_iTargetRanderamt;

	int		m_iFrustration;

	int		m_iShell;
};

#endif // MONSTER_HASSASSIN_H