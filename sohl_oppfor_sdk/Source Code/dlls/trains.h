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
#ifndef TRAINS_H
#define TRAINS_H

// Tracktrain spawn flags
#define SF_TRACKTRAIN_NOPITCH		0x0001
#define SF_TRACKTRAIN_NOCONTROL		0x0002
#define SF_TRACKTRAIN_FORWARDONLY	0x0004
#define SF_TRACKTRAIN_PASSABLE		0x0008
#define SF_TRACKTRAIN_NOYAW			0x0010		//LRC
#define SF_TRACKTRAIN_AVELOCITY		0x800000	//LRC - avelocity has been set manually, don't turn.
#define SF_TRACKTRAIN_AVEL_GEARS	0x400000	//LRC - avelocity should be scaled up/down when the train changes gear.

// Spawnflag for CPathTrack
#define SF_PATH_DISABLED		0x00000001
#define SF_PATH_FIREONCE		0x00000002
#define SF_PATH_ALTREVERSE		0x00000004
#define SF_PATH_DISABLE_TRAIN	0x00000008
#define SF_PATH_ALTERNATE		0x00008000
#define SF_PATH_AVELOCITY		0x00080000 //LRC

// Spawnflags of CPathCorner
#define SF_CORNER_WAITFORTRIG	0x001
#define SF_CORNER_TELEPORT		0x002
#define SF_CORNER_FIREONCE		0x004
#define SF_CORNER_AVELOCITY     0x800000

//LRC - values in 'armortype'
#define PATHSPEED_SET			0
#define PATHSPEED_ACCEL			1
#define PATHSPEED_TIME			2
#define PATHSPEED_SET_MASTER	3

//LRC - values in 'frags'
#define PATHTURN_SET			0
#define PATHTURN_SET_MASTER		1
#define PATHTURN_RESET			2

//LRC - values in 'armorvalue'
#define PATHMATCH_NO			0
#define PATHMATCH_YES			1
#define PATHMATCH_TRACK			2

//#define PATH_SPARKLE_DEBUG		1	// This makes a particle effect around path_track entities for debugging
class CPathTrack : public CPointEntity
{
public:
	void		Spawn();
	void		Activate();
	void		KeyValue(KeyValueData* pkvd);

	void		SetPrevious(CPathTrack *pprevious);
	void		Link();
	void		Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	CPathTrack	*ValidPath(CPathTrack *ppath, int testFlag);		// Returns ppath if enabled, NULL otherwise
	void		Project(CPathTrack *pstart, CPathTrack *pend, Vector *origin, float dist);

	static CPathTrack *Instance(edict_t *pent);

	CPathTrack	*LookAhead(Vector *origin, float dist, int move);
	CPathTrack	*Nearest(Vector origin);

	CPathTrack	*GetNext();
	CPathTrack	*GetPrevious();

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);

	static	TYPEDESCRIPTION m_SaveData[];
#if PATH_SPARKLE_DEBUG
	void EXPORT Sparkle();
#endif

	float		m_length;
	string_t	m_altName;
	CPathTrack	*m_pnext;
	CPathTrack	*m_pprevious;
	CPathTrack	*m_paltpath;
};

class CTrainSequence;
class CBaseTrainDoor;

class CFuncTrackTrain : public CBaseEntity
{
public:
	void Spawn();
	void Precache();

	void Blocked(CBaseEntity *pOther);
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void KeyValue(KeyValueData* pkvd);

	//LRC
	void StartSequence(CTrainSequence *pSequence);
	void StopSequence();
	CTrainSequence *m_pSequence;

	void DesiredAction(); //LRC - used to be called Next!

//	void EXPORT Next( void );
	void EXPORT PostponeNext();
	void EXPORT Find();
	void EXPORT NearestPath();
	void EXPORT DeadEnd();

	void		NextThink(float thinkTime, BOOL alwaysThink);

	void SetTrack(CPathTrack *track) { m_ppath = track->Nearest(pev->origin); }
	void SetControls(entvars_t *pevControls);
	BOOL OnControls(entvars_t *pev);

	void StopSound();
	void UpdateSound();

	static CFuncTrackTrain *Instance(edict_t *pent);

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);

	static	TYPEDESCRIPTION m_SaveData[];
	virtual int	ObjectCaps() { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DIRECTIONAL_USE; }

	virtual void	OverrideReset();
	virtual void	ClearPointers();

	CPathTrack	*m_ppath;
	CBaseTrainDoor	*m_pDoor;
	float		m_length;
	float		m_height;
	// I get it... this records the train's max speed (as set by the level designer), whereas
	// pev->speed records the current speed (as set by the player). --LRC
	// m_speed is also stored, as an int, in pev->impulse.
	float		m_speed;
	float		m_dir;
	float		m_startSpeed;
	Vector		m_controlMins;
	Vector		m_controlMaxs;
	int			m_soundPlaying;
	int			m_sounds;
	float		m_flVolume;
	float		m_flBank;
	float		m_oldSpeed;
	Vector		m_vecMasterAvel; //LRC - masterAvel is to avelocity as m_speed is to speed.
	Vector		m_vecBaseAvel; // LRC - the underlying avelocity, superceded by normal turning behaviour where applicable

	EHANDLE m_hActivator;	//AJH (give frags to this entity)
public:
	void SetTrainDoor(CBaseTrainDoor *pDoor);
private:
	unsigned short m_usAdjustPitch;
};

typedef enum
{
	TD_CLOSED,
	TD_SHIFT_UP,
	TD_SLIDING_UP,
	TD_OPENED,
	TD_SLIDING_DOWN,
	TD_SHIFT_DOWN
} TRAINDOOR_STATE;

class CBaseTrainDoor : public CBaseToggle
{
public:
	void Spawn();
	void Precache();
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Blocked(CBaseEntity *pOther);

	virtual int	ObjectCaps()
	{
		return (CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION);
	};
	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);

	static	TYPEDESCRIPTION m_SaveData[];

	// local functions
	void EXPORT FindTrain();
	void EXPORT DoorGoUp();
	void EXPORT DoorGoDown();
	void EXPORT DoorHitTop();
	void EXPORT DoorSlideUp();
	void EXPORT DoorSlideDown();
	void EXPORT DoorSlideWait();		// wait before sliding
	void EXPORT DoorHitBottom();
	void EXPORT ActivateTrain();

	BYTE	m_bMoveSnd;			// sound a door makes while moving
	BYTE	m_bStopSnd;			// sound a door makes when it stops

	Vector	ConvertAngles();	// same as in plats.cpp

	CFuncTrackTrain	*m_pTrain;	// my train pointer

	float	m_flBlockedTime;
	Vector	m_vecOldAngles;
	Vector	m_vecPosition3;		// moving forward

	TRAINDOOR_STATE	door_state;
	virtual void	OverrideReset();
	virtual void	Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual STATE	GetState();
	void		DoorSetup();
	void		Evaluate();
	void		Stop();
};

#endif
