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