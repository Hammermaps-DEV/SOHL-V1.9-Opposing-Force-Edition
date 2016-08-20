/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Spirit of Half-Life and their logos are the property of their respective owners.
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
*   All Rights Reserved.
*
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/
//=========================================================
// NPC: Human Frunt Ally Medic
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"animation.h"
#include	"rcallymonster.h"
#include	"schedule.h"
#include	"defaultai.h"
#include	"scripted.h"
#include	"weapons.h"
#include	"soundent.h"
#include	"customentity.h"
#include	"decals.h"
#include	"proj_grenade.h"
#include	"monster_hgrunt_medic.h"

//=========================================================
// torch defines
//=========================================================
#define	MEDIC_CLIP_SIZE_9MM			17
#define	MEDIC_CLIP_SIZE_DEAGLE		7
#define MEDIC_LIMP_HEALTH			20

#define MEDIC_EAGLE					1
#define MEDIC_PISTOL				2
#define MEDIC_NEEDLE				3

// Weapon group
#define GUN_GROUP					3
#define GUN_EAGLE					0
#define GUN_PISTOL					1
#define GUN_NEEDLE					2
#define GUN_NONE					3

// Head group
#define MEDIC_HEAD_GROUP			2
#define MEDIC_HEAD_WHITE			0
#define MEDIC_HEAD_BLACK			1

//=========================================================
// monster-specific conditions
//=========================================================
#define bits_COND_MEDIC_NOFIRE	( bits_COND_SPECIAL1 )

//=========================================================
// monster-specific tasks
//=========================================================
enum  {
	TASK_MEDIC_FACE_TOSS_DIR = LAST_TALKMONSTER_TASK + 1,
	TASK_MEDIC_CHECK_FIRE,
	TASK_MEDIC_SAY_HEAL,
	TASK_MEDIC_HEAL,
};
//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		MEDIC_AE_RELOAD			( 2 )
#define		MEDIC_AE_KICK			( 3 )
#define		MEDIC_AE_BURST1			( 4 )
#define		MEDIC_AE_BURST2			( 5 ) 
#define		MEDIC_AE_BURST3			( 6 ) 
#define		MEDIC_AE_GREN_TOSS		( 7 )
#define		MEDIC_AE_GREN_LAUNCH	( 8 )
#define		MEDIC_AE_GREN_DROP		( 9 )
#define		MEDIC_AE_CAUGHT_ENEMY	( 10 ) // grunt established sight with an enemy (player only) that had previously eluded the squad.
#define		MEDIC_AE_DROP_GUN		( 11 ) // grunt (probably dead) is dropping his mp5.
#define		MEDIC_AE_HIDEGUN		( 15 )
#define		MEDIC_AE_SHOWNEEDLE		( 16 )
#define		MEDIC_AE_HIDENEEDLE		( 17 )
#define		MEDIC_AE_SHOWGUN		( 18 )

//=========================================================
// monster-specific schedule types
//=========================================================
enum {
	SCHED_MEDIC_SUPPRESS = LAST_TALKMONSTER_SCHEDULE + 1,
	SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE,// move to a location to set up an attack against the enemy. (usually when a friendly is in the way).
	SCHED_MEDIC_COVER_AND_RELOAD,
	SCHED_MEDIC_SWEEP,
	SCHED_MEDIC_FOUND_ENEMY,
	SCHED_MEDIC_REPEL,
	SCHED_MEDIC_REPEL_ATTACK,
	SCHED_MEDIC_REPEL_LAND,
	SCHED_MEDIC_WAIT_FACE_ENEMY,
	SCHED_MEDIC_TAKECOVER_FAILED,// special schedule type that forces analysis of conditions and picks the best possible schedule to recover from this type of failure.
	SCHED_MEDIC_ELOF_FAIL,
};

LINK_ENTITY_TO_CLASS( monster_human_medic_ally, CMedic );

TYPEDESCRIPTION	CMedic::m_SaveData[] = {
	DEFINE_FIELD( CMedic, m_flNextGrenadeCheck, FIELD_TIME ),
	DEFINE_FIELD( CMedic, m_flNextPainTime, FIELD_TIME ),
	DEFINE_FIELD( CMedic, m_vecTossVelocity, FIELD_VECTOR ),
	DEFINE_FIELD( CMedic, m_fThrowGrenade, FIELD_BOOLEAN ),
	DEFINE_FIELD( CMedic, m_fStanding, FIELD_BOOLEAN ),
	DEFINE_FIELD( CMedic, m_fFirstEncounter, FIELD_BOOLEAN ),
	DEFINE_FIELD( CMedic, m_cClipSize, FIELD_INTEGER ),
	DEFINE_FIELD( CMedic, m_healTime, FIELD_TIME ),
	DEFINE_FIELD( CMedic, m_flHealAnount, FIELD_FLOAT ),
	DEFINE_FIELD( CMedic, m_iHead, FIELD_INTEGER ),
	DEFINE_FIELD( CMedic, m_fDepleteLine, FIELD_BOOLEAN ),
};

IMPLEMENT_SAVERESTORE( CMedic, CRCAllyMonster );

const char *CMedic::pGruntSentences[] = {
	"FG_GREN", // grenade scared grunt
	"FG_ALERT", // sees player
	"FG_MONSTER", // sees monster
	"FG_COVER", // running to cover
	"FG_THROW", // about to throw grenade
	"FG_CHARGE",  // running out to get the enemy
	"FG_TAUNT", // say rude things
};

enum {
	MEDIC_SENT_NONE = -1,
	MEDIC_SENT_GREN = 0,
	MEDIC_SENT_ALERT,
	MEDIC_SENT_MONSTER,
	MEDIC_SENT_COVER,
	MEDIC_SENT_THROW,
	MEDIC_SENT_CHARGE,
	MEDIC_SENT_TAUNT,
} MEDIC_SENTENCE_TYPES;

//=========================================================
// KeyValue
// !!! netname entvar field is used in squadmonster for groupname!!!
//=========================================================
void CMedic :: KeyValue( KeyValueData *pkvd ) {
	if (FStrEq(pkvd->szKeyName, "head")) {
		m_iHead = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	} else if (FStrEq(pkvd->szKeyName, "immortal")) {
		m_fImmortal = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	} else {
		CRCAllyMonster::KeyValue(pkvd);
	}
}

//=========================================================
// JustSpoke
//=========================================================
void CMedic::JustSpoke(void) {
	CRCAllyMonster::g_talkWaitTime = UTIL_GlobalTimeBase() + RANDOM_FLOAT(1.5, 2.0);
	m_iSentence = MEDIC_SENT_NONE;
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Task_t	tlMedicFollow[] =
{
	{ TASK_MOVE_TO_TARGET_RANGE,(float)128		},	// Move within 128 of target ent (client)
	{ TASK_SET_SCHEDULE,		(float)SCHED_TARGET_FACE },
};

Schedule_t	slMedicFollow[] =
{
	{
		tlMedicFollow,
		HL_ARRAYSIZE( tlMedicFollow ),
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND |
		bits_COND_MEDIC_HEAL |
		bits_COND_PROVOKED,
		bits_SOUND_DANGER,
		"Follow"
	},
};
Task_t	tlMedicFaceTarget[] =
{
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_FACE_TARGET,			(float)0		},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_SET_SCHEDULE,		(float)SCHED_TARGET_CHASE },
};

Schedule_t	slMedicFaceTarget[] =
{
	{
		tlMedicFaceTarget,
		HL_ARRAYSIZE( tlMedicFaceTarget ),
		bits_COND_CLIENT_PUSH	|
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND |
		bits_COND_MEDIC_HEAL |
		bits_COND_PROVOKED,
		bits_SOUND_DANGER,
		"FaceTarget"
	},
};


Task_t	tlMedicIdleStand[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT,				(float)2		}, // repick IDLESTAND every two seconds.
	{ TASK_TLK_HEADRESET,		(float)0		}, // reset head position
};

Schedule_t	slMedicIdleStand[] =
{
	{ 
		tlMedicIdleStand,
		HL_ARRAYSIZE( tlMedicIdleStand ),
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND	|
		bits_COND_SMELL			|
		bits_COND_MEDIC_HEAL	|
		bits_COND_PROVOKED,
		bits_SOUND_COMBAT		|// sound flags - change these, and you'll break the talking code.
		bits_SOUND_DANGER		|
		bits_SOUND_MEAT			|// scents
		bits_SOUND_CARCASS		|
		bits_SOUND_GARBAGE,
		"IdleStand"
	},
};
//=========================================================
// GruntFail
//=========================================================
Task_t	tlMedicFail[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT,				(float)2		},
	{ TASK_WAIT_PVS,			(float)0		},
};

Schedule_t	slMedicFail[] =
{
	{
		tlMedicFail,
		HL_ARRAYSIZE( tlMedicFail ),
		bits_COND_CAN_RANGE_ATTACK1 |
		bits_COND_CAN_RANGE_ATTACK2 |
		bits_COND_CAN_MELEE_ATTACK1 |
		bits_COND_MEDIC_HEAL		|
		bits_COND_CAN_MELEE_ATTACK2,
		0,
		"Grunt Fail"
	},
};

//=========================================================
// Grunt Combat Fail
//=========================================================
Task_t	tlMedicCombatFail[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT_FACE_ENEMY,		(float)2		},
	{ TASK_WAIT_PVS,			(float)0		},
};

Schedule_t	slMedicCombatFail[] =
{
	{
		tlMedicCombatFail,
		HL_ARRAYSIZE ( tlMedicCombatFail ),
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2,
		0,
		"Grunt Combat Fail"
	},
};

//=========================================================
// Victory dance!
//=========================================================
Task_t	tlMedicVictoryDance[] =
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

Schedule_t	slMedicVictoryDance[] =
{
	{ 
		tlMedicVictoryDance,
		HL_ARRAYSIZE ( tlMedicVictoryDance ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_HEAVY_DAMAGE,
		0,
		"GruntVictoryDance"
	},
};

//=========================================================
// Establish line of fire - move to a position that allows
// the grunt to attack.
//=========================================================
Task_t tlMedicEstablishLineOfFire[] = 
{
	{ TASK_SET_FAIL_SCHEDULE,	(float)SCHED_MEDIC_ELOF_FAIL	},
	{ TASK_GET_PATH_TO_ENEMY,	(float)0						},
	{ TASK_RUN_PATH,			(float)0						},
	{ TASK_WAIT_FOR_MOVEMENT,	(float)0						},
};

Schedule_t slMedicEstablishLineOfFire[] =
{
	{ 
		tlMedicEstablishLineOfFire,
		HL_ARRAYSIZE ( tlMedicEstablishLineOfFire ),
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_MELEE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2	|
		bits_COND_CAN_MELEE_ATTACK2	|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER,
		"GruntEstablishLineOfFire"
	},
};

//=========================================================
// GruntFoundEnemy - grunt established sight with an enemy
// that was hiding from the squad.
//=========================================================
Task_t	tlMedicFoundEnemy[] =
{
	{ TASK_STOP_MOVING,				0							},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,(float)ACT_SIGNAL1			},
};

Schedule_t	slMedicFoundEnemy[] =
{
	{ 
		tlMedicFoundEnemy,
		HL_ARRAYSIZE ( tlMedicFoundEnemy ), 
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER,
		"GruntFoundEnemy"
	},
};

//=========================================================
// GruntCombatFace Schedule
//=========================================================
Task_t	tlMedicCombatFace1[] =
{
	{ TASK_STOP_MOVING,				0							},
	{ TASK_SET_ACTIVITY,			(float)ACT_IDLE				},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_WAIT,					(float)1.5					},
	{ TASK_SET_SCHEDULE,			(float)SCHED_MEDIC_SWEEP	},
};

Schedule_t	slMedicCombatFace[] =
{
	{ 
		tlMedicCombatFace1,
		HL_ARRAYSIZE ( tlMedicCombatFace1 ), 
		bits_COND_NEW_ENEMY				|
		bits_COND_ENEMY_DEAD			|
		bits_COND_CAN_RANGE_ATTACK1		|
		bits_COND_CAN_RANGE_ATTACK2,
		0,
		"Combat Face"
	},
};

//=========================================================
// Suppressing fire - don't stop shooting until the clip is
// empty or grunt gets hurt.
//=========================================================
Task_t	tlMedicSignalSuppress[] =
{
	{ TASK_STOP_MOVING,					0						},
	{ TASK_FACE_IDEAL,					(float)0				},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,	(float)ACT_SIGNAL2		},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_MEDIC_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_MEDIC_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_MEDIC_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_MEDIC_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_MEDIC_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
};

Schedule_t	slMedicSignalSuppress[] =
{
	{ 
		tlMedicSignalSuppress,
		HL_ARRAYSIZE ( tlMedicSignalSuppress ), 
		bits_COND_ENEMY_DEAD		|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND		|
		bits_COND_MEDIC_NOFIRE		|
		bits_COND_NO_AMMO_LOADED,

		bits_SOUND_DANGER,
		"SignalSuppress"
	},
};

Task_t	tlMedicSuppress[] =
{
	{ TASK_STOP_MOVING,			0							},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
};

Schedule_t	slMedicSuppress[] =
{
	{ 
		tlMedicSuppress,
		HL_ARRAYSIZE ( tlMedicSuppress ), 
		bits_COND_ENEMY_DEAD		|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND		|
		bits_COND_MEDIC_NOFIRE		|
		bits_COND_NO_AMMO_LOADED,

		bits_SOUND_DANGER,
		"Suppress"
	},
};


//=========================================================
// grunt wait in cover - we don't allow danger or the ability
// to attack to break a grunt's run to cover schedule, but
// when a grunt is in cover, we do want them to attack if they can.
//=========================================================
Task_t	tlMedicWaitInCover[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_SET_ACTIVITY,			(float)ACT_IDLE				},
	{ TASK_WAIT_FACE_ENEMY,			(float)1					},
};

Schedule_t	slMedicWaitInCover[] =
{
	{ 
		tlMedicWaitInCover,
		HL_ARRAYSIZE ( tlMedicWaitInCover ), 
		bits_COND_NEW_ENEMY			|
		bits_COND_HEAR_SOUND		|
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2	|
		bits_COND_CAN_MELEE_ATTACK1	|
		bits_COND_CAN_MELEE_ATTACK2,

		bits_SOUND_DANGER,
		"GruntWaitInCover"
	},
};

//=========================================================
// run to cover.
// !!!BUGBUG - set a decent fail schedule here.
//=========================================================
Task_t	tlMedicTakeCover1[] =
{
	{ TASK_STOP_MOVING,				(float)0							},
	{ TASK_SET_FAIL_SCHEDULE,		(float)SCHED_MEDIC_TAKECOVER_FAILED	},
	{ TASK_WAIT,					(float)0.2							},
	{ TASK_FIND_COVER_FROM_ENEMY,	(float)0							},
	{ TASK_RUN_PATH,				(float)0							},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0							},
	{ TASK_REMEMBER,				(float)bits_MEMORY_INCOVER			},
	{ TASK_SET_SCHEDULE,			(float)SCHED_MEDIC_WAIT_FACE_ENEMY	},
};

Schedule_t	slMedicTakeCover[] =
{
	{ 
		tlMedicTakeCover1,
		HL_ARRAYSIZE ( tlMedicTakeCover1 ), 
		0,
		0,
		"TakeCover"
	},
};

//=========================================================
// drop grenade then run to cover.
//=========================================================
Task_t	tlMedicGrenadeCover1[] =
{
	{ TASK_STOP_MOVING,						(float)0							},
	{ TASK_FIND_COVER_FROM_ENEMY,			(float)99							},
	{ TASK_FIND_FAR_NODE_COVER_FROM_ENEMY,	(float)384							},
	{ TASK_PLAY_SEQUENCE,					(float)ACT_SPECIAL_ATTACK1			},
	{ TASK_CLEAR_MOVE_WAIT,					(float)0							},
	{ TASK_RUN_PATH,						(float)0							},
	{ TASK_WAIT_FOR_MOVEMENT,				(float)0							},
	{ TASK_SET_SCHEDULE,					(float)SCHED_MEDIC_WAIT_FACE_ENEMY	},
};

Schedule_t	slMedicGrenadeCover[] =
{
	{ 
		tlMedicGrenadeCover1,
		HL_ARRAYSIZE ( tlMedicGrenadeCover1 ), 
		0,
		0,
		"GrenadeCover"
	},
};


//=========================================================
// drop grenade then run to cover.
//=========================================================
Task_t	tlMedicTossGrenadeCover1[] =
{
	{ TASK_FACE_ENEMY,						(float)0							},
	{ TASK_RANGE_ATTACK2, 					(float)0							},
	{ TASK_SET_SCHEDULE,					(float)SCHED_TAKE_COVER_FROM_ENEMY	},
};

Schedule_t	slMedicTossGrenadeCover[] =
{
	{ 
		tlMedicTossGrenadeCover1,
		HL_ARRAYSIZE ( tlMedicTossGrenadeCover1 ), 
		0,
		0,
		"TossGrenadeCover"
	},
};

//=========================================================
// hide from the loudest sound source (to run from grenade)
//=========================================================
Task_t	tlMedicTakeCoverFromBestSound[] =
{
	{ TASK_SET_FAIL_SCHEDULE,			(float)SCHED_COWER			},// duck and cover if cannot move from explosion
	{ TASK_STOP_MOVING,					(float)0					},
	{ TASK_FIND_COVER_FROM_BEST_SOUND,	(float)0					},
	{ TASK_RUN_PATH,					(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,			(float)0					},
	{ TASK_REMEMBER,					(float)bits_MEMORY_INCOVER	},
	{ TASK_TURN_LEFT,					(float)179					},
};

Schedule_t	slMedicTakeCoverFromBestSound[] =
{
	{ 
		tlMedicTakeCoverFromBestSound,
		HL_ARRAYSIZE ( tlMedicTakeCoverFromBestSound ), 
		0,
		0,
		"GruntTakeCoverFromBestSound"
	},
};

//=========================================================
// Grunt reload schedule
//=========================================================
Task_t	tlMedicHideReload[] =
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

Schedule_t slMedicHideReload[] = 
{
	{
		tlMedicHideReload,
		HL_ARRAYSIZE ( tlMedicHideReload ),
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND,

		bits_SOUND_DANGER,
		"GruntHideReload"
	}
};

//=========================================================
// Do a turning sweep of the area
//=========================================================
Task_t	tlMedicSweep[] =
{
	{ TASK_TURN_LEFT,			(float)179	},
	{ TASK_WAIT,				(float)1	},
	{ TASK_TURN_LEFT,			(float)179	},
	{ TASK_WAIT,				(float)1	},
};

Schedule_t	slMedicSweep[] =
{
	{ 
		tlMedicSweep,
		HL_ARRAYSIZE ( tlMedicSweep ), 
		
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_HEAR_SOUND,

		bits_SOUND_WORLD		|// sound flags
		bits_SOUND_DANGER		|
		bits_SOUND_PLAYER,

		"Grunt Sweep"
	},
};

//=========================================================
// primary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlMedicRangeAttack1A[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,		(float)ACT_CROUCH },
	{ TASK_MEDIC_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
};

Schedule_t	slMedicRangeAttack1A[] =
{
	{ 
		tlMedicRangeAttack1A,
		HL_ARRAYSIZE ( tlMedicRangeAttack1A ), 
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_ENEMY_OCCLUDED	|
		bits_COND_HEAR_SOUND		|
		bits_COND_MEDIC_NOFIRE		|
		bits_COND_NO_AMMO_LOADED,
		
		bits_SOUND_DANGER,
		"Range Attack1A"
	},
};


//=========================================================
// primary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlMedicRangeAttack1B[] =
{
	{ TASK_STOP_MOVING,				(float)0		},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,(float)ACT_IDLE_ANGRY  },
	{ TASK_MEDIC_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_MEDIC_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
};

Schedule_t	slMedicRangeAttack1B[] =
{
	{ 
		tlMedicRangeAttack1B,
		HL_ARRAYSIZE ( tlMedicRangeAttack1B ), 
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_ENEMY_OCCLUDED	|
		bits_COND_NO_AMMO_LOADED	|
		bits_COND_MEDIC_NOFIRE		|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER,
		"Range Attack1B"
	},
};

//=========================================================
// secondary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlMedicRangeAttack2[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_MEDIC_FACE_TOSS_DIR,		(float)0					},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_RANGE_ATTACK2	},
	{ TASK_SET_SCHEDULE,			(float)SCHED_MEDIC_WAIT_FACE_ENEMY	},// don't run immediately after throwing grenade.
};

Schedule_t	slMedicRangeAttack2[] =
{
	{ 
		tlMedicRangeAttack2,
		HL_ARRAYSIZE ( tlMedicRangeAttack2 ), 
		0,
		0,
		"RangeAttack2"
	},
};


//=========================================================
// repel 
//=========================================================
Task_t	tlMedicRepel[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_GLIDE 	},
};

Schedule_t	slMedicRepel[] =
{
	{ 
		tlMedicRepel,
		HL_ARRAYSIZE ( tlMedicRepel ), 
		bits_COND_SEE_ENEMY			|
		bits_COND_NEW_ENEMY			|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER			|
		bits_SOUND_COMBAT			|
		bits_SOUND_PLAYER, 
		"Repel"
	},
};


//=========================================================
// repel 
//=========================================================
Task_t	tlMedicRepelAttack[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_FLY 	},
};

Schedule_t	slMedicRepelAttack[] =
{
	{ 
		tlMedicRepelAttack,
		HL_ARRAYSIZE ( tlMedicRepelAttack ), 
		bits_COND_ENEMY_OCCLUDED,
		0,
		"Repel Attack"
	},
};

//=========================================================
// repel land
//=========================================================
Task_t	tlMedicRepelLand[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_LAND	},
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0				},
	{ TASK_RUN_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_CLEAR_LASTPOSITION,		(float)0				},
};

Schedule_t	slMedicRepelLand[] =
{
	{ 
		tlMedicRepelLand,
		HL_ARRAYSIZE ( tlMedicRepelLand ), 
		bits_COND_SEE_ENEMY			|
		bits_COND_NEW_ENEMY			|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER			|
		bits_SOUND_COMBAT			|
		bits_SOUND_PLAYER, 
		"Repel Land"
	},
};
//=========================================================
// heal - heal the player or grunt
// 
//=========================================================
Task_t	tlMedicHeal[] =
{
	{ TASK_MOVE_TO_TARGET_RANGE,			(float)50		},	// Move within 60 of target ent (client)
	{ TASK_SET_FAIL_SCHEDULE,				(float)SCHED_TARGET_CHASE },	// If you fail, catch up with that guy! (change this to put syringe away and then chase)
	{ TASK_FACE_IDEAL,						(float)0		},
	{ TASK_MEDIC_SAY_HEAL,					(float)0		},
	{ TASK_PLAY_SEQUENCE_FACE_TARGET,		(float)ACT_ARM	},			// Whip out the needle
	{ TASK_MEDIC_HEAL,						(float)0	},	// Put it in the player
	{ TASK_PLAY_SEQUENCE_FACE_TARGET,		(float)ACT_DISARM	},			// Put away the needle
};

Schedule_t	slMedicHeal[] =
{
	{
		tlMedicHeal,
		HL_ARRAYSIZE ( tlMedicHeal ),
		0,	// Don't interrupt or he'll end up running around with a needle all the time
		0,
		"Heal"
	},
};

DEFINE_CUSTOM_SCHEDULES( CMedic )
{
	slMedicFollow,
	slMedicFaceTarget,
	slMedicIdleStand,
	slMedicFail,
	slMedicCombatFail,
	slMedicVictoryDance,
	slMedicEstablishLineOfFire,
	slMedicFoundEnemy,
	slMedicCombatFace,
	slMedicSignalSuppress,
	slMedicSuppress,
	slMedicWaitInCover,
	slMedicTakeCover,
	slMedicGrenadeCover,
	slMedicTossGrenadeCover,
	slMedicTakeCoverFromBestSound,
	slMedicHideReload,
	slMedicSweep,
	slMedicRangeAttack1A,
	slMedicRangeAttack1B,
	slMedicRangeAttack2,
	slMedicRepel,
	slMedicRepelAttack,
	slMedicRepelLand,
	slMedicHeal,
};

IMPLEMENT_CUSTOM_SCHEDULES( CMedic, CRCAllyMonster );

//=========================================================
// StartTask
//=========================================================
void CMedic :: StartTask( Task_t *pTask ) {
	m_iTaskStatus = TASKSTATUS_RUNNING;
	switch ( pTask->iTask ) {
		case TASK_MEDIC_SAY_HEAL:
			Talk( 2 );
			m_hTalkTarget = m_hTargetEnt;
			PlaySentence( "MG_HEAL", 2, VOL_NORM, ATTN_IDLE );
			TaskComplete();
		break;
		case TASK_MEDIC_CHECK_FIRE:
			if ( !NoFriendlyFire() ) {
				SetConditions( bits_COND_MEDIC_NOFIRE );
			}
			TaskComplete();
		break;
		case TASK_WALK_PATH:
		case TASK_RUN_PATH:
			// grunt no longer assumes he is covered if he moves
			Forget( bits_MEMORY_INCOVER );
			CRCAllyMonster ::StartTask( pTask );
		break;
		case TASK_RELOAD:
			m_IdealActivity = ACT_RELOAD;
		break;
		case TASK_MEDIC_FACE_TOSS_DIR: break;
		case TASK_MEDIC_HEAL:
			m_IdealActivity = ACT_MELEE_ATTACK2;
			Heal();
		break;
		case TASK_FACE_IDEAL:
		case TASK_FACE_ENEMY:
			CRCAllyMonster :: StartTask( pTask );
			if (pev->movetype == MOVETYPE_FLY) {
				m_IdealActivity = ACT_GLIDE;
			}
		break;
		default: 
			CRCAllyMonster :: StartTask( pTask );
		break;
	}
}

//=========================================================
// RunTask
//=========================================================
void CMedic :: RunTask( Task_t *pTask ) {
	switch ( pTask->iTask ) {
		case TASK_MEDIC_FACE_TOSS_DIR: {
			// project a point along the toss vector and turn to face that point.
			MakeIdealYaw( pev->origin + m_vecTossVelocity * 64 );
			ChangeYaw( pev->yaw_speed );
			if ( FacingIdeal() ) {
				m_iTaskStatus = TASKSTATUS_COMPLETE;
			}
		}
		break;
		case TASK_MEDIC_HEAL: {
			if ( m_fSequenceFinished ) {
				TaskComplete();
			} else {
				if ( TargetDistance() > 90 )
					TaskComplete();
				pev->ideal_yaw = UTIL_VecToYaw( m_hTargetEnt->pev->origin - pev->origin );
				ChangeYaw( pev->yaw_speed );
			}
		}
		break;
		default: {
			CRCAllyMonster :: RunTask( pTask );
		}
		break;
	}
}
//=========================================================
// GibMonster - make gun fly through the air.
//=========================================================
void CMedic::GibMonster(void) {
	if (GetBodygroup(3) != 2 && !(pev->spawnflags & SF_MONSTER_SPAWNFLAG_1024)) {
		Vector	vecGunPos;
		Vector	vecGunAngles;
		GetAttachment(0, vecGunPos, vecGunAngles);

		CBaseEntity *pGun;
		if (pev->weapons == MEDIC_PISTOL) {
			pGun = DropItem("weapon_9mmhandgun", vecGunPos, vecGunAngles);
		} else if (pev->weapons == MEDIC_EAGLE) {
			pGun = DropItem("weapon_eagle", vecGunPos, vecGunAngles);
		}

		if (pGun) {
			pGun->pev->velocity = Vector(RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(200, 300));
			pGun->pev->avelocity = Vector(0, RANDOM_FLOAT(200, 400), 0);
		}
	}

	CRCAllyMonster::GibMonster();
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CMedic::SetYawSpeed(void) {
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
// CheckRangeAttack1 - overridden for HGrunt, cause 
// FCanCheckAttacks() doesn't disqualify all attacks based
// on whether or not the enemy is occluded because unlike
// the base class, the HGrunt can attack when the enemy is
// occluded (throw grenade over wall, etc). We must 
// disqualify the machine gun attack if the enemy is occluded.
//=========================================================
BOOL CMedic::CheckRangeAttack1(float flDot, float flDist) {
	if ((GetBodygroup(3) != 2) && (GetBodygroup(3) != 3)) {
		return CRCAllyMonster::CheckRangeAttack1(flDot, flDist);
	}

	return FALSE;
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CMedic::HandleAnimEvent(MonsterEvent_t *pEvent) {
	switch( pEvent->event ) {
		case MEDIC_AE_SHOWNEEDLE: {
			SetBodygroup( GUN_GROUP, GUN_NEEDLE );
		}
		break;
		case MEDIC_AE_SHOWGUN: {
			if (pev->weapons == MEDIC_EAGLE) {
				SetBodygroup(GUN_GROUP, GUN_EAGLE);
			} else {
				SetBodygroup(GUN_GROUP, GUN_PISTOL);
			}
		}
		break;
		case MEDIC_AE_HIDEGUN: {
			SetBodygroup( GUN_GROUP, GUN_NONE );
		}
		break;
		case MEDIC_AE_HIDENEEDLE: {
			SetBodygroup( GUN_GROUP, GUN_NONE );
		}
		break;
		case MEDIC_AE_DROP_GUN: {
			Vector	vecGunPos, vecGunAngles;
			GetAttachment(0, vecGunPos, vecGunAngles);
			
			// switch to body group with no gun.
			SetBodygroup(GUN_GROUP, GUN_NONE);

			// now spawn a gun.
			if (pev->weapons == MEDIC_PISTOL) {
				DropItem("weapon_9mmhandgun", vecGunPos, vecGunAngles);
			} else if (pev->weapons == MEDIC_EAGLE) {
				DropItem("weapon_eagle", vecGunPos, vecGunAngles);
			}
		}
		break;
		case MEDIC_AE_RELOAD: {
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "hgrunt/gr_reload1.wav", VOL_NORM, ATTN_NORM);
			m_cAmmoLoaded = m_cClipSize;
			ClearConditions(bits_COND_NO_AMMO_LOADED);
		}
		break;
		case MEDIC_AE_GREN_TOSS: {
			UTIL_MakeVectors(pev->angles);
			CGrenade::ShootTimed(pev, GetGunPosition(), m_vecTossVelocity, RANDOM_FLOAT(1.5, 3));
			
			m_fThrowGrenade = FALSE;
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 6;// wait six seconds before even looking again to see if a grenade can be thrown.
		}
		break;
		case MEDIC_AE_GREN_DROP: {
			UTIL_MakeVectors( pev->angles );
			CGrenade::ShootTimed( pev, pev->origin + gpGlobals->v_forward * 17 - gpGlobals->v_right * 27 + gpGlobals->v_up * 6, g_vecZero, RANDOM_FLOAT(1.5, 2));
		}
		break;
		case MEDIC_AE_BURST1:
		case MEDIC_AE_BURST2:
		case MEDIC_AE_BURST3: {
			if ( pev->weapons == MEDIC_EAGLE )
				ShootDesertEagle();
			else
				ShootGlock();
		}
		break;
		case MEDIC_AE_KICK: {
			CBaseEntity *pHurt = Kick();
			if ( pHurt ) {
				// SOUND HERE!
				UTIL_MakeVectors( pev->angles );
				pHurt->pev->punchangle.x = 20;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * 150 + gpGlobals->v_up * 80;
				pHurt->TakeDamage( pev, pev, gSkillData.medicDmgKick, DMG_CLUB );
			}
		}
		break;
		case MEDIC_AE_CAUGHT_ENEMY: {
			if ( FOkToSpeak() ) {
				SENTENCEG_PlayRndSz(ENT(pev), "FG_ALERT", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
				JustSpoke();
			}
		}
		break;
		default:
			CRCAllyMonster::HandleAnimEvent( pEvent );
		break;
	}
}

//=========================================================
// Spawn
//=========================================================
void CMedic::Spawn(void) {
	Precache( );

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/hgrunt_medic.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_RED;

	if (pev->health == 0) //LRC
		pev->health = gSkillData.medicHealth;

	m_flHealAnount = gSkillData.medicHeal;

	pev->view_ofs		= Vector ( 0, 0, 50 );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_MonsterState		= MONSTERSTATE_NONE;

	m_flDebug = false; //Debug Massages

	m_flHitgroupHead = gSkillData.fgruntHead;
	m_flHitgroupChest = gSkillData.fgruntChest;
	m_flHitgroupStomach = gSkillData.fgruntStomach;
	m_flHitgroupArm = gSkillData.fgruntArm;
	m_flHitgroupLeg = gSkillData.fgruntLeg;

	m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 1;
	m_flNextPainTime	= UTIL_GlobalTimeBase();

	m_afCapability		= bits_CAP_HEAR | bits_CAP_SQUAD | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;

	m_hHealTarget		= NULL;
	m_fEnemyEluded		= FALSE;
	m_fFirstEncounter	= TRUE;// this is true when the grunt spawns, because he hasn't encountered an enemy yet.

	m_HackedGunPos = Vector ( 0, 0, 55 );

	if (!pev->weapons) {
		pev->weapons = MEDIC_EAGLE;
	}

	if ( pev->weapons == MEDIC_PISTOL ) {
		SetBodygroup( GUN_GROUP, GUN_PISTOL );
		m_cClipSize = MEDIC_CLIP_SIZE_9MM;
	} else if ( pev->weapons == MEDIC_EAGLE ) {
		SetBodygroup( GUN_GROUP, GUN_EAGLE );
		m_cClipSize = MEDIC_CLIP_SIZE_DEAGLE;
	} else if ( pev->weapons == MEDIC_NEEDLE ) {
		SetBodygroup( GUN_GROUP, GUN_NEEDLE );
		m_cClipSize = MEDIC_CLIP_SIZE_DEAGLE;
	}

	m_cAmmoLoaded = m_cClipSize;

	if ( m_iHead == 0 ) {
		SetBodygroup( MEDIC_HEAD_GROUP, MEDIC_HEAD_WHITE );
	} else if ( m_iHead == 1 ) {
		SetBodygroup( MEDIC_HEAD_GROUP, MEDIC_HEAD_BLACK );
	} else {
		switch (RANDOM_LONG(0, 1)) {
			case 0:
				SetBodygroup(MEDIC_HEAD_GROUP, MEDIC_HEAD_WHITE);
			break;
			case 1:
				SetBodygroup(MEDIC_HEAD_GROUP, MEDIC_HEAD_BLACK);
			break;
		}
	}

	CRCAllyMonster::g_talkWaitTime = 0;

	MonsterInit();
	StartMonster();
	SetUse(&CMedic :: FollowerUse );
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CMedic::Precache(void) {
	CRCAllyMonster::Precache();

	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/hgrunt_medic.mdl");
}

//=========================================================
// HealerFollow
//=========================================================
void CMedic::HealerFollow( CBaseEntity *pHealTarget ) {
	if ( m_pCine )
		m_pCine->CancelScript();

	m_hTargetEnt = pHealTarget;
	m_hTalkTarget = pHealTarget;

	ClearSchedule();
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Schedule_t* CMedic::GetScheduleOfType(int Type) {
	Schedule_t *psched;

	switch( Type )
	{
	// Hook these to make a looping schedule
	case SCHED_TARGET_FACE:
		{
			// call base class default so that barney will talk
			// when 'used' 
			psched = CRCAllyMonster::GetScheduleOfType(Type);

			if (psched == slIdleStand)
				return slMedicFaceTarget;	// override this for different target face behavior
			else
				return psched;
		}
	case SCHED_TARGET_CHASE:
		{
			return slMedicFollow;
		}
	case SCHED_IDLE_STAND:
		{
			psched = CRCAllyMonster::GetScheduleOfType(Type);

			if (psched == slIdleStand)
			{
				// just look straight ahead.
				return slMedicIdleStand;
			}
			else
				return psched;	
		}
	case SCHED_TAKE_COVER_FROM_ENEMY:
		{
			return &slMedicTakeCover[ 0 ];
		}
	case SCHED_TAKE_COVER_FROM_BEST_SOUND:
		{
			return &slMedicTakeCoverFromBestSound[ 0 ];
		}
	case SCHED_MEDIC_TAKECOVER_FAILED:
		{
			if ( HasConditions( bits_COND_CAN_RANGE_ATTACK1 ) && OccupySlot( bits_SLOTS_FGRUNT_ENGAGE ) )
			{
				return GetScheduleOfType( SCHED_RANGE_ATTACK1 );
			}
			else
			{
				return GetScheduleOfType ( SCHED_FAIL );
			}
		}
		break;
	case SCHED_MEDIC_ELOF_FAIL:
		{
			// assassin is unable to move to a position that allows him to attack the enemy.
			if ( HasConditions( bits_COND_CAN_RANGE_ATTACK1 ) && OccupySlot( bits_SLOTS_FGRUNT_ENGAGE ) )
			{
				return GetScheduleOfType( SCHED_RANGE_ATTACK1 );
			}
			else
			{
				return GetScheduleOfType ( SCHED_FAIL );
			}
		}
		break;
	case SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE:
		{
			return &slMedicEstablishLineOfFire[ 0 ];
		}
		break;
	case SCHED_RANGE_ATTACK1:
		{
			// randomly stand or crouch
			if (RANDOM_LONG(0,9) == 0)
				m_fStanding = RANDOM_LONG(0,1);
		 
			if (m_fStanding)
				return &slMedicRangeAttack1B[ 0 ];
			else
				return &slMedicRangeAttack1A[ 0 ];
		}
	case SCHED_RANGE_ATTACK2:
		{
			return &slMedicRangeAttack2[ 0 ];
		}
	case SCHED_COMBAT_FACE:
		{
			return &slMedicCombatFace[ 0 ];
		}
	case SCHED_MEDIC_WAIT_FACE_ENEMY:
		{
			return &slMedicWaitInCover[ 0 ];
		}
	case SCHED_MEDIC_SWEEP:
		{
			return &slMedicSweep[ 0 ];
		}
	case SCHED_MEDIC_COVER_AND_RELOAD:
		{
			return &slMedicHideReload[ 0 ];
		}
	case SCHED_MEDIC_FOUND_ENEMY:
		{
			return &slMedicFoundEnemy[ 0 ];
		}
	case SCHED_VICTORY_DANCE:
		{
			if ( InSquad() )
			{
				if ( !IsLeader() )
				{
					return &slMedicFail[ 0 ];
				}
			}
			if ( IsFollowing() )
			{
				return &slMedicFail[ 0 ];
			}

			return &slMedicVictoryDance[ 0 ];
		}
	case SCHED_MEDIC_SUPPRESS:
		{
			if ( m_fFirstEncounter )
			{
				m_fFirstEncounter = FALSE;// after first encounter, leader won't issue handsigns anymore when he has a new enemy
				return &slMedicSignalSuppress[ 0 ];
			}
			else
			{
				return &slMedicSuppress[ 0 ];
			}
		}
	case SCHED_FAIL:
		{
			if ( m_hEnemy != NULL )
			{
				// grunt has an enemy, so pick a different default fail schedule most likely to help recover.
				return &slMedicCombatFail[ 0 ];
			}

			return &slMedicFail[ 0 ];
		}
	case SCHED_MEDIC_REPEL:
		{
			if (pev->velocity.z > -128)
				pev->velocity.z -= 32;
			return &slMedicRepel[ 0 ];
		}
	case SCHED_MEDIC_REPEL_ATTACK:
		{
			if (pev->velocity.z > -128)
				pev->velocity.z -= 32;
			return &slMedicRepelAttack[ 0 ];
		}
	case SCHED_MEDIC_REPEL_LAND:
		{
			return &slMedicRepelLand[ 0 ];
		}
	default:
		{
			return CRCAllyMonster :: GetScheduleOfType ( Type );
		}
	}
}
//=========================================================
// SetActivity 
//=========================================================
void CMedic :: SetActivity ( Activity NewActivity )
{
	int	iSequence = ACTIVITY_NOT_AVAILABLE;
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	switch ( NewActivity)
	{
	case ACT_RANGE_ATTACK1:
		if ( m_fStanding )
		{
			// get aimable sequence
			iSequence = LookupSequence( "standing_mp5" );
		}
		else
		{
			// get crouching shoot
			iSequence = LookupSequence( "crouching_mp5" );
		}
		break;
	case ACT_RANGE_ATTACK2:
		// grunt is going to a secondary long range attack. This may be a thrown 
		// grenade or fired grenade, we must determine which and pick proper sequence
		// get toss anim
		iSequence = LookupSequence( "throwgrenade" );
		break;
	case ACT_RUN:
		if ( pev->health <= MEDIC_LIMP_HEALTH )
		{
			// limp!
			iSequence = LookupActivity ( ACT_RUN_HURT );
		}
		else
		{
			iSequence = LookupActivity ( NewActivity );
		}
		break;
	case ACT_WALK:
		if ( pev->health <= MEDIC_LIMP_HEALTH )
		{
			// limp!
			iSequence = LookupActivity ( ACT_WALK_HURT );
		}
		else
		{
			iSequence = LookupActivity ( NewActivity );
		}
		break;
	case ACT_IDLE:
		if ( m_MonsterState == MONSTERSTATE_COMBAT )
		{
			NewActivity = ACT_IDLE_ANGRY;
		}
		iSequence = LookupActivity ( NewActivity );
		break;
	default:
		iSequence = LookupActivity ( NewActivity );
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
	} else {
		// Not available try to get default anim
		ALERT(at_console, "%s has no sequence for act:%d\n", STRING(pev->classname), NewActivity);
		pev->sequence = 0;	// Set to the reset anim (if it's there)
	}
}

//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *CMedic::GetSchedule(void) {
	// clear old sentence
	m_iSentence = -1;

	// flying? If PRONE, barnacle has me. IF not, it's assumed I am rapelling. 
	if (pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE) {
		if (pev->flags & FL_ONGROUND) {
			// just landed
			pev->movetype = MOVETYPE_STEP;
			return GetScheduleOfType(SCHED_MEDIC_REPEL_LAND);
		}
		else {
			// repel down a rope, 
			if (m_MonsterState == MONSTERSTATE_COMBAT)
				return GetScheduleOfType(SCHED_MEDIC_REPEL_ATTACK);
			else
				return GetScheduleOfType(SCHED_MEDIC_REPEL);
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

	switch( m_MonsterState ) {
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
							return GetScheduleOfType(SCHED_MEDIC_SUPPRESS);
						} else {
							return GetScheduleOfType(SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE);
						}
					} else {
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
							return GetScheduleOfType(SCHED_MEDIC_SUPPRESS);
						} else {
							return GetScheduleOfType(SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE);
						}
					}
				}
			} else if (HasConditions(bits_COND_NO_AMMO_LOADED)) { // no ammo
				//!!!KELLY - this individual just realized he's out of bullet ammo. 
				// He's going to try to find cover to run to and reload, but rarely, if 
				// none is available, he'll drop and reload in the open here. 
				return GetScheduleOfType(SCHED_MEDIC_COVER_AND_RELOAD);
			} else if (HasConditions(bits_COND_LIGHT_DAMAGE)) { // damaged just a little
				if (RANDOM_LONG(0, 99) <= 90 && m_hEnemy != NULL) {
					if (FOkToSpeak()) {
						//SENTENCEG_PlayRndSz( ENT(pev), "HG_COVER", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
						m_iSentence = MEDIC_SENT_COVER;
						//JustSpoke();
					}
					// only try to take cover if we actually have an enemy!
					return GetScheduleOfType(SCHED_TAKE_COVER_FROM_ENEMY);
				} else {
					return GetScheduleOfType(SCHED_SMALL_FLINCH);
				}
			} else if (HasConditions(bits_COND_CAN_MELEE_ATTACK1)) { // can kick
				return GetScheduleOfType(SCHED_MELEE_ATTACK1);
			} else if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) ) {
				if (InSquad()) {
					// if the enemy has eluded the squad and a squad member has just located the enemy
					// and the enemy does not see the squad member, issue a call to the squad to waste a 
					// little time and give the player a chance to turn.
					if (MySquadLeader()->m_fEnemyEluded && !HasConditions(bits_COND_ENEMY_FACING_ME)) {
						MySquadLeader()->m_fEnemyEluded = FALSE;
						return GetScheduleOfType(SCHED_MEDIC_FOUND_ENEMY);
					}
				}

				if (OccupySlot(bits_SLOTS_FGRUNT_ENGAGE)) {
					// try to take an available ENGAGE slot
					return GetScheduleOfType(SCHED_RANGE_ATTACK1);
				} else if (HasConditions(bits_COND_CAN_RANGE_ATTACK2) && OccupySlot(bits_SLOTS_FGRUNT_GRENADE)) {
					// throw a grenade if can and no engage slots are available
					return GetScheduleOfType(SCHED_RANGE_ATTACK2);
				} else {
					// hide!
					return GetScheduleOfType(SCHED_TAKE_COVER_FROM_ENEMY);
				}
			} else if ( HasConditions( bits_COND_ENEMY_OCCLUDED ) ) { // can't see enemy
				if ( HasConditions( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_FGRUNT_GRENADE ) ) {
					//!!!KELLY - this grunt is about to throw or fire a grenade at the player. Great place for "fire in the hole"  "frag out" etc
					if (FOkToSpeak()) {
						SENTENCEG_PlayRndSz( ENT(pev), "FG_THROW", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
						JustSpoke();
					}

					return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
				} else if ( OccupySlot( bits_SLOTS_FGRUNT_ENGAGE ) ) {
					return GetScheduleOfType( SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE );
				} else {
					//!!!KELLY - grunt is going to stay put for a couple seconds to see if
					// the enemy wanders back out into the open, or approaches the
					// grunt's covered position. Good place for a taunt, I guess?
					if (FOkToSpeak() && RANDOM_LONG(0,1)) {
						SENTENCEG_PlayRndSz( ENT(pev), "FG_TAUNT", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
					}

					return GetScheduleOfType( SCHED_STANDOFF );
				}
			}
			
			if ( HasConditions( bits_COND_SEE_ENEMY ) && !HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) ) {
				return GetScheduleOfType ( SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE );
			}
		}
		break;
		case MONSTERSTATE_ALERT:	
		case MONSTERSTATE_IDLE:
			if ( HasConditions ( bits_COND_NO_AMMO_LOADED ) ) {
				return GetScheduleOfType ( SCHED_RELOAD );
			}

			if ( m_hEnemy == NULL && IsFollowing() ) {
				// If I'm already close enough to my target
				if ( TargetDistance() <= 128 ) {
					if ( CanHeal() )	// Heal opportunistically
						return slMedicHeal;
					if ( HasConditions( bits_COND_CLIENT_PUSH ) )	// Player wants me to move
						return GetScheduleOfType( SCHED_MOVE_AWAY_FOLLOW );
				}

				if ( !m_hTargetEnt->IsAlive() ) {
					// UNDONE: Comment about the recently dead player here?
					StopFollowing( FALSE );
					break;
				} else {
					if ( HasConditions( bits_COND_CLIENT_PUSH ) ) {
						return GetScheduleOfType( SCHED_MOVE_AWAY_FOLLOW );
					}

					return GetScheduleOfType( SCHED_TARGET_FACE );
				}
			}

			if ( HasConditions( bits_COND_CLIENT_PUSH ) ) {
				return GetScheduleOfType( SCHED_MOVE_AWAY );
			}

		// try to say something about smells
		TrySmellTalk();
		break;
	}
	
	return CRCAllyMonster :: GetSchedule();
}

//=========================================================
// CanHeal
//=========================================================
BOOL CMedic::CanHeal( void ) { 
	ALERT(at_console, "Heal amount is %f\n", m_flHealAnount );
	if ( m_flHealAnount <= 0 ) {
		if ( !m_fDepleteLine ) {
			PlaySentence( "MG_NOTHEAL", 2, VOL_NORM, ATTN_IDLE );
			m_fDepleteLine = TRUE;
		}

		return FALSE;
	}

	if ( (m_healTime > UTIL_GlobalTimeBase()) || (m_hTargetEnt == NULL) || (m_hTargetEnt->pev->health > (m_hTargetEnt->pev->max_health * 0.9)) ) {
		return FALSE;
	}
	return TRUE;
}

//=========================================================
// Heal
//=========================================================
void CMedic::Heal( void ) {
	if ( !CanHeal() )
		return;

	m_flHealAnount -= (m_hTargetEnt->pev->max_health - m_hTargetEnt->pev->health);
	Vector target = m_hTargetEnt->pev->origin - pev->origin;
	if ( target.Length() > 100 )
		return;

	m_hTargetEnt->TakeHealth( m_hTargetEnt->pev->max_health - m_hTargetEnt->pev->health, DMG_GENERIC );
}

//=========================================================
// CMedicRepel - when triggered, spawns a
// repelling down a line.
//=========================================================
LINK_ENTITY_TO_CLASS( monster_medic_ally_repel, CMedicRepel );

void CMedicRepel::Spawn( void )
{
	Precache( );
	pev->solid = SOLID_NOT;

	SetUse(&CMedicRepel :: RepelUse );
}

void CMedicRepel::Precache( void )
{
	UTIL_PrecacheOther( "monster_human_medic_ally" );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/rope.spr" );
}

void CMedicRepel::RepelUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin + Vector( 0, 0, -4096.0), dont_ignore_monsters, ENT(pev), &tr);

	CBaseEntity *pEntity = Create( "monster_human_medic_ally", pev->origin, pev->angles );
	CBaseMonster *pGrunt = pEntity->MyMonsterPointer( );
	pGrunt->pev->movetype = MOVETYPE_FLY;
	pGrunt->pev->velocity = Vector( 0, 0, RANDOM_FLOAT( -196, -128 ) );
	pGrunt->SetActivity( ACT_GLIDE );
	pGrunt->m_vecLastPosition = tr.vecEndPos;

	CBeam *pBeam = CBeam::BeamCreate( "sprites/rope.spr", 10 );
	pBeam->PointEntInit( pev->origin + Vector(0,0,112), pGrunt->entindex() );
	pBeam->SetFlags( BEAM_FSOLID );
	pBeam->SetColor( 255, 255, 255 );
	pBeam->SetThink(&CBeam:: SUB_Remove );
	pBeam->pev->nextthink = UTIL_GlobalTimeBase() + -4096.0 * tr.flFraction / pGrunt->pev->velocity.z + 0.5;

	UTIL_Remove( this );
}