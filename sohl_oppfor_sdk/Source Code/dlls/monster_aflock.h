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

#ifndef MONSTER_AFLOCK_H
#define MONSTER_AFLOCK_H

class CFlockingFlyerFlock : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	void KeyValue(KeyValueData *pkvd);
	void SpawnFlock(void);

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	// Sounds are shared by the flock
	static  void PrecacheFlockSounds(void);

	int		m_cFlockSize;
	float	m_flFlockRadius;
};

class CFlockingFlyer : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	void SpawnCommonCode(void);
	void EXPORT IdleThink(void);
	void BoidAdvanceFrame(void);
	void EXPORT FormFlock(void);
	void EXPORT Start(void);
	void EXPORT FlockLeaderThink(void);
	void EXPORT FlockFollowerThink(void);
	void EXPORT FallHack(void);
	void MakeSound(void);
	void AlertFlock(void);
	void SpreadFlock(void);
	void SpreadFlock2(void);
	void Killed(entvars_t *pevAttacker, int iGib);
	void Poop(void);
	BOOL FPathBlocked(void);
	//void KeyValue( KeyValueData *pkvd );

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int IsLeader(void) { return m_pSquadLeader == this; }
	int	InSquad(void) { return m_pSquadLeader != NULL; }
	int	SquadCount(void);
	void SquadRemove(CFlockingFlyer *pRemove);
	void SquadUnlink(void);
	void SquadAdd(CFlockingFlyer *pAdd);
	void SquadDisband(void);

	CFlockingFlyer *m_pSquadLeader;
	CFlockingFlyer *m_pSquadNext;
	BOOL	m_fTurning;// is this boid turning?
	BOOL	m_fCourseAdjust;// followers set this flag TRUE to override flocking while they avoid something
	BOOL	m_fPathBlocked;// TRUE if there is an obstacle ahead
	Vector	m_vecReferencePoint;// last place we saw leader
	Vector	m_vecAdjustedVelocity;// adjusted velocity (used when fCourseAdjust is TRUE)
	float	m_flGoalSpeed;
	float	m_flLastBlockedTime;
	float	m_flFakeBlockedTime;
	float	m_flAlertTime;
	float	m_flFlockNextSoundTime;
};

#endif // MONSTER_AFLOCK_H