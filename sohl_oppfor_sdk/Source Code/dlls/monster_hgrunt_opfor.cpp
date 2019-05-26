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
//=========================================================
// NPC: Human Grunt Ally
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"animation.h"
#include	"schedule.h"
#include	"defaultai.h"
#include	"scripted.h"
#include	"weapons.h"
#include	"soundent.h"
#include	"customentity.h"
#include	"proj_grenade.h"
#include	"monster_hgrunt_opfor.h"

//=========================================================
// Monster's define
//=========================================================
#define	HGRUNT_ALLY_AE_RELOAD		( 2 )
#define	HGRUNT_ALLY_AE_KICK			( 3 )
#define	HGRUNT_ALLY_AE_BURST1		( 4 )
#define	HGRUNT_ALLY_AE_BURST2		( 5 ) 
#define	HGRUNT_ALLY_AE_BURST3		( 6 ) 
#define	HGRUNT_ALLY_AE_GREN_TOSS	( 7 )
#define	HGRUNT_ALLY_AE_GREN_LAUNCH	( 8 )
#define	HGRUNT_ALLY_AE_GREN_DROP	( 9 )
#define	HGRUNT_ALLY_AE_CAUGHT_ENEMY	( 10 ) // grunt established sight with an enemy (player only) that had previously eluded the squad.
#define	HGRUNT_ALLY_AE_DROP_GUN		( 11 ) // grunt (probably dead) is dropping his mp5

#define	FGRUNT_CLIP_SIZE_9MM		40
#define	FGRUNT_CLIP_SIZE_M249		70
#define FGRUNT_LIMP_HEALTH			20

#define FGRUNT_MEDIC_WAIT			5

#define FGRUNT_9MMAR				(1 << 0)
#define FGRUNT_HANDGRENADE			(1 << 1)
#define FGRUNT_GRENADELAUNCHER		(1 << 2)
#define FGRUNT_SHOTGUN				(1 << 3)
#define FGRUNT_M249					(1 << 4)

// Torso group for weapons
#define	FG_TORSO_GROUP				2
#define FG_TORSO_DEFAULT			0
#define FG_TORSO_M249				1
#define FG_TORSO_FLAT				2
#define FG_TORSO_SHOTGUN			3

// Weapon group
#define FG_GUN_GROUP				3
#define FG_GUN_MP5					0
#define FG_GUN_SHOTGUN				1
#define FG_GUN_SAW					2
#define FG_GUN_NONE					3

//=========================================================
// Monster's specific conditions
//=========================================================
#define bits_COND_FGRUNT_NOFIRE	( bits_COND_SPECIAL1 )

//=========================================================
// Monster's specific tasks
//=========================================================
enum {
	TASK_HGRUNT_ALLY_FACE_TOSS_DIR = LAST_TALKMONSTER_TASK + 1,
	TASK_HGRUNT_ALLY_CHECK_FIRE,
	TASK_HGRUNT_ALLY_FIND_MEDIC
};

//=========================================================
// Monster heads
//=========================================================
#define FG_HEAD_GROUP				1
#define FG_HEAD_MASK				0
#define FG_HEAD_BERET				1
#define FG_HEAD_SHOTGUN				2
#define FG_HEAD_SAW					3
#define FG_HEAD_SAW_BLACK			4
#define FG_HEAD_MP					5
#define FG_HEAD_MAJOR				6
#define FG_HEAD_BERET_BLACK			7

//=========================================================
// Monster's specific schedule types
//=========================================================
enum {
	SCHED_HGRUNT_ALLY_SUPPRESS = LAST_TALKMONSTER_SCHEDULE + 1,
	SCHED_HGRUNT_ALLY_ESTABLISH_LINE_OF_FIRE,// move to a location to set up an attack against the enemy. (usually when a friendly is in the way).
	SCHED_HGRUNT_ALLY_COVER_AND_RELOAD,
	SCHED_HGRUNT_ALLY_SWEEP,
	SCHED_HGRUNT_ALLY_FOUND_ENEMY,
	SCHED_HGRUNT_ALLY_REPEL,
	SCHED_HGRUNT_ALLY_REPEL_ATTACK,
	SCHED_HGRUNT_ALLY_REPEL_LAND,
	SCHED_HGRUNT_ALLY_WAIT_FACE_ENEMY,
	SCHED_HGRUNT_ALLY_TAKECOVER_FAILED,// special schedule type that forces analysis of conditions and picks the best possible schedule to recover from this type of failure.
	SCHED_HGRUNT_ALLY_ELOF_FAIL,
	SCHED_HGRUNT_ALLY_FIND_MEDIC
};

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS(monster_human_grunt_ally, CHFGrunt);

TYPEDESCRIPTION	CHFGrunt::m_SaveData[] = {
	DEFINE_FIELD(CHFGrunt, m_flNextGrenadeCheck, FIELD_TIME),
	DEFINE_FIELD(CHFGrunt, m_flNextPainTime, FIELD_TIME),
	DEFINE_FIELD(CHFGrunt, m_flMedicWaitTime, FIELD_TIME),
	DEFINE_FIELD(CHFGrunt, m_vecTossVelocity, FIELD_VECTOR),
	DEFINE_FIELD(CHFGrunt, m_fThrowGrenade, FIELD_BOOLEAN),
	DEFINE_FIELD(CHFGrunt, m_fStanding, FIELD_BOOLEAN),
	DEFINE_FIELD(CHFGrunt, m_fFirstEncounter, FIELD_BOOLEAN),
	DEFINE_FIELD(CHFGrunt, m_fImmortal, FIELD_BOOLEAN),
	DEFINE_FIELD(CHFGrunt, m_cClipSize, FIELD_INTEGER),
	DEFINE_FIELD(CHFGrunt, m_iHead, FIELD_INTEGER)
};

IMPLEMENT_SAVERESTORE(CHFGrunt, CRCAllyMonster);

//=========================================================
// Monster's Sentences
//=========================================================
const char *CHFGrunt::pGruntSentences[] = {
	"FG_GREN", // grenade scared grunt
	"FG_ALERT", // sees player
	"FG_MONSTER", // sees monster
	"FG_COVER", // running to cover
	"FG_THROW", // about to throw grenade
	"FG_CHARGE",  // running out to get the enemy
	"FG_TAUNT" // say rude things
};

enum {
	FGRUNT_SENT_NONE = -1,
	FGRUNT_SENT_GREN = 0,
	FGRUNT_SENT_ALERT,
	FGRUNT_SENT_MONSTER,
	FGRUNT_SENT_COVER,
	FGRUNT_SENT_THROW,
	FGRUNT_SENT_CHARGE,
	FGRUNT_SENT_TAUNT
} FGRUNT_SENTENCE_TYPES;

//=========================================================
// KeyValue
// !!! netname entvar field is used in squadmonster for groupname!!!
//=========================================================
void CHFGrunt::KeyValue(KeyValueData *pkvd) {
	if (FStrEq(pkvd->szKeyName, "head")) {
		m_iHead = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "immortal")) {
		m_fImmortal = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else {
		CRCAllyMonster::KeyValue(pkvd);
	}
}

//=========================================================
// Spawn Human Grunt Ally
//=========================================================
void CHFGrunt::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/hgrunt_opfor.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	SetSolidType(SOLID_SLIDEBOX);
	SetMoveType(MOVETYPE_STEP);
	m_bloodColor = BLOOD_COLOR_RED;

	if (pev->health == 0) //LRC
		pev->health = gSkillData.fgruntHealth;

	pev->view_ofs = Vector(0, 0, 50);// position of the eyes relative to monster's origin.
	m_flFieldOfView = VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_MonsterState = MONSTERSTATE_NONE;

	m_flDebug = false; //Debug Massages

	m_flHitgroupHead = gSkillData.fgruntHead;
	m_flHitgroupChest = gSkillData.fgruntChest;
	m_flHitgroupStomach = gSkillData.fgruntStomach;
	m_flHitgroupArm = gSkillData.fgruntArm;
	m_flHitgroupLeg = gSkillData.fgruntLeg;

	m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 1;
	m_flNextPainTime = UTIL_GlobalTimeBase();

	m_afCapability = bits_CAP_HEAR | bits_CAP_SQUAD | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;

	m_fEnemyEluded = FALSE;
	m_fFirstEncounter = TRUE;// this is true when the grunt spawns, because he hasn't encountered an enemy yet.

	m_HackedGunPos = Vector(0, 0, 55);

	m_cClipSize = FGRUNT_CLIP_SIZE_9MM;
	if (FBitSet(pev->weapons, FGRUNT_SHOTGUN)) {
		if (!IsLeader() && m_iHead == -1) {
			SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SHOTGUN);
		}
		SetBodygroup(FG_GUN_GROUP, FG_GUN_SHOTGUN);
		SetBodygroup(FG_TORSO_GROUP, FG_TORSO_SHOTGUN);
		m_cClipSize = 8;
	}
	else if (FBitSet(pev->weapons, FGRUNT_M249)) {
		SetBodygroup(FG_GUN_GROUP, FG_GUN_SAW);
		SetBodygroup(FG_TORSO_GROUP, FG_TORSO_M249);
		//Random Skins
		if (!IsLeader() && m_iHead == -1) {
			switch ((int)RANDOM_FLOAT(0, 2)) {
			case 0: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SAW); break;
			case 1: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SAW_BLACK); break;
			}
		}

		m_cClipSize = FGRUNT_CLIP_SIZE_M249;
	}
	else {
		SetBodygroup(FG_GUN_GROUP, FG_GUN_MP5);
		if (!IsLeader() && m_iHead == -1) {
			switch ((int)RANDOM_FLOAT(0, 3)) {
				//Random Skins
			case 0: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_MASK); break;
			case 1: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SAW); break;
			case 2: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SAW_BLACK); break;
			}
		}
	}
	m_cAmmoLoaded = m_cClipSize;

	//When is leader, set Beret or Major skin
	if ((pev->spawnflags & SF_MONSTER_SPAWNFLAG_32) && m_iHead == -1) {
		switch ((int)RANDOM_FLOAT(0, 3)) {
			//Random Skins
		case 0: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_BERET); break;
		case 1: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_BERET_BLACK); break;
		case 2: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_MAJOR); break;
		}
	}

	//Overwrite Skins over Editor
	if (m_iHead != -1) {
		switch (INT(m_iHead)) {
		case 0: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_MASK); break;
		case 1: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_BERET); break;
		case 2: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SHOTGUN); break;
		case 3: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SAW); break;
		case 4: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SAW_BLACK); break;
		case 5: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_MP); break;
		case 6: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_MAJOR); break;
		case 7: SetBodygroup(FG_HEAD_GROUP, FG_HEAD_BERET_BLACK); break;
		}
	}

	CRCAllyMonster::g_talkWaitTime = 0;

	MonsterInit();
	StartMonster();
	SetUse(&CHFGrunt::FollowerUse);
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CHFGrunt::Precache() {
	CRCAllyMonster::Precache();

	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/hgrunt_opfor.mdl");

	PRECACHE_SOUND("weapons/saw_reload2.wav");
	PRECACHE_SOUND("weapons/glauncher.wav");
}

//=========================================================
// JustSpoke
//=========================================================
void CHFGrunt::JustSpoke() {
	CRCAllyMonster::g_talkWaitTime = UTIL_GlobalTimeBase() + RANDOM_FLOAT(1.5, 2.0);
	m_iSentence = FGRUNT_SENT_NONE;
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Task_t	tlFGruntFollow[] =
{
	{ TASK_MOVE_TO_TARGET_RANGE,(float)128		},	// Move within 128 of target ent (client)
	{ TASK_SET_SCHEDULE,		(float)SCHED_TARGET_FACE },
};

Schedule_t	slFGruntFollow[] =
{
	{
		tlFGruntFollow,
		HL_ARRAYSIZE(tlFGruntFollow),
		bits_COND_NEW_ENEMY |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_HEAR_SOUND |
		bits_COND_PROVOKED,
		bits_SOUND_DANGER,
		"Follow"
	},
};
Task_t	tlFGruntFaceTarget[] =
{
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_FACE_TARGET,			(float)0		},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_SET_SCHEDULE,		(float)SCHED_TARGET_CHASE },
};

Schedule_t	slFGruntFaceTarget[] =
{
	{
		tlFGruntFaceTarget,
		HL_ARRAYSIZE(tlFGruntFaceTarget),
		bits_COND_CLIENT_PUSH |
		bits_COND_NEW_ENEMY |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_HEAR_SOUND |
		bits_COND_PROVOKED,
		bits_SOUND_DANGER,
		"FaceTarget"
	},
};


Task_t	tlFGruntIdleStand[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT,				(float)2		}, // repick IDLESTAND every two seconds.
	{ TASK_TLK_HEADRESET,		(float)0		}, // reset head position
};

Schedule_t	slFGruntIdleStand[] =
{
	{
		tlFGruntIdleStand,
		HL_ARRAYSIZE(tlFGruntIdleStand),
		bits_COND_NEW_ENEMY |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_HEAR_SOUND |
		bits_COND_SMELL |
		bits_COND_PROVOKED,
		bits_SOUND_COMBAT |// sound flags - change these, and you'll break the talking code.
		bits_SOUND_DANGER |
		bits_SOUND_MEAT |// scents
		bits_SOUND_CARCASS |
		bits_SOUND_GARBAGE,
		"IdleStand"
	},
};
//=========================================================
// FGruntFail
//=========================================================
Task_t	tlFGruntFail[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT,				(float)2		},
	{ TASK_WAIT_PVS,			(float)0		},
};

Schedule_t	slFGruntFail[] =
{
	{
		tlFGruntFail,
		HL_ARRAYSIZE(tlFGruntFail),
		bits_COND_CAN_RANGE_ATTACK1 |
		bits_COND_CAN_RANGE_ATTACK2 |
		bits_COND_CAN_MELEE_ATTACK1 |
		bits_COND_CAN_MELEE_ATTACK2,
		0,
		"FGrunt Fail"
	},
};

//=========================================================
// FGrunt Combat Fail
//=========================================================
Task_t	tlFGruntCombatFail[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT_FACE_ENEMY,		(float)2		},
	{ TASK_WAIT_PVS,			(float)0		},
};

Schedule_t	slFGruntCombatFail[] =
{
	{
		tlFGruntCombatFail,
		HL_ARRAYSIZE(tlFGruntCombatFail),
		bits_COND_CAN_RANGE_ATTACK1 |
		bits_COND_CAN_RANGE_ATTACK2,
		0,
		"FGrunt Combat Fail"
	},
};

//=========================================================
// Victory dance!
//=========================================================
Task_t	tlFGruntVictoryDance[] =
{
	{ TASK_SET_FAIL_SCHEDULE,				(float)SCHED_FAIL			},
	{ TASK_STOP_MOVING,						(float)0					},
	{ TASK_FACE_ENEMY,						(float)0					},
	{ TASK_WAIT,							(float)1.5					},
	{ TASK_GET_PATH_TO_ENEMY_CORPSE,		(float)0					},
	{ TASK_WALK_PATH,						(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,				(float)0					},
	{ TASK_FACE_ENEMY,						(float)0					},
	{ TASK_PLAY_SEQUENCE,					(float)ACT_VICTORY_DANCE	},
};

Schedule_t	slFGruntVictoryDance[] =
{
	{
		tlFGruntVictoryDance,
		HL_ARRAYSIZE(tlFGruntVictoryDance),
		bits_COND_NEW_ENEMY |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE,
		0,
		"FGruntVictoryDance"
	},
};

//=========================================================
// Establish line of fire - move to a position that allows
// the grunt to attack.
//=========================================================
Task_t tlFGruntEstablishLineOfFire[] =
{
	{ TASK_SET_FAIL_SCHEDULE,	(float)SCHED_HGRUNT_ALLY_ELOF_FAIL	},
	{ TASK_GET_PATH_TO_ENEMY,	(float)0						},
	{ TASK_RUN_PATH,			(float)0						},
	{ TASK_WAIT_FOR_MOVEMENT,	(float)0						},
};

Schedule_t slFGruntEstablishLineOfFire[] =
{
	{
		tlFGruntEstablishLineOfFire,
		HL_ARRAYSIZE(tlFGruntEstablishLineOfFire),
		bits_COND_NEW_ENEMY |
		bits_COND_ENEMY_DEAD |
		bits_COND_CAN_RANGE_ATTACK1 |
		bits_COND_CAN_MELEE_ATTACK1 |
		bits_COND_CAN_RANGE_ATTACK2 |
		bits_COND_CAN_MELEE_ATTACK2 |
		bits_COND_HEAR_SOUND,
		bits_SOUND_DANGER,
		"FGruntEstablishLineOfFire"
	},
};

//=========================================================
// FGruntFoundEnemy - FGrunt established sight with an enemy
// that was hiding from the squad.
//=========================================================
Task_t	tlFGruntFoundEnemy[] =
{
	{ TASK_STOP_MOVING,				0							},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,(float)ACT_SIGNAL1			},
};

Schedule_t	slFGruntFoundEnemy[] =
{
	{
		tlFGruntFoundEnemy,
		HL_ARRAYSIZE(tlFGruntFoundEnemy),
		bits_COND_HEAR_SOUND,
		bits_SOUND_DANGER,
		"FGruntFoundEnemy"
	},
};

//=========================================================
// GruntCombatFace Schedule
//=========================================================
Task_t	tlFGruntCombatFace1[] =
{
	{ TASK_STOP_MOVING,				0							},
	{ TASK_SET_ACTIVITY,			(float)ACT_IDLE				},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_WAIT,					(float)1.5					},
	{ TASK_SET_SCHEDULE,			(float)SCHED_HGRUNT_ALLY_SWEEP	},
};

Schedule_t	slFGruntCombatFace[] =
{
	{
		tlFGruntCombatFace1,
		HL_ARRAYSIZE(tlFGruntCombatFace1),
		bits_COND_NEW_ENEMY |
		bits_COND_ENEMY_DEAD |
		bits_COND_CAN_RANGE_ATTACK1 |
		bits_COND_CAN_RANGE_ATTACK2,
		0,
		"Combat Face"
	},
};

//=========================================================
// Suppressing fire - don't stop shooting until the clip is
// empty or FGrunt gets hurt.
//=========================================================
Task_t	tlFGruntSignalSuppress[] =
{
	{ TASK_STOP_MOVING,						0						},
	{ TASK_FACE_IDEAL,						(float)0				},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,		(float)ACT_SIGNAL2		},
	{ TASK_FACE_ENEMY,						(float)0				},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,					(float)0				},
	{ TASK_FACE_ENEMY,						(float)0				},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,					(float)0				},
	{ TASK_FACE_ENEMY,						(float)0				},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,					(float)0				},
	{ TASK_FACE_ENEMY,						(float)0				},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,					(float)0				},
	{ TASK_FACE_ENEMY,						(float)0				},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,					(float)0				},
};

Schedule_t	slFGruntSignalSuppress[] =
{
	{
		tlFGruntSignalSuppress,
		HL_ARRAYSIZE(tlFGruntSignalSuppress),
		bits_COND_ENEMY_DEAD |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_HEAR_SOUND |
		bits_COND_FGRUNT_NOFIRE |
		bits_COND_NO_AMMO_LOADED,

		bits_SOUND_DANGER,
		"SignalSuppress"
	},
};

Task_t	tlFGruntSuppress[] =
{
	{ TASK_STOP_MOVING,			0							},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
};

Schedule_t	slFGruntSuppress[] =
{
	{
		tlFGruntSuppress,
		HL_ARRAYSIZE(tlFGruntSuppress),
		bits_COND_ENEMY_DEAD |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_HEAR_SOUND |
		bits_COND_FGRUNT_NOFIRE |
		bits_COND_NO_AMMO_LOADED,

		bits_SOUND_DANGER,
		"Suppress"
	},
};


//=========================================================
// FGrunt wait in cover - we don't allow danger or the ability
// to attack to break a grunt's run to cover schedule, but
// when a grunt is in cover, we do want them to attack if they can.
//=========================================================
Task_t	tlFGruntWaitInCover[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_SET_ACTIVITY,			(float)ACT_IDLE				},
	{ TASK_WAIT_FACE_ENEMY,			(float)1					},
};

Schedule_t	slFGruntWaitInCover[] =
{
	{
		tlFGruntWaitInCover,
		HL_ARRAYSIZE(tlFGruntWaitInCover),
		bits_COND_NEW_ENEMY |
		bits_COND_HEAR_SOUND |
		bits_COND_CAN_RANGE_ATTACK1 |
		bits_COND_CAN_RANGE_ATTACK2 |
		bits_COND_CAN_MELEE_ATTACK1 |
		bits_COND_CAN_MELEE_ATTACK2,

		bits_SOUND_DANGER,
		"FGruntWaitInCover"
	},
};

//=========================================================
// run to cover.
// !!!BUGBUG - set a decent fail schedule here.
//=========================================================
Task_t	tlFGruntTakeCover1[] =
{
	{ TASK_STOP_MOVING,				(float)0							},
	{ TASK_SET_FAIL_SCHEDULE,		(float)SCHED_HGRUNT_ALLY_TAKECOVER_FAILED	},
	{ TASK_WAIT,					(float)0.2							},
	{ TASK_FIND_COVER_FROM_ENEMY,	(float)0							},
	{ TASK_RUN_PATH,				(float)0							},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0							},
	{ TASK_REMEMBER,				(float)bits_MEMORY_INCOVER			},
	{ TASK_SET_SCHEDULE,			(float)SCHED_HGRUNT_ALLY_WAIT_FACE_ENEMY	},
};

Schedule_t	slFGruntTakeCover[] =
{
	{
		tlFGruntTakeCover1,
		HL_ARRAYSIZE(tlFGruntTakeCover1),
		0,
		0,
		"TakeCover"
	},
};

//=========================================================
// drop grenade then run to cover.
//=========================================================
Task_t	tlFGruntGrenadeCover1[] =
{
	{ TASK_STOP_MOVING,						(float)0							},
	{ TASK_FIND_COVER_FROM_ENEMY,			(float)99							},
	{ TASK_FIND_FAR_NODE_COVER_FROM_ENEMY,	(float)384							},
	{ TASK_PLAY_SEQUENCE,					(float)ACT_SPECIAL_ATTACK1			},
	{ TASK_CLEAR_MOVE_WAIT,					(float)0							},
	{ TASK_RUN_PATH,						(float)0							},
	{ TASK_WAIT_FOR_MOVEMENT,				(float)0							},
	{ TASK_SET_SCHEDULE,					(float)SCHED_HGRUNT_ALLY_WAIT_FACE_ENEMY	},
};

Schedule_t	slFGruntGrenadeCover[] =
{
	{
		tlFGruntGrenadeCover1,
		HL_ARRAYSIZE(tlFGruntGrenadeCover1),
		0,
		0,
		"GrenadeCover"
	},
};


//=========================================================
// drop grenade then run to cover.
//=========================================================
Task_t	tlFGruntTossGrenadeCover1[] =
{
	{ TASK_FACE_ENEMY,						(float)0							},
	{ TASK_RANGE_ATTACK2, 					(float)0							},
	{ TASK_SET_SCHEDULE,					(float)SCHED_TAKE_COVER_FROM_ENEMY	},
};

Schedule_t	slFGruntTossGrenadeCover[] =
{
	{
		tlFGruntTossGrenadeCover1,
		HL_ARRAYSIZE(tlFGruntTossGrenadeCover1),
		0,
		0,
		"TossGrenadeCover"
	},
};

//=========================================================
// hide from the loudest sound source (to run from grenade)
//=========================================================
Task_t	tlFGruntTakeCoverFromBestSound[] =
{
	{ TASK_SET_FAIL_SCHEDULE,			(float)SCHED_COWER			},// duck and cover if cannot move from explosion
	{ TASK_STOP_MOVING,					(float)0					},
	{ TASK_FIND_COVER_FROM_BEST_SOUND,	(float)0					},
	{ TASK_RUN_PATH,					(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,			(float)0					},
	{ TASK_REMEMBER,					(float)bits_MEMORY_INCOVER	},
	{ TASK_TURN_LEFT,					(float)179					},
};

Schedule_t	slFGruntTakeCoverFromBestSound[] =
{
	{
		tlFGruntTakeCoverFromBestSound,
		HL_ARRAYSIZE(tlFGruntTakeCoverFromBestSound),
		0,
		0,
		"FGruntTakeCoverFromBestSound"
	},
};

//=========================================================
// Grunt reload schedule
//=========================================================
Task_t	tlFGruntHideReload[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_SET_FAIL_SCHEDULE,		(float)SCHED_RELOAD			},
	{ TASK_FIND_COVER_FROM_ENEMY,	(float)0					},
	{ TASK_RUN_PATH,				(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0					},
	{ TASK_REMEMBER,				(float)bits_MEMORY_INCOVER	},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_RELOAD			},
};

Schedule_t slFGruntHideReload[] =
{
	{
		tlFGruntHideReload,
		HL_ARRAYSIZE(tlFGruntHideReload),
		bits_COND_HEAVY_DAMAGE |
		bits_COND_HEAR_SOUND,
		bits_SOUND_DANGER,
		"FGruntHideReload"
	}
};

//=========================================================
// Do a turning sweep of the area
//=========================================================
Task_t	tlFGruntSweep[] =
{
	{ TASK_TURN_LEFT,			(float)179	},
	{ TASK_WAIT,				(float)1	},
	{ TASK_TURN_LEFT,			(float)179	},
	{ TASK_WAIT,				(float)1	},
};

Schedule_t	slFGruntSweep[] =
{
	{
		tlFGruntSweep,
		HL_ARRAYSIZE(tlFGruntSweep),
		bits_COND_NEW_ENEMY |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_CAN_RANGE_ATTACK1 |
		bits_COND_CAN_RANGE_ATTACK2 |
		bits_COND_HEAR_SOUND,
		bits_SOUND_WORLD |// sound flags
		bits_SOUND_DANGER |
		bits_SOUND_PLAYER,
		"FGrunt Sweep"
	},
};

//=========================================================
// primary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlFGruntRangeAttack1A[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,		(float)ACT_CROUCH },
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
};

Schedule_t	slFGruntRangeAttack1A[] =
{
	{
		tlFGruntRangeAttack1A,
		HL_ARRAYSIZE(tlFGruntRangeAttack1A),
		bits_COND_NEW_ENEMY |
		bits_COND_ENEMY_DEAD |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_ENEMY_OCCLUDED |
		bits_COND_HEAR_SOUND |
		bits_COND_FGRUNT_NOFIRE |
		bits_COND_NO_AMMO_LOADED,
		bits_SOUND_DANGER,
		"Range Attack1A"
	},
};


//=========================================================
// primary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlFGruntRangeAttack1B[] =
{
	{ TASK_STOP_MOVING,				(float)0		},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,(float)ACT_IDLE_ANGRY  },
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_HGRUNT_ALLY_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
};

Schedule_t	slFGruntRangeAttack1B[] =
{
	{
		tlFGruntRangeAttack1B,
		HL_ARRAYSIZE(tlFGruntRangeAttack1B),
		bits_COND_NEW_ENEMY |
		bits_COND_ENEMY_DEAD |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_ENEMY_OCCLUDED |
		bits_COND_NO_AMMO_LOADED |
		bits_COND_FGRUNT_NOFIRE |
		bits_COND_HEAR_SOUND,
		bits_SOUND_DANGER,
		"Range Attack1B"
	},
};

//=========================================================
// secondary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlFGruntRangeAttack2[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_HGRUNT_ALLY_FACE_TOSS_DIR,		(float)0					},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_RANGE_ATTACK2	},
	{ TASK_SET_SCHEDULE,			(float)SCHED_HGRUNT_ALLY_WAIT_FACE_ENEMY	},// don't run immediately after throwing grenade.
};

Schedule_t	slFGruntRangeAttack2[] =
{
	{
		tlFGruntRangeAttack2,
		HL_ARRAYSIZE(tlFGruntRangeAttack2),
		0,
		0,
		"RangeAttack2"
	},
};

//=========================================================
// repel 
//=========================================================
Task_t	tlFGruntRepel[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_GLIDE 	},
};

Schedule_t	slFGruntRepel[] =
{
	{
		tlFGruntRepel,
		HL_ARRAYSIZE(tlFGruntRepel),
		bits_COND_SEE_ENEMY |
		bits_COND_NEW_ENEMY |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_HEAR_SOUND,
		bits_SOUND_DANGER |
		bits_SOUND_COMBAT |
		bits_SOUND_PLAYER,
		"Repel"
	},
};

//=========================================================
// repel 
//=========================================================
Task_t	tlFGruntRepelAttack[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_FLY 	},
};

Schedule_t	slFGruntRepelAttack[] =
{
	{
		tlFGruntRepelAttack,
		HL_ARRAYSIZE(tlFGruntRepelAttack),
		bits_COND_ENEMY_OCCLUDED,
		0,
		"Repel Attack"
	},
};

//=========================================================
// repel land
//=========================================================
Task_t	tlFGruntRepelLand[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_LAND	},
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0				},
	{ TASK_RUN_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_CLEAR_LASTPOSITION,		(float)0				},
};

Schedule_t	slFGruntRepelLand[] =
{
	{
		tlFGruntRepelLand,
		HL_ARRAYSIZE(tlFGruntRepelLand),
		bits_COND_SEE_ENEMY |
		bits_COND_NEW_ENEMY |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_HEAR_SOUND,

		bits_SOUND_DANGER |
		bits_SOUND_COMBAT |
		bits_SOUND_PLAYER,
		"Repel Land"
	},
};

//=========================================================
// Find medic. Grunt stops moving and calls the nearest medic,
// if none is around, we don't do much. I don't think I have much
// to put in here, other than to make the grunt stop moving, and
// run the medic calling task, I guess.
//=========================================================
Task_t	tlFGruntFindMedic[] =
{
	{ TASK_STOP_MOVING,					(float)0	},
	{ TASK_HGRUNT_ALLY_FIND_MEDIC,		(float)0	},
};

Schedule_t	slFGruntFindMedic[] =
{
	{
		tlFGruntFindMedic,
		HL_ARRAYSIZE(tlFGruntFindMedic),
		bits_COND_NEW_ENEMY |
		bits_COND_SEE_FEAR |
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_HEAR_SOUND |
		bits_COND_PROVOKED,
		bits_SOUND_DANGER,
		"FGrunt Find Medic"
	},
};

DEFINE_CUSTOM_SCHEDULES(CHFGrunt)
{
	slFGruntFollow,
		slFGruntFaceTarget,
		slFGruntIdleStand,
		slFGruntFail,
		slFGruntCombatFail,
		slFGruntVictoryDance,
		slFGruntEstablishLineOfFire,
		slFGruntFoundEnemy,
		slFGruntCombatFace,
		slFGruntSignalSuppress,
		slFGruntSuppress,
		slFGruntWaitInCover,
		slFGruntTakeCover,
		slFGruntGrenadeCover,
		slFGruntTossGrenadeCover,
		slFGruntTakeCoverFromBestSound,
		slFGruntHideReload,
		slFGruntSweep,
		slFGruntRangeAttack1A,
		slFGruntRangeAttack1B,
		slFGruntRangeAttack2,
		slFGruntRepel,
		slFGruntRepelAttack,
		slFGruntRepelLand,
		slFGruntFindMedic,
};

IMPLEMENT_CUSTOM_SCHEDULES(CHFGrunt, CRCAllyMonster);

//=========================================================
// StartTask
//=========================================================
void CHFGrunt::StartTask(Task_t *pTask) {
	m_iTaskStatus = TASKSTATUS_RUNNING;
	switch (pTask->iTask) {
	case TASK_HGRUNT_ALLY_CHECK_FIRE:
		if (!NoFriendlyFire()) {
			SetConditions(bits_COND_FGRUNT_NOFIRE);
		}
		TaskComplete();
		break;
	case TASK_HGRUNT_ALLY_FIND_MEDIC:
		// First try looking for a medic in my squad
		if (InSquad()) {
			CRCAllyMonster *pSquadLeader = MySquadLeader();
			if (pSquadLeader) for (int i = 0; i < MAXRC_SQUAD_MEMBERS; i++) {
				CRCAllyMonster *pMember = pSquadLeader->MySquadMember(i);
				if (pMember && pMember != this) {
					CRCAllyMonster *pMedic = pMember->MyTalkSquadMonsterPointer();
					if (pMedic && pMedic->pev->deadflag == DEAD_NO && FClassnameIs(pMedic->pev, "monster_human_medic_ally")) {
						if (!pMedic->IsFollowing()) {
							ALERT(at_console, "I've found my medic!\n");
							EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fgrunt/medic.wav", VOL_NORM, ATTN_NORM, 0, GetVoicePitch());
							pMedic->GruntHealerCall(this);
							TaskComplete();
						}
					}
				}
			}
		}

		// If not, search bsp.
		if (!TaskIsComplete()) {
			CBaseEntity *pFriend = NULL;
			int i;

			// for each friend in this bsp...
			for (i = 0; i < TLK_CFRIENDS; i++) {
				while (pFriend = EnumFriends(pFriend, i, TRUE)) {
					CRCAllyMonster *pMedic = pFriend->MyTalkSquadMonsterPointer();
					if (pMedic && pMedic->pev->deadflag == DEAD_NO && FClassnameIs(pMedic->pev, "monster_human_medic_ally")) {
						if (!pMedic->IsFollowing()) {
							EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fgrunt/medic.wav", VOL_NORM, ATTN_NORM, 0, GetVoicePitch());
							pMedic->GruntHealerCall(this);
							TaskComplete();
						}
					}
				}
			}
		}

		if (!TaskIsComplete()) {
			TaskFail();
		}
		m_flMedicWaitTime = FGRUNT_MEDIC_WAIT + UTIL_GlobalTimeBase(); // Call again in ten seconds anyway.
		break;
	case TASK_WALK_PATH:
	case TASK_RUN_PATH:
		// grunt no longer assumes he is covered if he moves
		Forget(bits_MEMORY_INCOVER);
		CRCAllyMonster::StartTask(pTask);
		break;
	case TASK_RELOAD:
		m_IdealActivity = ACT_RELOAD;
		break;
	case TASK_HGRUNT_ALLY_FACE_TOSS_DIR:
		break;
	case TASK_FACE_IDEAL:
	case TASK_FACE_ENEMY:
		CRCAllyMonster::StartTask(pTask);
		if (pev->movetype == MOVETYPE_FLY) {
			m_IdealActivity = ACT_GLIDE;
		}
		break;
	default:
		CRCAllyMonster::StartTask(pTask);
		break;
	}
}

//=========================================================
// RunTask
//=========================================================
void CHFGrunt::RunTask(Task_t *pTask) {
	switch (pTask->iTask) {
	case TASK_HGRUNT_ALLY_FACE_TOSS_DIR: {
		// project a point along the toss vector and turn to face that point.
		MakeIdealYaw(pev->origin + m_vecTossVelocity * 64);
		ChangeYaw(pev->yaw_speed);
		if (FacingIdeal()) {
			m_iTaskStatus = TASKSTATUS_COMPLETE;
		}
	}
										 break;
	default: {
		CRCAllyMonster::RunTask(pTask);
	}
			 break;
	}
}

//=========================================================
// GibMonster - make gun fly through the air.
//=========================================================
void CHFGrunt::GibMonster() {
	if (GetBodygroup(3) != 3 && !(pev->spawnflags & SF_MONSTER_SPAWNFLAG_1024)) {
		Vector	vecGunPos;
		Vector	vecGunAngles;
		GetAttachment(0, vecGunPos, vecGunAngles);

		CBaseEntity *pGun;
		if (FBitSet(pev->weapons, FGRUNT_SHOTGUN)) {
			pGun = DropItem("weapon_shotgun", vecGunPos, vecGunAngles);
		}
		else if (FBitSet(pev->weapons, FGRUNT_M249)) {
			pGun = DropItem("weapon_m249", vecGunPos, vecGunAngles);
		}
		else {
			pGun = DropItem("weapon_9mmAR", vecGunPos, vecGunAngles);
		}

		if (pGun) {
			pGun->pev->velocity = Vector(RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(200, 300));
			pGun->pev->avelocity = Vector(0, RANDOM_FLOAT(200, 400), 0);
		}

		if (!(pev->spawnflags & SF_MONSTER_SPAWNFLAG_512)) { //Hack for "Don't Drop Explosives"
			if (FBitSet(pev->weapons, FGRUNT_HANDGRENADE)) {
				pGun = DropItem("weapon_handgrenade", vecGunPos, vecGunAngles);
				if (pGun) {
					pGun->pev->velocity = Vector(RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(200, 300));
					pGun->pev->avelocity = Vector(0, RANDOM_FLOAT(200, 400), 0);
				}
			}
		}

		if (!(pev->spawnflags & SF_MONSTER_SPAWNFLAG_512)) { //Hack for "Don't Drop Explosives"
			if (FBitSet(pev->weapons, FGRUNT_GRENADELAUNCHER)) {
				pGun = DropItem("ammo_ARgrenades", vecGunPos, vecGunAngles);
				if (pGun) {
					pGun->pev->velocity = Vector(RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(200, 300));
					pGun->pev->avelocity = Vector(0, RANDOM_FLOAT(200, 400), 0);
				}
			}
		}
	}

	CRCAllyMonster::GibMonster();
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CHFGrunt::SetYawSpeed() {
	switch (m_Activity) {
	case ACT_WALK:
	case ACT_TURN_LEFT:
	case ACT_TURN_RIGHT:
		pev->yaw_speed = 180;
		break;
	case ACT_IDLE:
	case ACT_RUN:
		pev->yaw_speed = 150;
		break;
	case ACT_RANGE_ATTACK1:
	case ACT_RANGE_ATTACK2:
	case ACT_MELEE_ATTACK1:
	case ACT_MELEE_ATTACK2:
		pev->yaw_speed = 120;
		break;
	case ACT_GLIDE:
	case ACT_FLY:
		pev->yaw_speed = 30;
		break;
	default:
		pev->yaw_speed = 90;
		break;
	}
}

//=========================================================
// CheckRangeAttack2 - this checks the Grunt's grenade
// attack. 
//=========================================================
bool CHFGrunt::CheckRangeAttack2(float flDot, float flDist) {
	if (!FBitSet(pev->weapons, (FGRUNT_HANDGRENADE | FGRUNT_GRENADELAUNCHER)) || FBitSet(pev->weapons, (FGRUNT_M249))) {
		return false;
	}

	// if the grunt isn't moving, it's ok to check.
	if (m_flGroundSpeed != 0) {
		m_fThrowGrenade = FALSE;
		return (bool)m_fThrowGrenade;
	}

	// assume things haven't changed too much since last time
	if (UTIL_GlobalTimeBase() < m_flNextGrenadeCheck) {
		return (bool)m_fThrowGrenade;
	}

	if (!FBitSet(m_hEnemy->pev->flags, FL_ONGROUND) && m_hEnemy->pev->waterlevel == 0 && m_vecEnemyLKP.z > pev->absmax.z) {
		//!!!BUGBUG - we should make this check movetype and make sure it isn't FLY? Players who jump a lot are unlikely to 
		// be grenaded.
		// don't throw grenades at anything that isn't on the ground!
		m_fThrowGrenade = FALSE;
		return (bool)m_fThrowGrenade;
	}

	Vector vecTarget;
	if (FBitSet(pev->weapons, FGRUNT_HANDGRENADE)) {
		// find feet
		if (RANDOM_LONG(0, 1)) {
			// magically know where they are
			vecTarget = Vector(m_hEnemy->pev->origin.x, m_hEnemy->pev->origin.y, m_hEnemy->pev->absmin.z);
		}
		else {
			// toss it to where you last saw them
			vecTarget = m_vecEnemyLKP;
		}
	}
	else {
		// find target
		// vecTarget = m_hEnemy->BodyTarget( pev->origin );
		vecTarget = m_vecEnemyLKP + (m_hEnemy->BodyTarget(pev->origin) - m_hEnemy->pev->origin);
		// estimate position
		if (HasConditions(bits_COND_SEE_ENEMY))
			vecTarget = vecTarget + ((vecTarget - pev->origin).Length() / gSkillData.fgruntGrenadeSpeed) * m_hEnemy->pev->velocity;
	}

	// are any of my squad members near the intended grenade impact area?
	if (InSquad()) {
		if (SquadMemberInRange(vecTarget, 256)) {
			// crap, I might blow my own guy up. Don't throw a grenade and don't check again for a while.
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 1; // one full second.
			m_fThrowGrenade = FALSE;
			return (bool)m_fThrowGrenade;	//AJH need this or it is overridden later.
		}
	}

	if ((vecTarget - pev->origin).Length2D() <= 256) {
		// crap, I don't want to blow myself up
		m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 1; // one full second.
		m_fThrowGrenade = FALSE;
		return (bool)m_fThrowGrenade;
	}


	if (FBitSet(pev->weapons, FGRUNT_HANDGRENADE)) {
		Vector vecToss = VecCheckToss(pev, GetGunPosition(), vecTarget, 0.5);

		if (vecToss != g_vecZero) {
			m_vecTossVelocity = vecToss;

			// throw a hand grenade
			m_fThrowGrenade = TRUE;
			// don't check again for a while.
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase(); // 1/3 second.
		}
		else {
			// don't throw
			m_fThrowGrenade = FALSE;
			// don't check again for a while.
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 1; // one full second.
		}
	}
	else {
		Vector vecToss = VecCheckThrow(pev, GetGunPosition(), vecTarget, gSkillData.fgruntGrenadeSpeed, 0.5);
		if (vecToss != g_vecZero) {
			m_vecTossVelocity = vecToss;

			// throw a hand grenade
			m_fThrowGrenade = TRUE;
			// don't check again for a while.
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 0.3; // 1/3 second.
		}
		else {
			// don't throw
			m_fThrowGrenade = FALSE;
			// don't check again for a while.
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 1; // one full second.
		}
	}

	return (bool)m_fThrowGrenade;
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CHFGrunt::HandleAnimEvent(MonsterEvent_t *pEvent) {
	Vector	vecShootDir, vecShootOrigin;
	switch (pEvent->event) {
	case HGRUNT_ALLY_AE_DROP_GUN: {
		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_1024) break; //LRC

		Vector	vecGunPos, vecGunAngles;
		GetAttachment(0, vecGunPos, vecGunAngles);

		// switch to body group with no gun.
		SetBodygroup(FG_GUN_GROUP, FG_GUN_NONE);

		if (FBitSet(pev->weapons, FGRUNT_SHOTGUN)) {
			DropItem("weapon_shotgun", vecGunPos, vecGunAngles);
		}
		else if (FBitSet(pev->weapons, FGRUNT_M249)) {
			DropItem("weapon_m249", vecGunPos, vecGunAngles);
		}
		else {
			DropItem("weapon_9mmAR", vecGunPos, vecGunAngles);
		}

		CBaseEntity *pGun;
		if (!(pev->spawnflags & SF_MONSTER_SPAWNFLAG_2048)) {
			if (FBitSet(pev->weapons, FGRUNT_HANDGRENADE)) {
				pGun = DropItem("weapon_handgrenade", vecGunPos, vecGunAngles);
				if (pGun) {
					pGun->pev->velocity = Vector(RANDOM_FLOAT(-30, 60), RANDOM_FLOAT(-20, 30), RANDOM_FLOAT(120, 220));
					pGun->pev->avelocity = Vector(0, RANDOM_FLOAT(90, 220), 0);
				}
			}
		}

		if (!(pev->spawnflags & SF_MONSTER_SPAWNFLAG_2048)) {
			if (FBitSet(pev->weapons, FGRUNT_GRENADELAUNCHER)) {
				pGun = DropItem("ammo_ARgrenades", vecGunPos, vecGunAngles);
				if (pGun) {
					pGun->pev->velocity = Vector(RANDOM_FLOAT(-30, 60), RANDOM_FLOAT(-20, 30), RANDOM_FLOAT(120, 220));
					pGun->pev->avelocity = Vector(0, RANDOM_FLOAT(90, 220), 0);
				}
			}
		}
	}
								  break;
	case HGRUNT_ALLY_AE_RELOAD:
		if (FBitSet(pev->weapons, FGRUNT_M249)) {
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/saw_reload2.wav", VOL_NORM, ATTN_NORM);
		}
		else {
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "hgrunt/gr_reload1.wav", VOL_NORM, ATTN_NORM);
		}

		m_cAmmoLoaded = m_cClipSize;
		ClearConditions(bits_COND_NO_AMMO_LOADED);
		break;
	case HGRUNT_ALLY_AE_GREN_TOSS: {
		UTIL_MakeVectors(pev->angles);
		//LRC - a bit of a hack. Ideally the grunts would work out in advance whether it's ok to throw.
		if (m_pCine) {
			Vector vecToss = g_vecZero;
			if (m_hTargetEnt != NULL && m_pCine->PreciseAttack()) {
				vecToss = VecCheckToss(pev, GetGunPosition(), m_hTargetEnt->pev->origin, 0.5);
			}

			if (vecToss == g_vecZero) {
				vecToss = (gpGlobals->v_forward*0.5 + gpGlobals->v_up*0.5).Normalize()*gSkillData.hgruntGrenadeSpeed;
			}
			CGrenade::ShootTimed(pev, GetGunPosition(), vecToss, RANDOM_FLOAT(1.5, 3));
		}
		else
			CGrenade::ShootTimed(pev, GetGunPosition(), m_vecTossVelocity, RANDOM_FLOAT(1.5, 3));

		m_fThrowGrenade = FALSE;
		m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 6;// wait six seconds before even looking again to see if a grenade can be thrown.
												   // !!!LATER - when in a group, only try to throw grenade if ordered.
	}
								   break;
	case HGRUNT_ALLY_AE_GREN_LAUNCH: {
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/glauncher.wav", VOL_LOW, ATTN_NORM);
		//LRC: firing due to a script?
		if (m_pCine) {
			Vector vecToss;
			if (m_hTargetEnt != NULL && m_pCine->PreciseAttack())
				vecToss = VecCheckThrow(pev, GetGunPosition(), m_hTargetEnt->pev->origin, gSkillData.hgruntGrenadeSpeed, 0.5);
			else {
				// just shoot diagonally up+forwards
				UTIL_MakeVectors(pev->angles);
				vecToss = (gpGlobals->v_forward*0.5 + gpGlobals->v_up*0.5).Normalize() * gSkillData.hgruntGrenadeSpeed;
			}

			CGrenade::ShootContact(pev, GetGunPosition(), vecToss);
		}
		else
			CGrenade::ShootContact(pev, GetGunPosition(), m_vecTossVelocity);

		m_fThrowGrenade = FALSE;
		if (g_iSkillLevel == SKILL_HARD)
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + RANDOM_FLOAT(2, 5);// wait a random amount of time before shooting again
		else
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 6;// wait six seconds before even looking again to see if a grenade can be thrown.
	}
									 break;
	case HGRUNT_ALLY_AE_GREN_DROP: {
		UTIL_MakeVectors(pev->angles);
		CGrenade::ShootTimed(pev, pev->origin + gpGlobals->v_forward * 17 - gpGlobals->v_right * 27 + gpGlobals->v_up * 6, g_vecZero, RANDOM_FLOAT(1.5, 2));
	}
								   break;
	case HGRUNT_ALLY_AE_BURST1:
	case HGRUNT_ALLY_AE_BURST2:
	case HGRUNT_ALLY_AE_BURST3: {
		if (FBitSet(pev->weapons, FGRUNT_M249)) {
			ShootM249();
		}
		else if (FBitSet(pev->weapons, FGRUNT_SHOTGUN)) {
			if (UTIL_GlobalTimeBase() >= m_nShotgunDouble && RANDOM_LONG(0, 4) >= 1 && m_cAmmoLoaded >= 2) {
				ShootShotgunDouble();
				m_nShotgunDouble = UTIL_GlobalTimeBase() + RANDOM_FLOAT(3, 6);
				m_fLockShootTime = UTIL_GlobalTimeBase() + 2;
			}
			else {
				ShootShotgun();
			}
		}
		else {
			ShootMP5();
		}
	}
								break;
	case HGRUNT_ALLY_AE_KICK: {
		CBaseEntity *pHurt = Kick();
		if (pHurt) {
			// SOUND HERE!
			UTIL_MakeVectors(pev->angles);
			pHurt->pev->punchangle.x = 20;
			pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * 150 + gpGlobals->v_up * 80;
			pHurt->TakeDamage(pev, pev, gSkillData.fgruntDmgKick, DMG_CLUB);
		}
	}
							  break;
	case HGRUNT_ALLY_AE_CAUGHT_ENEMY: {
		if (FOkToSpeak()) {
			SENTENCEG_PlayRndSz(ENT(pev), "FG_ALERT", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
			JustSpoke();
		}
	}
	default:
		CRCAllyMonster::HandleAnimEvent(pEvent);
		break;
	}
}

//=========================================================
// TraceAttack - make sure we're not taking it in the helmet
//=========================================================
void CHFGrunt::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CRCAllyMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
		return;
	}

	if (pev->takedamage) {
		if (m_fImmortal)
			flDamage = 0;

		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_64) {
			CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
			if (pEnt->IsPlayer()) { return; }
			if (pevAttacker->owner) {
				pEnt = CBaseEntity::Instance(pevAttacker->owner);
				if (pEnt->IsPlayer()) { return; }
			}
		}

		switch (ptr->iHitgroup) {
		case HITGROUP_HEAD:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_HEAD\n", GetClassname());

			if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_CLUB)) {
				flDamage -= 20;
				if (flDamage <= 0 && (GetBodygroup(FG_HEAD_GROUP) == FG_HEAD_MASK || GetBodygroup(FG_HEAD_GROUP) == FG_HEAD_MP)) {
					UTIL_Ricochet(ptr->vecEndPos, 1.0);
					flDamage = 0.01;
				}
			}
			else {
				flDamage = m_flHitgroupHead * flDamage;
			}
			break;
		case HITGROUP_CHEST:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_CHEST\n", GetClassname());
			if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST)) {
				flDamage = (m_flHitgroupChest*flDamage) / 2;
			}
			break;
		case HITGROUP_STOMACH:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_STOMACH\n", GetClassname());
			if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST)) {
				flDamage = (m_flHitgroupStomach*flDamage) / 2;
			}
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_ARM\n", GetClassname());
			flDamage = m_flHitgroupArm * flDamage;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_LEG\n", GetClassname());
			flDamage = m_flHitgroupLeg * flDamage;
			break;
		}
	}

	SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);// a little surface blood.
	TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
	AddMultiDamage(pevAttacker, this, flDamage, bitsDamageType);
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Schedule_t* CHFGrunt::GetScheduleOfType(int Type) {
	Schedule_t *psched;

	switch (Type) {
		// Hook these to make a looping schedule
	case SCHED_TARGET_FACE: {
		// call base class default so that barney will talk
		// when 'used' 
		psched = CRCAllyMonster::GetScheduleOfType(Type);

		if (psched == slIdleStand)
			return slFGruntFaceTarget;	// override this for different target face behavior
		else
			return psched;
	}
							break;
	case SCHED_TARGET_CHASE: {
		return slFGruntFollow;
	}
							 break;
	case SCHED_IDLE_STAND: {
		psched = CRCAllyMonster::GetScheduleOfType(Type);

		if (psched == slIdleStand) {
			// just look straight ahead.
			return slFGruntIdleStand;
		}
		else
			return psched;
	}
						   break;
	case SCHED_TAKE_COVER_FROM_ENEMY: {
		return &slFGruntTakeCover[0];
	}
									  break;
	case SCHED_TAKE_COVER_FROM_BEST_SOUND: {
		return &slFGruntTakeCoverFromBestSound[0];
	}
										   break;
	case SCHED_HGRUNT_ALLY_FIND_MEDIC: {
		return &slFGruntFindMedic[0];
	}
									   break;
	case SCHED_HGRUNT_ALLY_TAKECOVER_FAILED: {
		if (HasConditions(bits_COND_CAN_RANGE_ATTACK1) && OccupySlot(bits_SLOTS_FGRUNT_ENGAGE)) {
			return GetScheduleOfType(SCHED_RANGE_ATTACK1);
		}
		else {
			return GetScheduleOfType(SCHED_FAIL);
		}
	}
											 break;
	case SCHED_HGRUNT_ALLY_ELOF_FAIL: {
		return GetScheduleOfType(SCHED_RANGE_ATTACK1);
	}
									  break;
	case SCHED_HGRUNT_ALLY_ESTABLISH_LINE_OF_FIRE: {
		return &slFGruntEstablishLineOfFire[0];
	}
												   break;
	case SCHED_RANGE_ATTACK1: {
		// randomly stand or crouch
		if (RANDOM_LONG(0, 9) == 0)
			m_fStanding = RANDOM_LONG(0, 1);

		if (m_fStanding)
			return &slFGruntRangeAttack1B[0];
		else
			return &slFGruntRangeAttack1A[0];
	}
							  break;
	case SCHED_RANGE_ATTACK2: {
		return &slFGruntRangeAttack2[0];
	}
							  break;
	case SCHED_COMBAT_FACE: {
		return &slFGruntCombatFace[0];
	}
							break;
	case SCHED_HGRUNT_ALLY_WAIT_FACE_ENEMY: {
		return &slFGruntWaitInCover[0];
	}
	case SCHED_HGRUNT_ALLY_SWEEP: {
		return &slFGruntSweep[0];
	}
								  break;
	case SCHED_HGRUNT_ALLY_COVER_AND_RELOAD: {
		return &slFGruntHideReload[0];
	}
											 break;
	case SCHED_HGRUNT_ALLY_FOUND_ENEMY: {
		return &slFGruntFoundEnemy[0];
	}
										break;
	case SCHED_VICTORY_DANCE: {
		if (InSquad()) {
			if (!IsLeader()) {
				return &slFGruntFail[0];
			}
		}

		if (IsFollowing()) {
			return &slFGruntFail[0];
		}

		return &slFGruntVictoryDance[0];
	}
							  break;
	case SCHED_HGRUNT_ALLY_SUPPRESS: {
		if (m_fFirstEncounter) {
			m_fFirstEncounter = FALSE;// after first encounter, leader won't issue handsigns anymore when he has a new enemy
			return &slFGruntSignalSuppress[0];
		}
		else {
			return &slFGruntSuppress[0];
		}
	}
									 break;
	case SCHED_FAIL: {
		if (m_hEnemy != NULL) {
			// grunt has an enemy, so pick a different default fail schedule most likely to help recover.
			return &slFGruntCombatFail[0];
		}

		return &slFGruntFail[0];
	}
					 break;
	case SCHED_HGRUNT_ALLY_REPEL: {
		if (pev->velocity.z > -128)
			pev->velocity.z -= 32;
		return &slFGruntRepel[0];
	}
								  break;
	case SCHED_HGRUNT_ALLY_REPEL_ATTACK: {
		if (pev->velocity.z > -128)
			pev->velocity.z -= 32;
		return &slFGruntRepelAttack[0];
	}
										 break;
	case SCHED_HGRUNT_ALLY_REPEL_LAND: {
		return &slFGruntRepelLand[0];
	}
									   break;
	default: {
		return CRCAllyMonster::GetScheduleOfType(Type);
	}
	}
}

//=========================================================
// SetActivity 
//=========================================================
void CHFGrunt::SetActivity(Activity NewActivity) {
	int	iSequence = ACTIVITY_NOT_AVAILABLE;
	void *pmodel = GET_MODEL_PTR(ENT(pev));

	switch (NewActivity) {
	case ACT_RANGE_ATTACK1:
		// grunt is either shooting standing or shooting crouched
		if (FBitSet(pev->weapons, FGRUNT_9MMAR)) {
			if (m_fStanding) {
				// get aimable sequence
				iSequence = LookupSequence("standing_mp5");
			}
			else {
				// get crouching shoot
				iSequence = LookupSequence("crouching_mp5");
			}
		}
		else if (FBitSet(pev->weapons, FGRUNT_SHOTGUN)) {
			if (m_fStanding) {
				// get aimable sequence
				iSequence = LookupSequence("standing_shotgun");
			}
			else {
				// get crouching shoot
				iSequence = LookupSequence("crouching_shotgun");
			}
		}
		else {
			if (m_fStanding) {
				// get aimable sequence
				iSequence = LookupSequence("standing_saw");
			}
			else {
				// get crouching shoot
				iSequence = LookupSequence("crouching_saw");
			}
		}
		break;
	case ACT_RANGE_ATTACK2:
		// grunt is going to a secondary long range attack. This may be a thrown 
		// grenade or fired grenade, we must determine which and pick proper sequence
		if (pev->weapons & FGRUNT_HANDGRENADE) {
			// get toss anim
			iSequence = LookupSequence("throwgrenade");
		}
		else if (pev->weapons & FGRUNT_GRENADELAUNCHER) { // LRC: added a test to stop a marine without a launcher from firing.
		 // get launch anim
			iSequence = LookupSequence("launchgrenade");
		}
		else {
			ALERT(at_debug, "No grenades available. "); // flow into the error message we get at the end...
		}
		break;
	case ACT_RUN:
		if (pev->health <= FGRUNT_LIMP_HEALTH) {
			// limp!
			iSequence = LookupActivity(ACT_RUN_HURT);
		}
		else {
			iSequence = LookupActivity(NewActivity);
		}
		break;
	case ACT_WALK:
		if (pev->health <= FGRUNT_LIMP_HEALTH) {
			// limp!
			iSequence = LookupActivity(ACT_WALK_HURT);
		}
		else {
			iSequence = LookupActivity(NewActivity);
		}
		break;
	case ACT_IDLE:
		if (m_MonsterState == MONSTERSTATE_COMBAT) {
			NewActivity = ACT_IDLE_ANGRY;
		}
		iSequence = LookupActivity(NewActivity);
		break;
	default:
		iSequence = LookupActivity(NewActivity);
		break;
	}

	m_Activity = NewActivity; // Go ahead and set this so it doesn't keep trying when the anim is not present

	// Set to the desired anim, or default anim if the desired is not present
	if (iSequence > ACTIVITY_NOT_AVAILABLE) {
		if (pev->sequence != iSequence || !m_fSequenceLoops) {
			pev->frame = 0;
		}

		pev->sequence = iSequence;	// Set to the reset anim (if it's there)
		ResetSequenceInfo();
		SetYawSpeed();
	}
	else {
		// Not available try to get default anim
		ALERT(at_console, "%s has no sequence for act:%d\n", GetClassname(), NewActivity);
		pev->sequence = 0;	// Set to the reset anim (if it's there)
	}
}

//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *CHFGrunt::GetSchedule() {
	// clear old sentence
	m_iSentence = -1;

	// flying? If PRONE, barnacle has me. IF not, it's assumed I am rapelling. 
	if (pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE) {
		if (pev->flags & FL_ONGROUND) {
			// just landed
			SetMoveType(MOVETYPE_STEP);
			return GetScheduleOfType(SCHED_HGRUNT_ALLY_REPEL_LAND);
		}
		else {
			// repel down a rope, 
			if (m_MonsterState == MONSTERSTATE_COMBAT)
				return GetScheduleOfType(SCHED_HGRUNT_ALLY_REPEL_ATTACK);
			else
				return GetScheduleOfType(SCHED_HGRUNT_ALLY_REPEL);
		}
	}

	if (HasConditions(bits_COND_ENEMY_DEAD) && FOkToSpeak()) {
		PlaySentence("FG_KILL", 4, VOL_NORM, ATTN_NORM);
	}

	// grunts place HIGH priority on running away from danger sounds.
	if (HasConditions(bits_COND_HEAR_SOUND)) {
		CSound *pSound;
		pSound = PBestSound();

		ASSERT(pSound != NULL);
		if (pSound) {
			if (pSound->m_iType & bits_SOUND_DANGER) {
				// dangerous sound nearby!

				//!!!KELLY - currently, this is the grunt's signal that a grenade has landed nearby,
				// and the grunt should find cover from the blast
				// good place for "SHIT!" or some other colorful verbal indicator of dismay.
				// It's not safe to play a verbal order here "Scatter", etc cause 
				// this may only affect a single individual in a squad. 

				if (FOkToSpeak()) {
					SENTENCEG_PlayRndSz(ENT(pev), "FG_GREN", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
					JustSpoke();
				}

				return GetScheduleOfType(SCHED_TAKE_COVER_FROM_BEST_SOUND);
			}
		}
	}

	switch (m_MonsterState) {
	case MONSTERSTATE_COMBAT: {
		if (HasConditions(bits_COND_ENEMY_DEAD)) { // dead enemy
			// call base class, all code to handle dead enemies is centralized there.
			return CBaseMonster::GetSchedule();
		}

		if (HasConditions(bits_COND_NEW_ENEMY)) { // new enemy
			if (InSquad()) {
				MySquadLeader()->m_fEnemyEluded = FALSE;

				if (!IsLeader()) {
					if (HasConditions(bits_COND_CAN_RANGE_ATTACK1)) {
						return GetScheduleOfType(SCHED_HGRUNT_ALLY_SUPPRESS);
					}
					else {
						return GetScheduleOfType(SCHED_HGRUNT_ALLY_ESTABLISH_LINE_OF_FIRE);
					}
				}
				else {
					ALERT(at_aiconsole, "leader spotted player!\n");
					//!!!KELLY - the leader of a squad of grunts has just seen the player or a 
					// monster and has made it the squad's enemy. You
					// can check pev->flags for FL_CLIENT to determine whether this is the player
					// or a monster. He's going to immediately start
					// firing, though. If you'd like, we can make an alternate "first sight" 
					// schedule where the leader plays a handsign anim
					// that gives us enough time to hear a short sentence or spoken command
					// before he starts pluggin away.
					if (FOkToSpeak()) {
						if ((m_hEnemy != NULL) &&
							(m_hEnemy->Classify() != CLASS_PLAYER_ALLY) &&
							(m_hEnemy->Classify() != CLASS_HUMAN_MILITARY) &&
							(m_hEnemy->Classify() != CLASS_HUMAN_PASSIVE) &&
							(m_hEnemy->Classify() != CLASS_MACHINE))
							// monster
							SENTENCEG_PlayRndSz(ENT(pev), "FG_ALERT", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
						else
							// player
							SENTENCEG_PlayRndSz(ENT(pev), "FG_ATTACK", VOL_NORM, ATTN_NORM, 0, m_voicePitch);

						JustSpoke();
					}

					if (HasConditions(bits_COND_CAN_RANGE_ATTACK1)) {
						return GetScheduleOfType(SCHED_HGRUNT_ALLY_SUPPRESS);
					}
					else {
						return GetScheduleOfType(SCHED_HGRUNT_ALLY_ESTABLISH_LINE_OF_FIRE);
					}
				}
			}
		}
		else if (HasConditions(bits_COND_NO_AMMO_LOADED)) { // no ammo
		 //!!!KELLY - this individual just realized he's out of bullet ammo. 
		 // He's going to try to find cover to run to and reload, but rarely, if 
		 // none is available, he'll drop and reload in the open here. 
			return GetScheduleOfType(SCHED_HGRUNT_ALLY_COVER_AND_RELOAD);
		}
		else if (HasConditions(bits_COND_LIGHT_DAMAGE)) { // damaged just a little
			if (RANDOM_LONG(0, 99) <= 90 && m_hEnemy != NULL) {
				if (FOkToSpeak()) {
					//SENTENCEG_PlayRndSz( ENT(pev), "HG_COVER", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
					m_iSentence = FGRUNT_SENT_COVER;
					//JustSpoke();
				}
				// only try to take cover if we actually have an enemy!

				return GetScheduleOfType(SCHED_TAKE_COVER_FROM_ENEMY);
			}
			else {
				return GetScheduleOfType(SCHED_SMALL_FLINCH);
			}
		}
		else if (HasConditions(bits_COND_CAN_MELEE_ATTACK1)) { // can kick
			return GetScheduleOfType(SCHED_MELEE_ATTACK1);
		}
		else if (FBitSet(pev->weapons, FGRUNT_GRENADELAUNCHER) && HasConditions(bits_COND_CAN_RANGE_ATTACK2) && OccupySlot(bits_SLOTS_FGRUNT_GRENADE)) { // can grenade launch
		 // shoot a grenade if you can
			return GetScheduleOfType(SCHED_RANGE_ATTACK2);
		}
		else if (HasConditions(bits_COND_CAN_RANGE_ATTACK1)) { // can shoot
			if (InSquad()) {
				// if the enemy has eluded the squad and a squad member has just located the enemy
				// and the enemy does not see the squad member, issue a call to the squad to waste a 
				// little time and give the player a chance to turn.
				if (MySquadLeader()->m_fEnemyEluded && !HasConditions(bits_COND_ENEMY_FACING_ME)) {
					MySquadLeader()->m_fEnemyEluded = FALSE;
					return GetScheduleOfType(SCHED_HGRUNT_ALLY_FOUND_ENEMY);
				}
			}

			if (OccupySlot(bits_SLOTS_FGRUNT_ENGAGE)) {
				// try to take an available ENGAGE slot
				return GetScheduleOfType(SCHED_RANGE_ATTACK1);
			}
			else if (HasConditions(bits_COND_CAN_RANGE_ATTACK2) && OccupySlot(bits_SLOTS_FGRUNT_GRENADE)) {
				// throw a grenade if can and no engage slots are available
				return GetScheduleOfType(SCHED_RANGE_ATTACK2);
			}
			else {
				// hide!
				return GetScheduleOfType(SCHED_TAKE_COVER_FROM_ENEMY);
			}
		}
		else if (HasConditions(bits_COND_ENEMY_OCCLUDED)) { // can't see enemy
			if (HasConditions(bits_COND_CAN_RANGE_ATTACK2) && OccupySlot(bits_SLOTS_FGRUNT_GRENADE)) {
				//!!!KELLY - this grunt is about to throw or fire a grenade at the player. Great place for "fire in the hole"  "frag out" etc
				if (FOkToSpeak()) {
					SENTENCEG_PlayRndSz(ENT(pev), "FG_THROW", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
					JustSpoke();
				}

				return GetScheduleOfType(SCHED_RANGE_ATTACK2);
			}
			else if (OccupySlot(bits_SLOTS_FGRUNT_ENGAGE)) {
				return GetScheduleOfType(SCHED_HGRUNT_ALLY_ESTABLISH_LINE_OF_FIRE);
			}
			else {
				//!!!KELLY - grunt is going to stay put for a couple seconds to see if
				// the enemy wanders back out into the open, or approaches the
				// grunt's covered position. Good place for a taunt, I guess?
				if (FOkToSpeak() && RANDOM_LONG(0, 1)) {
					SENTENCEG_PlayRndSz(ENT(pev), "FG_TAUNT", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
					JustSpoke();
				}

				return GetScheduleOfType(SCHED_STANDOFF);
			}
		}

		if (HasConditions(bits_COND_SEE_ENEMY) && !HasConditions(bits_COND_CAN_RANGE_ATTACK1)) {
			return GetScheduleOfType(SCHED_HGRUNT_ALLY_ESTABLISH_LINE_OF_FIRE);
		}
	}
							  break;
	case MONSTERSTATE_ALERT:
	case MONSTERSTATE_IDLE:
		if (HasConditions(bits_COND_NO_AMMO_LOADED)) {
			return GetScheduleOfType(SCHED_RELOAD);
		}

		if (pev->health < pev->max_health && pev->health <= 20 && (m_flMedicWaitTime < UTIL_GlobalTimeBase())) {
			// Find a medic 
			return GetScheduleOfType(SCHED_HGRUNT_ALLY_FIND_MEDIC); // Unresolved
		}

		if (m_hEnemy == NULL && IsFollowing()) {
			if (!m_hTargetEnt->IsAlive()) {
				// UNDONE: Comment about the recently dead player here?
				StopFollowing(false);
				break;
			}
			else {
				if (HasConditions(bits_COND_CLIENT_PUSH)) {
					return GetScheduleOfType(SCHED_MOVE_AWAY_FOLLOW);
				}

				return GetScheduleOfType(SCHED_TARGET_FACE);
			}
		}

		if (HasConditions(bits_COND_CLIENT_PUSH)) {
			return GetScheduleOfType(SCHED_MOVE_AWAY);
		}

		// try to say something about smells
		TrySmellTalk();
		break;
	}

	return CRCAllyMonster::GetSchedule();
}

//=========================================================
// CHFGruntRepel - when triggered, spawns a
// repelling down a line.
//=========================================================
LINK_ENTITY_TO_CLASS(monster_hgrunt_ally_repel, CHFGruntRepel);

void CHFGruntRepel::Spawn() {
	Precache();
	SetSolidType(SOLID_NOT);

	SetUse(&CHFGruntRepel::RepelUse);
}

void CHFGruntRepel::Precache() {
	UTIL_PrecacheOther("monster_human_grunt_ally");
	m_iSpriteTexture = PRECACHE_MODEL("sprites/rope.spr");
}

void CHFGruntRepel::RepelUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) {
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + Vector(0, 0, -4096.0), dont_ignore_monsters, ENT(pev), &tr);

	CBaseEntity *pEntity = Create("monster_human_grunt_ally", pev->origin, pev->angles);
	CBaseMonster *pGrunt = pEntity->MyMonsterPointer();
	pGrunt->SetMoveType(MOVETYPE_FLY);
	pGrunt->pev->velocity = Vector(0, 0, RANDOM_FLOAT(-196, -128));
	pGrunt->SetActivity(ACT_GLIDE);
	pGrunt->m_vecLastPosition = tr.vecEndPos;

	CBeam *pBeam = CBeam::BeamCreate("sprites/rope.spr", 10);
	pBeam->PointEntInit(pev->origin + Vector(0, 0, 112), pGrunt->entindex());
	pBeam->SetFlags(BEAM_FSOLID);
	pBeam->SetColor(255, 255, 255);
	pBeam->SetThink(&CBeam::SUB_Remove);
	pBeam->SetNextThink(-4096.0 * tr.flFraction / pGrunt->pev->velocity.z + 0.5);

	UTIL_Remove(this);
}

//=========================================================
// BGrunt Dead PROP
//
// Designer selects a pose in worldcraft, 0 through num_poses-1
// this value is added to what is selected as the 'first dead pose'
// among the monster's normal animations. All dead poses must
// appear sequentially in the model file. Be sure and set
// the m_iFirstPose properly!
//
//=========================================================
char *CDeadFGrunt::m_szPoses[] = { "deadstomach", "deadside", "deadsitting", "dead_on_back", "dead_headcrabbed", "hgrunt_dead_stomach", "dead_canyon" };

void CDeadFGrunt::KeyValue(KeyValueData *pkvd) {
	if (FStrEq(pkvd->szKeyName, "pose")) {
		m_iPose = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "head")) {
		m_iHead = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue(pkvd);
}

LINK_ENTITY_TO_CLASS(monster_human_grunt_ally_dead, CDeadFGrunt);

//=========================================================
// ********** DeadBGrunt SPAWN **********
//=========================================================
void CDeadFGrunt::Spawn() {
	PRECACHE_MODEL("models/hgrunt_opfor.mdl");
	SET_MODEL(ENT(pev), "models/hgrunt_opfor.mdl");

	pev->effects = 0;
	pev->yaw_speed = 8;
	pev->sequence = 0;
	m_bloodColor = BLOOD_COLOR_RED;

	pev->sequence = LookupSequence(m_szPoses[m_iPose]);
	if (pev->sequence == -1)
	{
		ALERT(at_console, "Dead fgrunt with bad pose\n");
	}
	// Corpses have less health
	pev->health = 8;


	if (pev->weapons == 0 || pev->weapons == -1)
	{
		SetBodygroup(FG_GUN_GROUP, FG_GUN_NONE);
	}
	if (FBitSet(pev->weapons, FGRUNT_SHOTGUN))
	{
		SetBodygroup(FG_GUN_GROUP, FG_GUN_SHOTGUN);
		SetBodygroup(FG_TORSO_GROUP, FG_TORSO_SHOTGUN);
	}
	if (FBitSet(pev->weapons, FGRUNT_9MMAR))
	{
		SetBodygroup(FG_GUN_GROUP, FG_GUN_MP5);
	}
	if (FBitSet(pev->weapons, FGRUNT_M249))
	{
		SetBodygroup(FG_GUN_GROUP, FG_GUN_SAW);
		SetBodygroup(FG_TORSO_GROUP, FG_TORSO_M249);
	}

	if (m_iHead == 0)
		SetBodygroup(FG_HEAD_GROUP, FG_HEAD_MASK);
	if (m_iHead == 1)
		SetBodygroup(FG_HEAD_GROUP, FG_HEAD_BERET);
	if (m_iHead == 2)
		SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SHOTGUN);
	if (m_iHead == 3)
		SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SAW);
	if (m_iHead == 4)
		SetBodygroup(FG_HEAD_GROUP, FG_HEAD_SAW_BLACK);
	if (m_iHead == 5)
		SetBodygroup(FG_HEAD_GROUP, FG_HEAD_MP);
	if (m_iHead == 6)
		SetBodygroup(FG_HEAD_GROUP, FG_HEAD_MAJOR);
	if (m_iHead == 7)
		SetBodygroup(FG_HEAD_GROUP, FG_HEAD_BERET_BLACK);

	MonsterInitDead();
}