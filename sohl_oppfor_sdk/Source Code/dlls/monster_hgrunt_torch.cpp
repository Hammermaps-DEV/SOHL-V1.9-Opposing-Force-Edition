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
// NPC: Human Grunt Torch Ally
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
#include	"explode.h"
#include	"proj_grenade.h"
#include	"monster_hgrunt_torch.h"

//=========================================================
// Monster's define
//=========================================================
#define	TORCH_CLIP_SIZE_9MM			17
#define	TORCH_CLIP_SIZE_DEAGLE		7

#define FGRUNT_LIMP_HEALTH			20

#define TORCH_EAGLE					1
#define TORCH_PISTOL				2
#define TORCH_BLOWTORCH				3

#define FGRUNT_MEDIC_WAIT			5

// Weapon group
#define GUN_GROUP					2
#define GUN_EAGLE					0
#define GUN_TORCH					1
#define GUN_NONE					2

//=========================================================
// monster-specific conditions
//=========================================================
#define bits_COND_TORCH_NOFIRE	( bits_COND_SPECIAL1 )

//=========================================================
// monster-specific tasks
//=========================================================
enum  {
	TASK_TORCH_FACE_TOSS_DIR = LAST_TALKMONSTER_TASK + 1,
	TASK_TORCH_CHECK_FIRE,
	TASK_TORCH_ALLY_FIND_MEDIC
};

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		TORCH_AE_RELOAD			( 2 )
#define		TORCH_AE_KICK			( 3 )
#define		TORCH_AE_BURST1			( 4 )
#define		TORCH_AE_BURST2			( 5 ) 
#define		TORCH_AE_BURST3			( 6 ) 
#define		TORCH_AE_GREN_TOSS		( 7 )
#define		TORCH_AE_GREN_LAUNCH	( 8 )
#define		TORCH_AE_GREN_DROP		( 9 )
#define		TORCH_AE_CAUGHT_ENEMY	( 10) // grunt established sight with an enemy (player only) that had previously eluded the squad.
#define		TORCH_AE_DROP_GUN		( 11) // grunt (probably dead) is dropping his mp5.
#define		TORCH_AE_SHOWGUN		( 17)
#define		TORCH_AE_SHOWTORCH		( 18)
#define		TORCH_AE_HIDETORCH		( 19)
#define		TORCH_AE_ONGAS			( 20)
#define		TORCH_AE_OFFGAS			( 21)

//=========================================================
// monster-specific schedule types
//=========================================================
enum {
	SCHED_TORCH_SUPPRESS = LAST_TALKMONSTER_SCHEDULE + 1,
	SCHED_TORCH_ESTABLISH_LINE_OF_FIRE,// move to a location to set up an attack against the enemy. (usually when a friendly is in the way).
	SCHED_TORCH_COVER_AND_RELOAD,
	SCHED_TORCH_SWEEP,
	SCHED_TORCH_FOUND_ENEMY,
	SCHED_TORCH_REPEL,
	SCHED_TORCH_REPEL_ATTACK,
	SCHED_TORCH_REPEL_LAND,
	SCHED_TORCH_WAIT_FACE_ENEMY,
	SCHED_TORCH_TAKECOVER_FAILED,// special schedule type that forces analysis of conditions and picks the best possible schedule to recover from this type of failure.
	SCHED_TORCH_ELOF_FAIL,
};

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS( monster_human_torch_ally, CTorch );

TYPEDESCRIPTION	CTorch::m_SaveData[] = 
{
	DEFINE_FIELD( CTorch, m_pBeam, FIELD_CLASSPTR ),
	DEFINE_FIELD( CTorch, m_flNextGrenadeCheck, FIELD_TIME ),
	DEFINE_FIELD( CTorch, m_flNextPainTime, FIELD_TIME ),
	DEFINE_FIELD( CTorch, m_vecTossVelocity, FIELD_VECTOR ),
	DEFINE_FIELD( CTorch, m_fThrowGrenade, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTorch, m_fStanding, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTorch, m_fFirstEncounter, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTorch, m_cClipSize, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CTorch, CRCAllyMonster );

//=========================================================
// Monster's Sentences
//=========================================================
const char *CTorch::pGruntSentences[] = 
{
	"FG_GREN", // grenade scared grunt
	"FG_ALERT", // sees player
	"FG_MONSTER", // sees monster
	"FG_COVER", // running to cover
	"FG_THROW", // about to throw grenade
	"FG_CHARGE",  // running out to get the enemy
	"FG_TAUNT", // say rude things
};

enum
{
	TORCH_SENT_NONE = -1,
	TORCH_SENT_GREN = 0,
	TORCH_SENT_ALERT,
	TORCH_SENT_MONSTER,
	TORCH_SENT_COVER,
	TORCH_SENT_THROW,
	TORCH_SENT_CHARGE,
	TORCH_SENT_TAUNT,
} TORCH_SENTENCE_TYPES;

//=========================================================
// KeyValue
// !!! netname entvar field is used in squadmonster for groupname!!!
//=========================================================
void CTorch::KeyValue(KeyValueData *pkvd) {
	if (FStrEq(pkvd->szKeyName, "immortal")) {
		m_fImmortal = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	} else {
		CRCAllyMonster::KeyValue(pkvd);
	}
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Task_t	tlTorchFollow[] =
{
	{ TASK_MOVE_TO_TARGET_RANGE,(float)128		},	// Move within 128 of target ent (client)
	{ TASK_SET_SCHEDULE,		(float)SCHED_TARGET_FACE },
};

Schedule_t	slTorchFollow[] =
{
	{
		tlTorchFollow,
		HL_ARRAYSIZE ( tlTorchFollow ),
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND |
		bits_COND_PROVOKED,
		bits_SOUND_DANGER,
		"Follow"
	},
};
Task_t	tlTorchFaceTarget[] =
{
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_FACE_TARGET,			(float)0		},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_SET_SCHEDULE,		(float)SCHED_TARGET_CHASE },
};

Schedule_t	slTorchFaceTarget[] =
{
	{
		tlTorchFaceTarget,
		HL_ARRAYSIZE ( tlTorchFaceTarget ),
		bits_COND_CLIENT_PUSH	|
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND |
		bits_COND_PROVOKED,
		bits_SOUND_DANGER,
		"FaceTarget"
	},
};


Task_t	tlTorchIdleStand[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT,				(float)2		}, // repick IDLESTAND every two seconds.
	{ TASK_TLK_HEADRESET,		(float)0		}, // reset head position
};

Schedule_t	slTorchIdleStand[] =
{
	{ 
		tlTorchIdleStand,
		HL_ARRAYSIZE ( tlTorchIdleStand ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND	|
		bits_COND_SMELL			|
		bits_COND_PROVOKED,

		bits_SOUND_COMBAT		|// sound flags - change these, and you'll break the talking code.
		//bits_SOUND_PLAYER		|
		//bits_SOUND_WORLD		|
		
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
Task_t	tlTorchFail[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT,				(float)2		},
	{ TASK_WAIT_PVS,			(float)0		},
};

Schedule_t	slTorchFail[] =
{
	{
		tlTorchFail,
		HL_ARRAYSIZE ( tlTorchFail ),
		bits_COND_CAN_RANGE_ATTACK1 |
		bits_COND_CAN_RANGE_ATTACK2 |
		bits_COND_CAN_MELEE_ATTACK1 |
		bits_COND_CAN_MELEE_ATTACK2,
		0,
		"Grunt Fail"
	},
};

//=========================================================
// Grunt Combat Fail
//=========================================================
Task_t	tlTorchCombatFail[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
	{ TASK_WAIT_FACE_ENEMY,		(float)2		},
	{ TASK_WAIT_PVS,			(float)0		},
};

Schedule_t	slTorchCombatFail[] =
{
	{
		tlTorchCombatFail,
		HL_ARRAYSIZE ( tlTorchCombatFail ),
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2,
		0,
		"Grunt Combat Fail"
	},
};

//=========================================================
// Victory dance!
//=========================================================
Task_t	tlTorchVictoryDance[] =
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

Schedule_t	slTorchVictoryDance[] =
{
	{ 
		tlTorchVictoryDance,
		HL_ARRAYSIZE ( tlTorchVictoryDance ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE,
		0,
		"GruntVictoryDance"
	},
};

//=========================================================
// Establish line of fire - move to a position that allows
// the grunt to attack.
//=========================================================
Task_t tlTorchEstablishLineOfFire[] = 
{
	{ TASK_SET_FAIL_SCHEDULE,	(float)SCHED_TORCH_ELOF_FAIL	},
	{ TASK_GET_PATH_TO_ENEMY,	(float)0						},
	{ TASK_RUN_PATH,			(float)0						},
	{ TASK_WAIT_FOR_MOVEMENT,	(float)0						},
};

Schedule_t slTorchEstablishLineOfFire[] =
{
	{ 
		tlTorchEstablishLineOfFire,
		HL_ARRAYSIZE ( tlTorchEstablishLineOfFire ),
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
Task_t	tlTorchFoundEnemy[] =
{
	{ TASK_STOP_MOVING,				0							},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,(float)ACT_SIGNAL1			},
};

Schedule_t	slTorchFoundEnemy[] =
{
	{ 
		tlTorchFoundEnemy,
		HL_ARRAYSIZE ( tlTorchFoundEnemy ), 
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER,
		"GruntFoundEnemy"
	},
};

//=========================================================
// GruntCombatFace Schedule
//=========================================================
Task_t	tlTorchCombatFace1[] =
{
	{ TASK_STOP_MOVING,				0							},
	{ TASK_SET_ACTIVITY,			(float)ACT_IDLE				},
	{ TASK_FACE_ENEMY,				(float)0					},
	{ TASK_WAIT,					(float)1.5					},
	{ TASK_SET_SCHEDULE,			(float)SCHED_TORCH_SWEEP	},
};

Schedule_t	slTorchCombatFace[] =
{
	{ 
		tlTorchCombatFace1,
		HL_ARRAYSIZE ( tlTorchCombatFace1 ), 
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
Task_t	tlTorchSignalSuppress[] =
{
	{ TASK_STOP_MOVING,					0						},
	{ TASK_FACE_IDEAL,					(float)0				},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,	(float)ACT_SIGNAL2		},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_TORCH_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_TORCH_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_TORCH_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_TORCH_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
	{ TASK_FACE_ENEMY,					(float)0				},
	{ TASK_TORCH_CHECK_FIRE,			(float)0				},
	{ TASK_RANGE_ATTACK1,				(float)0				},
};

Schedule_t	slTorchSignalSuppress[] =
{
	{ 
		tlTorchSignalSuppress,
		HL_ARRAYSIZE ( tlTorchSignalSuppress ), 
		bits_COND_ENEMY_DEAD		|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND		|
		bits_COND_TORCH_NOFIRE		|
		bits_COND_NO_AMMO_LOADED,

		bits_SOUND_DANGER,
		"SignalSuppress"
	},
};

Task_t	tlTorchSuppress[] =
{
	{ TASK_STOP_MOVING,			0							},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_TORCH_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_TORCH_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_TORCH_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_TORCH_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
	{ TASK_FACE_ENEMY,			(float)0					},
	{ TASK_TORCH_CHECK_FIRE,	(float)0					},
	{ TASK_RANGE_ATTACK1,		(float)0					},
};

Schedule_t	slTorchSuppress[] =
{
	{ 
		tlTorchSuppress,
		HL_ARRAYSIZE ( tlTorchSuppress ), 
		bits_COND_ENEMY_DEAD		|
		bits_COND_LIGHT_DAMAGE		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_HEAR_SOUND		|
		bits_COND_TORCH_NOFIRE		|
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
Task_t	tlTorchWaitInCover[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_SET_ACTIVITY,			(float)ACT_IDLE				},
	{ TASK_WAIT_FACE_ENEMY,			(float)1					},
};

Schedule_t	slTorchWaitInCover[] =
{
	{ 
		tlTorchWaitInCover,
		HL_ARRAYSIZE ( tlTorchWaitInCover ), 
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
Task_t	tlTorchTakeCover1[] =
{
	{ TASK_STOP_MOVING,				(float)0							},
	{ TASK_SET_FAIL_SCHEDULE,		(float)SCHED_TORCH_TAKECOVER_FAILED	},
	{ TASK_WAIT,					(float)0.2							},
	{ TASK_FIND_COVER_FROM_ENEMY,	(float)0							},
	{ TASK_RUN_PATH,				(float)0							},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0							},
	{ TASK_REMEMBER,				(float)bits_MEMORY_INCOVER			},
	{ TASK_SET_SCHEDULE,			(float)SCHED_TORCH_WAIT_FACE_ENEMY	},
};

Schedule_t	slTorchTakeCover[] =
{
	{ 
		tlTorchTakeCover1,
		HL_ARRAYSIZE ( tlTorchTakeCover1 ), 
		0,
		0,
		"TakeCover"
	},
};

//=========================================================
// drop grenade then run to cover.
//=========================================================
Task_t	tlTorchGrenadeCover1[] =
{
	{ TASK_STOP_MOVING,						(float)0							},
	{ TASK_FIND_COVER_FROM_ENEMY,			(float)99							},
	{ TASK_FIND_FAR_NODE_COVER_FROM_ENEMY,	(float)384							},
	{ TASK_PLAY_SEQUENCE,					(float)ACT_SPECIAL_ATTACK1			},
	{ TASK_CLEAR_MOVE_WAIT,					(float)0							},
	{ TASK_RUN_PATH,						(float)0							},
	{ TASK_WAIT_FOR_MOVEMENT,				(float)0							},
	{ TASK_SET_SCHEDULE,					(float)SCHED_TORCH_WAIT_FACE_ENEMY	},
};

Schedule_t	slTorchGrenadeCover[] =
{
	{ 
		tlTorchGrenadeCover1,
		HL_ARRAYSIZE ( tlTorchGrenadeCover1 ), 
		0,
		0,
		"GrenadeCover"
	},
};


//=========================================================
// drop grenade then run to cover.
//=========================================================
Task_t	tlTorchTossGrenadeCover1[] =
{
	{ TASK_FACE_ENEMY,						(float)0							},
	{ TASK_RANGE_ATTACK2, 					(float)0							},
	{ TASK_SET_SCHEDULE,					(float)SCHED_TAKE_COVER_FROM_ENEMY	},
};

Schedule_t	slTorchTossGrenadeCover[] =
{
	{ 
		tlTorchTossGrenadeCover1,
		HL_ARRAYSIZE ( tlTorchTossGrenadeCover1 ), 
		0,
		0,
		"TossGrenadeCover"
	},
};

//=========================================================
// hide from the loudest sound source (to run from grenade)
//=========================================================
Task_t	tlTorchTakeCoverFromBestSound[] =
{
	{ TASK_SET_FAIL_SCHEDULE,			(float)SCHED_COWER			},// duck and cover if cannot move from explosion
	{ TASK_STOP_MOVING,					(float)0					},
	{ TASK_FIND_COVER_FROM_BEST_SOUND,	(float)0					},
	{ TASK_RUN_PATH,					(float)0					},
	{ TASK_WAIT_FOR_MOVEMENT,			(float)0					},
	{ TASK_REMEMBER,					(float)bits_MEMORY_INCOVER	},
	{ TASK_TURN_LEFT,					(float)179					},
};

Schedule_t	slTorchTakeCoverFromBestSound[] =
{
	{ 
		tlTorchTakeCoverFromBestSound,
		HL_ARRAYSIZE ( tlTorchTakeCoverFromBestSound ), 
		0,
		0,
		"GruntTakeCoverFromBestSound"
	},
};

//=========================================================
// Grunt reload schedule
//=========================================================
Task_t	tlTorchHideReload[] =
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

Schedule_t slTorchHideReload[] = 
{
	{
		tlTorchHideReload,
		HL_ARRAYSIZE ( tlTorchHideReload ),
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND,

		bits_SOUND_DANGER,
		"GruntHideReload"
	}
};

//=========================================================
// Do a turning sweep of the area
//=========================================================
Task_t	tlTorchSweep[] =
{
	{ TASK_TURN_LEFT,			(float)179	},
	{ TASK_WAIT,				(float)1	},
	{ TASK_TURN_LEFT,			(float)179	},
	{ TASK_WAIT,				(float)1	},
};

Schedule_t	slTorchSweep[] =
{
	{ 
		tlTorchSweep,
		HL_ARRAYSIZE ( tlTorchSweep ), 
		
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_CAN_RANGE_ATTACK1	|
		bits_COND_CAN_RANGE_ATTACK2	|
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
Task_t	tlTorchRangeAttack1A[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,		(float)ACT_CROUCH },
	{ TASK_TORCH_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_TORCH_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_TORCH_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_TORCH_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
};

Schedule_t	slTorchRangeAttack1A[] =
{
	{ 
		tlTorchRangeAttack1A,
		HL_ARRAYSIZE ( tlTorchRangeAttack1A ), 
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_ENEMY_OCCLUDED	|
		bits_COND_HEAR_SOUND		|
		bits_COND_TORCH_NOFIRE		|
		bits_COND_NO_AMMO_LOADED,
		
		bits_SOUND_DANGER,
		"Range Attack1A"
	},
};


//=========================================================
// primary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlTorchRangeAttack1B[] =
{
	{ TASK_STOP_MOVING,				(float)0		},
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY,(float)ACT_IDLE_ANGRY  },
	{ TASK_TORCH_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_TORCH_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_TORCH_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_TORCH_CHECK_FIRE,	(float)0		},
	{ TASK_RANGE_ATTACK1,		(float)0		},
};

Schedule_t	slTorchRangeAttack1B[] =
{
	{ 
		tlTorchRangeAttack1B,
		HL_ARRAYSIZE ( tlTorchRangeAttack1B ), 
		bits_COND_NEW_ENEMY			|
		bits_COND_ENEMY_DEAD		|
		bits_COND_HEAVY_DAMAGE		|
		bits_COND_ENEMY_OCCLUDED	|
		bits_COND_NO_AMMO_LOADED	|
		bits_COND_TORCH_NOFIRE		|
		bits_COND_HEAR_SOUND,
		
		bits_SOUND_DANGER,
		"Range Attack1B"
	},
};

//=========================================================
// secondary range attack. Overriden because base class stops attacking when the enemy is occluded.
// grunt's grenade toss requires the enemy be occluded.
//=========================================================
Task_t	tlTorchRangeAttack2[] =
{
	{ TASK_STOP_MOVING,				(float)0					},
	{ TASK_TORCH_FACE_TOSS_DIR,		(float)0					},
	{ TASK_PLAY_SEQUENCE,			(float)ACT_RANGE_ATTACK2	},
	{ TASK_SET_SCHEDULE,			(float)SCHED_TORCH_WAIT_FACE_ENEMY	},// don't run immediately after throwing grenade.
};

Schedule_t	slTorchRangeAttack2[] =
{
	{ 
		tlTorchRangeAttack2,
		HL_ARRAYSIZE ( tlTorchRangeAttack2 ), 
		0,
		0,
		"RangeAttack2"
	},
};


//=========================================================
// repel 
//=========================================================
Task_t	tlTorchRepel[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_IDEAL,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_GLIDE 	},
};

Schedule_t	slTorchRepel[] =
{
	{ 
		tlTorchRepel,
		HL_ARRAYSIZE ( tlTorchRepel ), 
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
Task_t	tlTorchRepelAttack[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_FACE_ENEMY,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_FLY 	},
};

Schedule_t	slTorchRepelAttack[] =
{
	{ 
		tlTorchRepelAttack,
		HL_ARRAYSIZE ( tlTorchRepelAttack ), 
		bits_COND_ENEMY_OCCLUDED,
		0,
		"Repel Attack"
	},
};

//=========================================================
// repel land
//=========================================================
Task_t	tlTorchRepelLand[] =
{
	{ TASK_STOP_MOVING,			(float)0		},
	{ TASK_PLAY_SEQUENCE,		(float)ACT_LAND	},
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0				},
	{ TASK_RUN_PATH,				(float)0				},
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0				},
	{ TASK_CLEAR_LASTPOSITION,		(float)0				},
};

Schedule_t	slTorchRepelLand[] =
{
	{ 
		tlTorchRepelLand,
		HL_ARRAYSIZE ( tlTorchRepelLand ), 
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
DEFINE_CUSTOM_SCHEDULES( CTorch )
{
	slTorchFollow,
	slTorchFaceTarget,
	slTorchIdleStand,
	slTorchFail,
	slTorchCombatFail,
	slTorchVictoryDance,
	slTorchEstablishLineOfFire,
	slTorchFoundEnemy,
	slTorchCombatFace,
	slTorchSignalSuppress,
	slTorchSuppress,
	slTorchWaitInCover,
	slTorchTakeCover,
	slTorchGrenadeCover,
	slTorchTossGrenadeCover,
	slTorchTakeCoverFromBestSound,
	slTorchHideReload,
	slTorchSweep,
	slTorchRangeAttack1A,
	slTorchRangeAttack1B,
	slTorchRangeAttack2,
	slTorchRepel,
	slTorchRepelAttack,
	slTorchRepelLand,
};


IMPLEMENT_CUSTOM_SCHEDULES( CTorch, CRCAllyMonster );

//=========================================================
// StartTask
//=========================================================
void CTorch::StartTask(Task_t *pTask) {
	m_iTaskStatus = TASKSTATUS_RUNNING;
	switch (pTask->iTask) {
		case TASK_TORCH_CHECK_FIRE:
			if (!NoFriendlyFire()) {
				SetConditions(bits_COND_TORCH_NOFIRE);
			}
			TaskComplete();
		break;
		case TASK_TORCH_ALLY_FIND_MEDIC:
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
								EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fgrunt/medic.wav", 1, ATTN_NORM, 0, GetVoicePitch());
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
								EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fgrunt/medic.wav", 1, ATTN_NORM, 0, GetVoicePitch());
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
void CTorch :: RunTask( Task_t *pTask ) {
	switch ( pTask->iTask ) {
		case TASK_TORCH_FACE_TOSS_DIR: {
			// project a point along the toss vector and turn to face that point.
			MakeIdealYaw( pev->origin + m_vecTossVelocity * 64 );
			ChangeYaw( pev->yaw_speed );
			if ( FacingIdeal() ){
				m_iTaskStatus = TASKSTATUS_COMPLETE;
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
void CTorch :: GibMonster ( void ) {
	if (!(pev->spawnflags & SF_MONSTER_SPAWNFLAG_1024) && GetBodygroup(2) != 2) {
		Vector	vecGunPos;
		Vector	vecGunAngles;

		GetAttachment(0, vecGunPos, vecGunAngles);

		CBaseEntity *pGun;
		pGun = DropItem("weapon_eagle", vecGunPos, vecGunAngles);
		if (pGun) {
			pGun->pev->velocity = Vector(RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(200, 300));
			pGun->pev->avelocity = Vector(0, RANDOM_FLOAT(200, 400), 0);
		}
	}

	CBaseMonster :: GibMonster();
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CTorch :: SetYawSpeed ( void ) {
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
// PrescheduleThink - this function runs after conditions
// are collected and before scheduling code is run.
//=========================================================
void CTorch::PrescheduleThink(void) {
	if (m_pBeam) {
		UpdateGas();
	}

	CRCAllyMonster::PrescheduleThink();
}

//=========================================================
// CheckRangeAttack1 - overridden for HGrunt, cause 
// FCanCheckAttacks() doesn't disqualify all attacks based
// on whether or not the enemy is occluded because unlike
// the base class, the HGrunt can attack when the enemy is
// occluded (throw grenade over wall, etc). We must 
// disqualify the machine gun attack if the enemy is occluded.
//=========================================================
BOOL CTorch :: CheckRangeAttack1 ( float flDot, float flDist ) {
	if(GetBodygroup( GUN_GROUP ) != GUN_TORCH) {
		return CRCAllyMonster::CheckRangeAttack1(flDot, flDist);
	}

	return FALSE;
}

//=========================================================
// UpdateGas 
//=========================================================
void CTorch::UpdateGas( void ) {
	TraceResult tr;
	Vector posGun, angleGun;
	Vector vecEndPos;

	if ( m_pBeam ) {
		UTIL_MakeVectors( pev->angles );
		GetAttachment( 2, posGun, angleGun );
			
		Vector vecEnd = (gpGlobals->v_forward * 5) + posGun;
		UTIL_TraceLine( posGun, vecEnd, dont_ignore_monsters, edict(), &tr );

		if ( tr.flFraction != 1.0 ) {
			m_pBeam->DoSparks( tr.vecEndPos, posGun );
			UTIL_DecalTrace(&tr, DECAL_BIGSHOT1 + RANDOM_LONG(0,4));

			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
				WRITE_BYTE( TE_STREAK_SPLASH );
				WRITE_COORD( tr.vecEndPos.x );		// origin
				WRITE_COORD( tr.vecEndPos.y );
				WRITE_COORD( tr.vecEndPos.z );
				WRITE_COORD( tr.vecPlaneNormal.x );	// direction
				WRITE_COORD( tr.vecPlaneNormal.y );
				WRITE_COORD( tr.vecPlaneNormal.z );
				WRITE_BYTE( 10 );	// Streak color 6
				WRITE_SHORT( 60 );	// count
				WRITE_SHORT( 25 );
				WRITE_SHORT( 50 );	// Random velocity modifier
			MESSAGE_END();
		}
		
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_DLIGHT );
			WRITE_COORD( posGun.x );		// origin
			WRITE_COORD( posGun.y );
			WRITE_COORD( posGun.z );
			WRITE_BYTE( RANDOM_LONG(4, 16) );	// radius
			WRITE_BYTE( 251 );	// R
			WRITE_BYTE( 68 );	// G
			WRITE_BYTE( 36 );	// B
			WRITE_BYTE( 1 );	// life * 10
			WRITE_BYTE( 0 ); // decay
		MESSAGE_END();

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_ELIGHT );
			WRITE_SHORT( entindex( ) + 0x1000 * 3 );		// entity, attachment
			WRITE_COORD( posGun.x );		// origin
			WRITE_COORD( posGun.y );
			WRITE_COORD( posGun.z );
			WRITE_COORD( RANDOM_LONG(8, 12) );	// radius
			WRITE_BYTE( 251 );	// R
			WRITE_BYTE( 68 );	// G
			WRITE_BYTE( 36 );	// B
			WRITE_BYTE( 1 );	// life * 10
			WRITE_COORD( 0 ); // decay 
		MESSAGE_END();
	}
}

//=========================================================
// MakeGas 
//=========================================================
void CTorch::MakeGas( void ) {
	Vector posGun, angleGun;
	TraceResult tr;
	Vector vecEndPos;

	UTIL_MakeVectors( pev->angles );
	m_pBeam = CBeam::BeamCreate( g_pModelNameLaser, 7 );

	if ( m_pBeam ) {
		GetAttachment( 4, posGun, angleGun );
		GetAttachment( 3, posGun, angleGun );
		UTIL_Sparks( posGun );
		Vector vecEnd = (gpGlobals->v_forward * 5) + posGun;
		UTIL_TraceLine( posGun, vecEnd, dont_ignore_monsters, edict(), &tr );
		
		m_pBeam->EntsInit( entindex(), entindex() );
		m_pBeam->SetColor( 24, 121, 239 );
		m_pBeam->SetBrightness( 190 );
		m_pBeam->SetScrollRate( 20 );
		m_pBeam->SetStartAttachment( 4 );
		m_pBeam->SetEndAttachment( 3 );
		m_pBeam->DoSparks( tr.vecEndPos, posGun );
		m_pBeam->SetFlags( BEAM_FSHADEIN );
		m_pBeam->pev->spawnflags = SF_BEAM_SPARKSTART | SF_BEAM_TEMPORARY;
		UTIL_Sparks( tr.vecEndPos );
	}
	
	return;
}

//=========================================================
// KillGas 
//=========================================================
void CTorch :: KillGas( void ) {
	if ( m_pBeam ) {
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}

	return;
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CTorch :: HandleAnimEvent( MonsterEvent_t *pEvent ) {
	Vector	vecShootDir;
	Vector	vecShootOrigin;

	switch( pEvent->event ) {
		case TORCH_AE_SHOWTORCH:
			pev->body = GUN_NONE;
			pev->body = GUN_TORCH;
		break;
		case TORCH_AE_SHOWGUN:
			pev->body = GUN_NONE;
			pev->body = GUN_EAGLE;
		break;
		case TORCH_AE_HIDETORCH:
			pev->body = GUN_NONE;
		break;
		case TORCH_AE_ONGAS:
			MakeGas ();
			UpdateGas ();
		break;
		case TORCH_AE_OFFGAS:
			KillGas ();
		break;
		case TORCH_AE_DROP_GUN: {
			if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_1024) break; //LRC

			Vector	vecGunPos;
			Vector	vecGunAngles;

			GetAttachment( 0, vecGunPos, vecGunAngles );

			// switch to body group with no gun.
			SetBodygroup( GUN_GROUP, GUN_NONE );

			// now spawn a gun.
			if (FBitSet( pev->weapons, TORCH_EAGLE )) {
				DropItem( "weapon_eagle", vecGunPos, vecGunAngles );
			}
		}
		break;
		case TORCH_AE_RELOAD:
			EMIT_SOUND( ENT(pev), CHAN_WEAPON, "hgrunt/gr_reload1.wav", 1, ATTN_NORM );
			m_cAmmoLoaded = m_cClipSize;
			ClearConditions(bits_COND_NO_AMMO_LOADED);
		break;
		case TORCH_AE_GREN_TOSS: {
			UTIL_MakeVectors( pev->angles );
			CGrenade::ShootTimed( pev, GetGunPosition(), m_vecTossVelocity, RANDOM_FLOAT(1.5, 3));

			m_fThrowGrenade = FALSE;
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 6;// wait six seconds before even looking again to see if a grenade can be thrown.
			// !!!LATER - when in a group, only try to throw grenade if ordered.
		}
		break;
		case TORCH_AE_GREN_DROP: {
			Vector	vecGunPos, vecGunAngles;
			GetAttachment(0, vecGunPos, vecGunAngles);

			// switch to body group with no gun.
			SetBodygroup(GUN_GROUP, GUN_NONE);

			// now spawn a gun.
			if (pev->weapons == TORCH_PISTOL) {
				DropItem("weapon_9mmhandgun", vecGunPos, vecGunAngles);
			} else if (pev->weapons == TORCH_EAGLE) {
				DropItem("weapon_eagle", vecGunPos, vecGunAngles);
			}
		}
		break;
		case TORCH_AE_BURST1:
		case TORCH_AE_BURST2:
		case TORCH_AE_BURST3: {
			if (pev->weapons == TORCH_EAGLE)
				ShootDesertEagle();
			else
				ShootGlock();
		}
		case TORCH_AE_KICK: {
			CBaseEntity *pHurt = Kick();
			if ( pHurt ) {
				// SOUND HERE!
				UTIL_MakeVectors( pev->angles );
				pHurt->pev->punchangle.x = 20;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * 150 + gpGlobals->v_up * 80;
				pHurt->TakeDamage( pev, pev, gSkillData.torchDmgKick, DMG_CLUB );
			}
		}
		break;
		case TORCH_AE_CAUGHT_ENEMY: {
			if ( FOkToSpeak() ) {
				SENTENCEG_PlayRndSz(ENT(pev), "FG_ALERT", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
				JustSpoke();
			}
		}
		default:
			CRCAllyMonster::HandleAnimEvent( pEvent );
		break;
	}
}

//=========================================================
// Spawn
//=========================================================
void CTorch :: Spawn() {
	Precache( );

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/hgrunt_torch.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_RED;

	if (pev->health == 0) //LRC
		pev->health = gSkillData.torchHealth;

	pev->view_ofs		= Vector ( 0, 0, 50 );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_MonsterState		= MONSTERSTATE_NONE;

	m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 1;
	m_flNextPainTime	= UTIL_GlobalTimeBase();

	m_flDebug = false; //Debug Massages

	m_flHitgroupHead = gSkillData.fgruntHead;
	m_flHitgroupChest = gSkillData.fgruntChest;
	m_flHitgroupStomach = gSkillData.fgruntStomach;
	m_flHitgroupArm = gSkillData.fgruntArm;
	m_flHitgroupLeg = gSkillData.fgruntLeg;

	m_afCapability		= bits_CAP_HEAR | bits_CAP_SQUAD | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;

	m_fEnemyEluded		= FALSE;
	m_fFirstEncounter	= TRUE;// this is true when the grunt spawns, because he hasn't encountered an enemy yet.

	m_flLinkToggle = 0;
	m_cClipSize = TORCH_CLIP_SIZE_DEAGLE;
	m_HackedGunPos = Vector ( 0, 0, 55 );

	if (!pev->weapons) {
		pev->weapons = TORCH_EAGLE;
	}

	if (pev->weapons == TORCH_PISTOL) {
		SetBodygroup(GUN_GROUP, GUN_EAGLE);
		m_cClipSize = TORCH_CLIP_SIZE_9MM;
	} else if (pev->weapons == TORCH_EAGLE) {
		SetBodygroup(GUN_GROUP, GUN_EAGLE);
	} else if (pev->weapons == TORCH_BLOWTORCH) {
		SetBodygroup(GUN_GROUP, GUN_TORCH);
	}

	m_cAmmoLoaded = m_cClipSize;

	MonsterInit();
	StartMonster();
	SetUse( &CTorch :: FollowerUse );
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CTorch :: Precache() {
	CRCAllyMonster::Precache();

	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/hgrunt_torch.mdl");

	PRECACHE_SOUND("fgrunt/torch_cut_loop.wav");
	PRECACHE_SOUND("fgrunt/torch_light.wav");
}

//=========================================================
// JustSpoke
//=========================================================
void CTorch::JustSpoke(void) {
	CRCAllyMonster::g_talkWaitTime = UTIL_GlobalTimeBase() + RANDOM_FLOAT(1.5, 2.0);
	m_iSentence = TORCH_SENT_NONE;
}

//=========================================================
// TraceAttack - make sure we're not taking it in the helmet
//=========================================================
void CTorch :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
		return;
	}

	if (pev->takedamage) {
		if (m_fImmortal)
			flDamage = 0;

		if (IsAlive() && RANDOM_LONG(0, 4) <= 2) { PainSound(); }
		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_64) {
			CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
			if (pEnt->IsPlayer()) { CBaseMonster::TraceAttack(pevAttacker, 0, vecDir, ptr, bitsDamageType); }
			if (pevAttacker->owner) {
				pEnt = CBaseEntity::Instance(pevAttacker->owner);
				if (pEnt->IsPlayer()) { CBaseMonster::TraceAttack(pevAttacker, 0, vecDir, ptr, bitsDamageType); }
			}
		}

		switch (ptr->iHitgroup) {
			case HITGROUP_HEAD_HELMET_GT:
			case HITGROUP_HEAD:
				if (m_flDebug)
					ALERT(at_console, "%s:TraceAttack:HITGROUP_HEAD\n", STRING(pev->classname));
				if ((GetBodygroup(1) == 0 || GetBodygroup(1) == 5) &&
					(bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_CLUB))) {
					flDamage -= 20;
					if (flDamage <= 0) {
						UTIL_Ricochet(ptr->vecEndPos, 1.0);
						flDamage = 0.01;
					}
				}
				else {
					flDamage = m_flHitgroupHead*flDamage;
				}
				ptr->iHitgroup = HITGROUP_HEAD;
			break;
			case HITGROUP_CHEST:
				if (m_flDebug)
					ALERT(at_console, "%s:TraceAttack:HITGROUP_CHEST\n", STRING(pev->classname));
				if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST)) {
					flDamage = (m_flHitgroupChest*flDamage) / 2;
				}
			break;
			case HITGROUP_STOMACH:
				if (m_flDebug)
					ALERT(at_console, "%s:TraceAttack:HITGROUP_STOMACH\n", STRING(pev->classname));
				if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST)) {
					flDamage = (m_flHitgroupStomach*flDamage) / 2;
				}
			break;
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
				if (m_flDebug)
					ALERT(at_console, "%s:TraceAttack:HITGROUP_ARM\n", STRING(pev->classname));
				flDamage = m_flHitgroupArm*flDamage;
			break;
			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
				if (m_flDebug)
					ALERT(at_console, "%s:TraceAttack:HITGROUP_LEG\n", STRING(pev->classname));
				flDamage = m_flHitgroupLeg*flDamage;
			break;
			case HITGROUP_TANK:
				TraceResult tr;
				if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_CLUB)) {
					if (UTIL_PointContents(pev->origin) == CONTENTS_WATER) {
						UTIL_Bubbles(pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100);
					} else {
						Vector position = pev->origin;
						SpawnExplosion(position, 70, 0.3, 80);
						UTIL_ScreenShake(tr.vecEndPos, 25.0, 150.0, 1.0, 750);
					}

					if (UTIL_PointContents(pev->origin) != CONTENTS_WATER) {
						int sparkCount = RANDOM_LONG(1, 5);
						Vector mirpos = UTIL_MirrorPos(pev->origin);

						for (int i = 0; i < sparkCount; i++)
							Create("spark_shower", pev->origin, tr.vecPlaneNormal, NULL);

						if (mirpos != Vector(0, 0, 0))
							for (int i = 0; i < sparkCount; i++)
								Create("spark_shower", mirpos, tr.vecPlaneNormal, NULL);
					}
				}
			break;
		}
	}

	CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}

//=========================================================
// SpawnExplosion
//=========================================================
void CTorch :: SpawnExplosion(Vector center, float randomRange, float time, int magnitude) {
	KeyValueData	kvd; char buf[128];
	CBaseEntity *pExplosion = CBaseEntity::Create("env_explosion", center, g_vecZero, NULL);
	kvd.szKeyName = "iMagnitude";
	kvd.szValue = buf;
	pExplosion->KeyValue(&kvd);
	pExplosion->Spawn();
	pExplosion->SetThink(&CBaseEntity::SUB_CallUseToggle);
	pExplosion->SetNextThink(time);

	RadiusDamage(pev, pev, 100, CLASS_NONE, DMG_BLAST);
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Schedule_t* CTorch :: GetScheduleOfType ( int Type ) {
	Schedule_t *psched;
	switch( Type ) {
		// Hook these to make a looping schedule
		case SCHED_TARGET_FACE: {
			// call base class default so that barney will talk
			// when 'used' 
			psched = CRCAllyMonster::GetScheduleOfType(Type);

			if (psched == slIdleStand)
				return slTorchFaceTarget;	// override this for different target face behavior
			else
				return psched;
		}
		case SCHED_TARGET_CHASE: {
			return slTorchFollow;
		}
		case SCHED_IDLE_STAND: {
			psched = CRCAllyMonster::GetScheduleOfType(Type);

			if (psched == slIdleStand) {
				// just look straight ahead.
				return slTorchIdleStand;
			}
			else
				return psched;	
		}
		case SCHED_TAKE_COVER_FROM_ENEMY: {
			if ( InSquad() ) {
				return &slTorchTakeCover[ 0 ];
			} else {
				if ( RANDOM_LONG(0,1) ) {
					return &slTorchTakeCover[ 0 ];
				} else {
					return &slTorchGrenadeCover[ 0 ];
				}
			}
		}
		case SCHED_TAKE_COVER_FROM_BEST_SOUND: {
			return &slTorchTakeCoverFromBestSound[ 0 ];
		}
		case SCHED_TORCH_TAKECOVER_FAILED: {
			if ( HasConditions( bits_COND_CAN_RANGE_ATTACK1 ) && OccupySlot( bits_SLOTS_FGRUNT_ENGAGE ) ) {
				return GetScheduleOfType( SCHED_RANGE_ATTACK1 );
			}

			return GetScheduleOfType ( SCHED_FAIL );
		}
		break;
		case SCHED_TORCH_ELOF_FAIL: {
			// assassin is unable to move to a position that allows him to attack the enemy.
			if ( HasConditions( bits_COND_CAN_RANGE_ATTACK1 ) && OccupySlot( bits_SLOTS_FGRUNT_ENGAGE ) ) {
				return GetScheduleOfType( SCHED_RANGE_ATTACK1 );
			} else {
				return GetScheduleOfType ( SCHED_FAIL );
			}
		}
		break;
		case SCHED_TORCH_ESTABLISH_LINE_OF_FIRE: {
			return &slTorchEstablishLineOfFire[ 0 ];
		}
		break;
		case SCHED_RANGE_ATTACK1: {
			// randomly stand or crouch
			if (RANDOM_LONG(0,9) == 0)
				m_fStanding = RANDOM_LONG(0,1);
		 
			if (m_fStanding)
				return &slTorchRangeAttack1B[ 0 ];
			else
				return &slTorchRangeAttack1A[ 0 ];
		}
		case SCHED_RANGE_ATTACK2: {
			return &slTorchRangeAttack2[ 0 ];
		}
		case SCHED_COMBAT_FACE: {
			return &slTorchCombatFace[ 0 ];
		}
		case SCHED_TORCH_WAIT_FACE_ENEMY: {
			return &slTorchWaitInCover[ 0 ];
		}
		case SCHED_TORCH_SWEEP: {
			return &slTorchSweep[ 0 ];
		}
		case SCHED_TORCH_COVER_AND_RELOAD: {
			return &slTorchHideReload[ 0 ];
		}
		case SCHED_TORCH_FOUND_ENEMY: {
			return &slTorchFoundEnemy[ 0 ];
		}
		case SCHED_VICTORY_DANCE: {
			if ( InSquad() ) {
				if ( !IsLeader() ) {
					return &slTorchFail[ 0 ];
				}
			}

			if ( IsFollowing() ) {
				return &slTorchFail[ 0 ];
			}

			return &slTorchVictoryDance[ 0 ];
		}
		case SCHED_TORCH_SUPPRESS: {
			if ( m_fFirstEncounter ) {
				m_fFirstEncounter = FALSE;// after first encounter, leader won't issue handsigns anymore when he has a new enemy
				return &slTorchSignalSuppress[ 0 ];
			} else {
				return &slTorchSuppress[ 0 ];
			}
		}
		case SCHED_FAIL: {
			if ( m_hEnemy != NULL ) {
				// grunt has an enemy, so pick a different default fail schedule most likely to help recover.
				return &slTorchCombatFail[ 0 ];
			}

			return &slTorchFail[ 0 ];
		}
		case SCHED_TORCH_REPEL: {
			if (pev->velocity.z > -128)
				pev->velocity.z -= 32;
			return &slTorchRepel[ 0 ];
		}
		case SCHED_TORCH_REPEL_ATTACK: {
			if (pev->velocity.z > -128)
				pev->velocity.z -= 32;
			return &slTorchRepelAttack[ 0 ];
		}
		case SCHED_TORCH_REPEL_LAND: {
			return &slTorchRepelLand[ 0 ];
		}
		default: {
			return CRCAllyMonster :: GetScheduleOfType ( Type );
		}
	}
}
//=========================================================
// SetActivity 
//=========================================================
void CTorch :: SetActivity ( Activity NewActivity ) {
	int	iSequence = ACTIVITY_NOT_AVAILABLE;
	void *pmodel = GET_MODEL_PTR( ENT(pev) );
	switch ( NewActivity) {
		case ACT_RANGE_ATTACK1:
			if ( m_fStanding ) {
				// get aimable sequence
				iSequence = LookupSequence( "standing_mp5" );
			} else {
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
			if ( pev->health <= FGRUNT_LIMP_HEALTH ) {
				// limp!
				iSequence = LookupActivity ( ACT_RUN_HURT );
			} else {
				iSequence = LookupActivity ( NewActivity );
			}
		break;
		case ACT_WALK:
			if ( pev->health <= FGRUNT_LIMP_HEALTH ) {
				// limp!
				iSequence = LookupActivity ( ACT_WALK_HURT );
			} else {
				iSequence = LookupActivity ( NewActivity );
			}
		break;
		case ACT_IDLE:
			if ( m_MonsterState == MONSTERSTATE_COMBAT ) {
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
	if ( iSequence > ACTIVITY_NOT_AVAILABLE ) {
		if ( pev->sequence != iSequence || !m_fSequenceLoops ) {
			pev->frame = 0;
		}

		pev->sequence = iSequence;	// Set to the reset anim (if it's there)
		ResetSequenceInfo( );
		SetYawSpeed();
	} else {
		// Not available try to get default anim
		ALERT ( at_console, "%s has no sequence for act:%d\n", STRING(pev->classname), NewActivity );
		pev->sequence = 0;	// Set to the reset anim (if it's there)
	}
}
//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *CTorch :: GetSchedule ( void ) {
	// clear old sentence
	m_iSentence = -1;

	// flying? If PRONE, barnacle has me. IF not, it's assumed I am rapelling. 
	if (pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE) {
		if (pev->flags & FL_ONGROUND) {
			// just landed
			pev->movetype = MOVETYPE_STEP;
			return GetScheduleOfType(SCHED_TORCH_REPEL_LAND);
		}
		else {
			// repel down a rope, 
			if (m_MonsterState == MONSTERSTATE_COMBAT)
				return GetScheduleOfType(SCHED_TORCH_REPEL_ATTACK);
			else
				return GetScheduleOfType(SCHED_TORCH_REPEL);
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
			// dead enemy
			if ( HasConditions( bits_COND_ENEMY_DEAD ) ) {
				// call base class, all code to handle dead enemies is centralized there.
				return CBaseMonster :: GetSchedule();
			}

			// new enemy
			if ( HasConditions(bits_COND_NEW_ENEMY) ) {
				if ( InSquad() ) {
					MySquadLeader()->m_fEnemyEluded = FALSE;
					if ( !IsLeader() ) {
						if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) ) {
							return GetScheduleOfType ( SCHED_TORCH_SUPPRESS );
						} else {
							return GetScheduleOfType ( SCHED_TORCH_ESTABLISH_LINE_OF_FIRE );
						}
					} else {
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
								SENTENCEG_PlayRndSz( ENT(pev), "FG_ALERT", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
							else
								// player
								SENTENCEG_PlayRndSz( ENT(pev), "FG_ATTACK", VOL_NORM, ATTN_NORM, 0, m_voicePitch);

							JustSpoke();
						}
						
						if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) ) {
							return GetScheduleOfType ( SCHED_TORCH_SUPPRESS );
						} else {
							return GetScheduleOfType ( SCHED_TORCH_ESTABLISH_LINE_OF_FIRE );
						}
					}
				}
			} else if ( HasConditions ( bits_COND_NO_AMMO_LOADED ) ) {
				//!!!KELLY - this individual just realized he's out of bullet ammo. 
				// He's going to try to find cover to run to and reload, but rarely, if 
				// none is available, he'll drop and reload in the open here. 
				return GetScheduleOfType ( SCHED_TORCH_COVER_AND_RELOAD );
			} else if ( HasConditions( bits_COND_LIGHT_DAMAGE ) ) {
				// if hurt:
				// 90% chance of taking cover
				// 10% chance of flinch.
				int iPercent = RANDOM_LONG(0,99);

				if ( iPercent <= 90 && m_hEnemy != NULL ) {
					if (FOkToSpeak()) {
						//SENTENCEG_PlayRndSz( ENT(pev), "HG_COVER", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
						m_iSentence = TORCH_SENT_COVER;
						//JustSpoke();
					}
					// only try to take cover if we actually have an enemy!
					return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
				} else {
					return GetScheduleOfType( SCHED_SMALL_FLINCH );
				}
			} else if ( HasConditions ( bits_COND_CAN_MELEE_ATTACK1 ) ) {
				return GetScheduleOfType ( SCHED_MELEE_ATTACK1 );
			} else if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) ) {
				if ( InSquad() ) {
					// if the enemy has eluded the squad and a squad member has just located the enemy
					// and the enemy does not see the squad member, issue a call to the squad to waste a 
					// little time and give the player a chance to turn.
					if ( MySquadLeader()->m_fEnemyEluded && !HasConditions ( bits_COND_ENEMY_FACING_ME ) ) {
						MySquadLeader()->m_fEnemyEluded = FALSE;
						return GetScheduleOfType ( SCHED_TORCH_FOUND_ENEMY );
					}
				}

				if ( OccupySlot ( bits_SLOTS_FGRUNT_ENGAGE ) ) {
					// try to take an available ENGAGE slot
					return GetScheduleOfType( SCHED_RANGE_ATTACK1 );
				} else if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_FGRUNT_GRENADE ) ) {
					// throw a grenade if can and no engage slots are available
					return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
				} else {
					// hide!
					return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
				}
			} else if ( HasConditions( bits_COND_ENEMY_OCCLUDED ) ) {
				if ( HasConditions( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_FGRUNT_GRENADE ) ) {
					//!!!KELLY - this grunt is about to throw or fire a grenade at the player. Great place for "fire in the hole"  "frag out" etc
					if (FOkToSpeak()) {
						SENTENCEG_PlayRndSz( ENT(pev), "FG_THROW", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
						JustSpoke();
					}
					return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
				} else if ( OccupySlot( bits_SLOTS_FGRUNT_ENGAGE ) ) {
					return GetScheduleOfType( SCHED_TORCH_ESTABLISH_LINE_OF_FIRE );
				} else {
					//!!!KELLY - grunt is going to stay put for a couple seconds to see if
					// the enemy wanders back out into the open, or approaches the
					// grunt's covered position. Good place for a taunt, I guess?
					if (FOkToSpeak() && RANDOM_LONG(0,1))
					{
						SENTENCEG_PlayRndSz( ENT(pev), "FG_TAUNT", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
					}
					return GetScheduleOfType( SCHED_STANDOFF );
				}
			}
			
			if ( HasConditions( bits_COND_SEE_ENEMY ) && !HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) ) {
				return GetScheduleOfType ( SCHED_TORCH_ESTABLISH_LINE_OF_FIRE );
			}
		}
		break;
		case MONSTERSTATE_ALERT:	
		case MONSTERSTATE_IDLE:
			if ( HasConditions ( bits_COND_NO_AMMO_LOADED ) ) {
				return GetScheduleOfType ( SCHED_RELOAD );
			}

			if ( m_hEnemy == NULL && IsFollowing() ) {
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
// CTorchRepel - when triggered, spawns a
// repelling down a line.
//=========================================================
LINK_ENTITY_TO_CLASS( monster_human_torch_ally_repel, CTorchRepel );

void CTorchRepel::Spawn( void )
{
	Precache( );
	pev->solid = SOLID_NOT;

	SetUse( &CTorchRepel :: RepelUse );
}

void CTorchRepel::Precache( void )
{
	UTIL_PrecacheOther( "monster_human_torch_ally" );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/rope.spr" );
}

void CTorchRepel::RepelUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin + Vector( 0, 0, -4096.0), dont_ignore_monsters, ENT(pev), &tr);

	CBaseEntity *pEntity = Create( "monster_human_torch_ally", pev->origin, pev->angles );
	CBaseMonster *pGrunt = pEntity->MyMonsterPointer( );
	pGrunt->pev->movetype = MOVETYPE_FLY;
	pGrunt->pev->velocity = Vector( 0, 0, RANDOM_FLOAT( -196, -128 ) );
	pGrunt->SetActivity( ACT_GLIDE );
	pGrunt->m_vecLastPosition = tr.vecEndPos;

	CBeam *pBeam = CBeam::BeamCreate( "sprites/rope.spr", 10 );
	pBeam->PointEntInit( pev->origin + Vector(0,0,112), pGrunt->entindex() );
	pBeam->SetFlags( BEAM_FSOLID );
	pBeam->SetColor( 255, 255, 255 );
	pBeam->SetThink( &CBeam :: SUB_Remove );
	pBeam->pev->nextthink = UTIL_GlobalTimeBase() + -4096.0 * tr.flFraction / pGrunt->pev->velocity.z + 0.5;

	UTIL_Remove( this );
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
char *CDeadTorch::m_szPoses[] = { "dead_on_stomach", "deadstomach", "deadside", "deadsitting" };

void CDeadTorch::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "pose"))
	{
		m_iPose = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else 
		CBaseMonster::KeyValue( pkvd );
}

LINK_ENTITY_TO_CLASS( monster_human_torch_ally_dead, CDeadTorch );

//=========================================================
// ********** DeadBGrunt SPAWN **********
//=========================================================
void CDeadTorch :: Spawn( )
{
	PRECACHE_MODEL("models/hgrunt_torch.mdl");
	SET_MODEL(ENT(pev), "models/hgrunt_torch.mdl");

	pev->effects		= 0;
	pev->yaw_speed		= 8;
	pev->sequence		= 0;
	m_bloodColor		= BLOOD_COLOR_RED;

	pev->sequence = LookupSequence( m_szPoses[m_iPose] );
	if (pev->sequence == -1)
	{
		ALERT ( at_console, "Dead fgrunt with bad pose\n" );
	}
	// Corpses have less health
	pev->health			= 8;

	MonsterInitDead();
}
//=========================================================
// TraceAttack - make sure we're not taking it in the helmet
//=========================================================
void CDeadTorch :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	TraceResult tr;
	// check for helmet shot
	if (ptr->iHitgroup == 8)
	{
		// make sure we're wearing one
		if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_CLUB))
		{
			UTIL_Ricochet( ptr->vecEndPos, 1.0 );
			MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
				WRITE_COORD( ptr->vecEndPos.x );	// Send to PAS because of the sound
				WRITE_COORD( ptr->vecEndPos.y );
				WRITE_COORD( ptr->vecEndPos.z );
				WRITE_SHORT( g_sModelIndexFireball );
				WRITE_BYTE( 15  ); // scale * 10
				WRITE_BYTE( 15  ); // framerate
				WRITE_BYTE( TE_EXPLFLAG_NONE );
			MESSAGE_END();
			RadiusDamage ( pev, pev, 100, CLASS_NONE, DMG_BLAST );
			Create( "spark_shower", pev->origin, tr.vecPlaneNormal, NULL );
		}
	}
	CBaseMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}