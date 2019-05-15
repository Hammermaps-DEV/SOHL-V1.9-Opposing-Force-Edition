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
#ifndef MONSTERS_H
#include "skill.h"
#define MONSTERS_H

/*

===== monsters.h ========================================================

  Header file for monster-related utility code

*/

// CHECKLOCALMOVE result types 
#define	LOCALMOVE_INVALID					0 // move is not possible
#define LOCALMOVE_INVALID_DONT_TRIANGULATE	1 // move is not possible, don't try to triangulate
#define LOCALMOVE_VALID						2 // move is possible

// Hit Group standards
#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_TANK		8 //Added for Torch

#define HITGROUP_HEAD_HELMET_BN		10 //Added for Otis & Barney
#define HITGROUP_HEAD_HELMET_GT		11 //Added for Torch

// =====================================
// Monster Spawnflags
// =====================================

//FGD - Flag Group 1
#define	SF_MONSTER_SPAWNFLAG_1		    1
#define	SF_MONSTER_SPAWNFLAG_2			2
#define SF_MONSTER_SPAWNFLAG_4			4
#define SF_MONSTER_SPAWNFLAG_8			8
#define SF_MONSTER_SPAWNFLAG_16			16
#define SF_MONSTER_SPAWNFLAG_32			32
#define SF_MONSTER_SPAWNFLAG_64			64
#define SF_MONSTER_SPAWNFLAG_128		128

//FGD - Flag Group 2
#define SF_MONSTER_SPAWNFLAG_256		256
#define SF_MONSTER_SPAWNFLAG_512		512
#define SF_MONSTER_SPAWNFLAG_1024		1024
#define SF_MONSTER_SPAWNFLAG_2048		2048
#define SF_MONSTER_SPAWNFLAG_4096		4096
#define SF_MONSTER_SPAWNFLAG_8192		8192
#define SF_MONSTER_SPAWNFLAG_16384		16384
#define SF_MONSTER_SPAWNFLAG_32768		32768

//FGD - Flag Group 3
#define SF_MONSTER_SPAWNFLAG_65536		65536
#define SF_MONSTER_SPAWNFLAG_131072		131072
#define SF_MONSTER_SPAWNFLAG_262144		262144
#define SF_MONSTER_SPAWNFLAG_524288		524288
#define SF_MONSTER_SPAWNFLAG_1048576	1048576
#define SF_MONSTER_SPAWNFLAG_2097152	2097152
#define SF_MONSTER_SPAWNFLAG_4194304	4194304
#define SF_MONSTER_SPAWNFLAG_8388608	8388608

//LRC - this clashes with 'not in deathmatch'. Replaced with m_iPlayerReact.
//#define SF_MONSTER_INVERT_PLAYERREACT	2048 //LRC- if this monster would usually attack the player, don't attack unless provoked. If you would usually NOT attack the player, attack him.
#define SF_MONSTER_FALL_TO_GROUND		0x80000000

// specialty spawnflags
#define SF_MONSTER_TURRET_AUTOACTIVATE	32
#define SF_MONSTER_TURRET_STARTINACTIVE	64
#define SF_MONSTER_WAIT_UNTIL_PROVOKED	64 // don't attack the player unless provoked

// MoveToOrigin stuff
#define		MOVE_START_TURN_DIST	64 // when this far away from moveGoal, start turning to face next goal
#define		MOVE_STUCK_DIST			32 // if a monster can't step this far, it is stuck.

// MoveToOrigin stuff
#define		MOVE_NORMAL				0// normal move in the direction monster is facing
#define		MOVE_STRAFE				1// moves in direction specified, no matter which way monster is facing

// spawn flags 256 and above are already taken by the engine
extern void UTIL_MoveToOrigin(edict_t* pent, const Vector &vecGoal, float flDist, int iMoveType);

Vector VecCheckToss(entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flGravityAdj = 1.0);
Vector VecCheckThrow(entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flSpeed, float flGravityAdj = 1.0);
extern DLL_GLOBAL Vector		g_vecAttackDir;
extern DLL_GLOBAL CONSTANT float g_flMeleeRange;
extern DLL_GLOBAL CONSTANT float g_flMediumRange;
extern DLL_GLOBAL CONSTANT float g_flLongRange;
extern void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype);
extern void ExplodeModel(const Vector &vecOrigin, float speed, int model, int count);
extern void WeaponFlash(const Vector &vecOrigin);

extern int gmsgParticles;//define external message

BOOL FBoxVisible(entvars_t *pevLooker, entvars_t *pevTarget);
BOOL FBoxVisible(entvars_t *pevLooker, entvars_t *pevTarget, Vector &vecTargetOrigin, float flSize = 0.0);

// monster to monster relationship types
#define R_AL	-2 // (ALLY) pals. Good alternative to R_NO when applicable.
#define R_FR	-1// (FEAR)will run
#define	R_NO	0// (NO RELATIONSHIP) disregard
#define R_DL	1// (DISLIKE) will attack
#define R_HT	2// (HATE)will attack this character instead of any visible DISLIKEd characters
#define R_NM	3// (NEMESIS)  A monster Will ALWAYS attack its nemsis, no matter what


// these bits represent the monster's memory
#define MEMORY_CLEAR					0
#define bits_MEMORY_PROVOKED			( 1 << 0 )// right now only used for houndeyes.
#define bits_MEMORY_INCOVER				( 1 << 1 )// monster knows it is in a covered position.
#define bits_MEMORY_SUSPICIOUS			( 1 << 2 )// Ally is suspicious of the player, and will move to provoked more easily
#define bits_MEMORY_PATH_FINISHED		( 1 << 3 )// Finished monster path (just used by big momma for now)
#define bits_MEMORY_ON_PATH				( 1 << 4 )// Moving on a path
#define bits_MEMORY_MOVE_FAILED			( 1 << 5 )// Movement has already failed
#define bits_MEMORY_FLINCHED			( 1 << 6 )// Has already flinched
#define bits_MEMORY_KILLED				( 1 << 7 )// HACKHACK -- remember that I've already called my Killed()
#define bits_MEMORY_CUSTOM4				( 1 << 28 )	// Monster-specific memory
#define bits_MEMORY_CUSTOM3				( 1 << 29 )	// Monster-specific memory
#define bits_MEMORY_CUSTOM2				( 1 << 30 )	// Monster-specific memory
#define bits_MEMORY_CUSTOM1				( 1 << 31 )	// Monster-specific memory

// trigger conditions for scripted AI
// these MUST match the CHOICES interface in halflife.fgd for the base monster
enum
{
	AITRIGGER_NONE = 0,
	AITRIGGER_SEEPLAYER_ANGRY_AT_PLAYER,
	AITRIGGER_TAKEDAMAGE,
	AITRIGGER_HALFHEALTH,
	AITRIGGER_DEATH,
	AITRIGGER_SQUADMEMBERDIE,
	AITRIGGER_SQUADLEADERDIE,
	AITRIGGER_HEARWORLD,
	AITRIGGER_HEARPLAYER,
	AITRIGGER_HEARCOMBAT,
	AITRIGGER_SEEPLAYER_UNCONDITIONAL,
	AITRIGGER_SEEPLAYER_NOT_IN_COMBAT,
};
/*
		0 : "No Trigger"
		1 : "See Player"
		2 : "Take Damage"
		3 : "50% Health Remaining"
		4 : "Death"
		5 : "Squad Member Dead"
		6 : "Squad Leader Dead"
		7 : "Hear World"
		8 : "Hear Player"
		9 : "Hear Combat"
*/

struct GibLimit
{
	const int MaxGibs;
};

/**
*	@brief Data used to spawn gibs
*/
struct GibData
{
	const char* const ModelName;
	const int FirstSubModel;
	const int SubModelCount;

	/**
	*	@brief Optional list of limits to apply to each submodel
	*	Must be SubModelCount elements large
	*	If used, instead of randomly selecting a submodel each submodel is used until the requested number of gibs have been spawned
	*/
	const GibLimit* const Limits = nullptr;
};

//
// A gib is a chunk of a body, or a piece of wood/metal/rocks/etc.
//
class CGib : public CBaseEntity
{
public:
	void Spawn(const char *szGibModel);
	void DLLEXPORT BounceGibTouch(CBaseEntity *pOther);
	void DLLEXPORT StickyGibTouch(CBaseEntity *pOther);
	void DLLEXPORT WaitTillLand();
	void		LimitVelocity();

	virtual int	ObjectCaps() { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE; }
	static	void SpawnHeadGib(entvars_t *pevVictim);
	static	void SpawnHeadGib(entvars_t *pevVictim, const char *szGibModel);
	static	void SpawnRandomGibs(entvars_t *pevVictim, int cGibs, const GibData& gibData);
	static	void SpawnRandomGibs(entvars_t *pevVictim, int cGibs, int human);
	static	void SpawnRandomGibs(entvars_t *pevVictim, int cGibs, int notfirst, const char *szGibModel); //LRC
	static  void SpawnStickyGibs(entvars_t *pevVictim, Vector vecOrigin, int cGibs);

	int		m_bloodColor;
	int		m_cBloodDecals;
	int		m_material;
	float	m_lifeTime;
};


#define CUSTOM_SCHEDULES\
		virtual Schedule_t *ScheduleFromName( const char *pName );\
		static Schedule_t *m_scheduleList[];

#define DEFINE_CUSTOM_SCHEDULES(derivedClass)\
	Schedule_t *derivedClass::m_scheduleList[] =

#define IMPLEMENT_CUSTOM_SCHEDULES(derivedClass, baseClass)\
		Schedule_t *derivedClass::ScheduleFromName( const char *pName )\
		{\
			Schedule_t *pSchedule = ScheduleInList( pName, m_scheduleList, HL_ARRAYSIZE(m_scheduleList) );\
			if ( !pSchedule )\
				return baseClass::ScheduleFromName(pName);\
			return pSchedule;\
		}



#endif	//MONSTERS_H
