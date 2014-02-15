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
//=========================================================
// Opposing-Force
// Human Frunt Ally Medic
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

//=========================================================
// torch defines
//=========================================================
#define	MEDIC_CLIP_SIZE					17 // how many bullets in a clip? - NOTE: 3 round burst sound, so keep as 3 * x!
#define MEDIC_LIMP_HEALTH				20

#define MEDIC_EAGLE					1
#define MEDIC_PISTOL				2
#define MEDIC_NEEDLE				4

// Weapon group
#define GUN_GROUP					3
#define GUN_EAGLE					0
#define GUN_PISTOL					1
#define GUN_NEEDLE					2
#define GUN_NONE					3

// Head group
#define MEDIC_HEAD_GROUP					2
#define MEDIC_HEAD_WHITE					0
#define MEDIC_HEAD_BLACK					1

//=========================================================
// monster-specific conditions
//=========================================================
#define bits_COND_MEDIC_NOFIRE	( bits_COND_SPECIAL4 )
//=========================================================
// monster-specific tasks
//=========================================================
enum 
{
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
#define		MEDIC_AE_CAUGHT_ENEMY	( 10) // grunt established sight with an enemy (player only) that had previously eluded the squad.
#define		MEDIC_AE_DROP_GUN		( 11) // grunt (probably dead) is dropping his mp5.
#define		MEDIC_AE_HIDEGUN		( 15)
#define		MEDIC_AE_SHOWNEEDLE		( 16)
#define		MEDIC_AE_HIDENEEDLE		( 17)
#define		MEDIC_AE_SHOWGUN		( 18)
//=========================================================
// monster-specific schedule types
//=========================================================
enum
{
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
class CMedic : public CRCAllyMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	int  ISoundMask( void );
	int  Classify ( void );
	void CheckAmmo ( void );
	void KeyValue( KeyValueData *pkvd );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	void SetActivity ( Activity NewActivity );
	void RunTask( Task_t *pTask );
	void StartTask( Task_t *pTask );
	virtual int	ObjectCaps( void ) { return CRCAllyMonster :: ObjectCaps() | FCAP_IMPULSE_USE; }
	BOOL FCanCheckAttacks ( void );
	BOOL CheckRangeAttack1 ( float flDot, float flDist );
	BOOL CheckRangeAttack2 ( float flDot, float flDist );
	BOOL CheckMeleeAttack1 ( float flDot, float flDist );
	void DeclineFollowing( void );
	void PrescheduleThink ( void );
	Vector GetGunPosition( void );
	void Shoot ( void );
	void Pistol ( void );

	void HealerFollow( CBaseEntity *pHealTarget );

	BOOL CanHeal( void ); // Can we heal the player, or the injured grunt?
	void Heal( void );// Lets apply the healing.
	
	// Override these to set behavior
	CBaseEntity	*Kick( void );
	Schedule_t *GetScheduleOfType ( int Type );
	Schedule_t *GetSchedule ( void );
	MONSTERSTATE GetIdealState ( void );

	void DeathSound( void );
	void PainSound( void );
	void GibMonster( void );
	void TalkInit( void );

	BOOL FOkToSpeak( void );
	void JustSpoke( void );

	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	
	int IRelationship ( CBaseEntity *pTarget );
	void Killed( entvars_t *pevAttacker, int iGib );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	// UNDONE: What is this for?  It isn't used?
	float	m_flPlayerDamage;// how much pain has the player inflicted on me?

	// checking the feasibility of a grenade toss is kind of costly, so we do it every couple of seconds,
	// not every server frame.
	float m_flNextGrenadeCheck;
	float m_flNextPainTime;
	float m_flLastEnemySightTime;

	Vector	m_vecTossVelocity;

	BOOL	m_fThrowGrenade;
	BOOL	m_fStanding;
	BOOL	m_fFirstEncounter;// only put on the handsign show in the squad's first encounter.
	int		m_cClipSize;

	BOOL	m_fDepleteLine;

	int		m_iHead;
	int		m_iBrassShell;
	int		m_iSentence;
	float	m_flHealAnount;

	static const char *pGruntSentences[];

	CUSTOM_SCHEDULES;

private:
	float m_healTime;
};

LINK_ENTITY_TO_CLASS( monster_human_medic_ally, CMedic );

TYPEDESCRIPTION	CMedic::m_SaveData[] = 
{
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

const char *CMedic::pGruntSentences[] = 
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
//
// !!! netname entvar field is used in squadmonster for groupname!!!
//=========================================================
void CMedic :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "head"))
	{
		m_iHead = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseMonster::KeyValue( pkvd );
	}
}
void CMedic::Killed( entvars_t *pevAttacker, int iGib )
{
	SetUse( NULL );

	CRCAllyMonster::Killed( pevAttacker, iGib );
}

//=========================================================
// someone else is talking - don't speak
//=========================================================
BOOL CMedic :: FOkToSpeak( void )
{
// if someone else is talking, don't speak
	if (gpGlobals->time <= CRCAllyMonster::g_talkWaitTime)
		return FALSE;

	if ( pev->spawnflags & SF_MONSTER_GAG )
	{
		if ( m_MonsterState != MONSTERSTATE_COMBAT )
		{
			// no talking outside of combat if gagged.
			return FALSE;
		}
	}

	// if player is not in pvs, don't speak
//	if (FNullEnt(FIND_CLIENT_IN_PVS(edict())))
//		return FALSE;
	
	return TRUE;
}
//=========================================================
//=========================================================
void CMedic :: JustSpoke( void )
{
	CRCAllyMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT(1.5, 2.0);
	m_iSentence = MEDIC_SENT_NONE;
}

//=========================================================
// IRelationship - overridden because Male Assassins are 
// Human Grunt's nemesis.
//=========================================================
int CMedic::IRelationship ( CBaseEntity *pTarget )
{
	if ( FClassnameIs( pTarget->pev, "monster_male_assassin" ) )
	{
		return R_NM;
	}

	return CRCAllyMonster::IRelationship( pTarget );
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
		ARRAYSIZE ( tlMedicFollow ),
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
		ARRAYSIZE ( tlMedicFaceTarget ),
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
		ARRAYSIZE ( tlMedicIdleStand ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND	|
		bits_COND_SMELL			|
		bits_COND_MEDIC_HEAL	|
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
		ARRAYSIZE ( tlMedicFail ),
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
		ARRAYSIZE ( tlMedicCombatFail ),
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
		ARRAYSIZE ( tlMedicVictoryDance ), 
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
		ARRAYSIZE ( tlMedicEstablishLineOfFire ),
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
		ARRAYSIZE ( tlMedicFoundEnemy ), 
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
		ARRAYSIZE ( tlMedicCombatFace1 ), 
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
		ARRAYSIZE ( tlMedicSignalSuppress ), 
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
		ARRAYSIZE ( tlMedicSuppress ), 
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
		ARRAYSIZE ( tlMedicWaitInCover ), 
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
		ARRAYSIZE ( tlMedicTakeCover1 ), 
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
		ARRAYSIZE ( tlMedicGrenadeCover1 ), 
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
		ARRAYSIZE ( tlMedicTossGrenadeCover1 ), 
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
		ARRAYSIZE ( tlMedicTakeCoverFromBestSound ), 
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
		ARRAYSIZE ( tlMedicHideReload ),
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
		ARRAYSIZE ( tlMedicSweep ), 
		
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
		ARRAYSIZE ( tlMedicRangeAttack1A ), 
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
		ARRAYSIZE ( tlMedicRangeAttack1B ), 
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
		ARRAYSIZE ( tlMedicRangeAttack2 ), 
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
		ARRAYSIZE ( tlMedicRepel ), 
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
		ARRAYSIZE ( tlMedicRepelAttack ), 
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
		ARRAYSIZE ( tlMedicRepelLand ), 
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
		ARRAYSIZE ( tlMedicHeal ),
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

void CMedic :: StartTask( Task_t *pTask )
{
	m_iTaskStatus = TASKSTATUS_RUNNING;

	switch ( pTask->iTask )
	{
	case TASK_MEDIC_SAY_HEAL:
		Talk( 2 );
		m_hTalkTarget = m_hTargetEnt;
		PlaySentence( "MG_HEAL", 2, VOL_NORM, ATTN_IDLE );

		TaskComplete();
		break;
	case TASK_MEDIC_CHECK_FIRE:
		if ( !NoFriendlyFire() )
		{
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

	case TASK_MEDIC_FACE_TOSS_DIR:
		break;

	case TASK_MEDIC_HEAL:
		m_IdealActivity = ACT_MELEE_ATTACK2;
		Heal();
		break;

	case TASK_FACE_IDEAL:
	case TASK_FACE_ENEMY:
		CRCAllyMonster :: StartTask( pTask );
		if (pev->movetype == MOVETYPE_FLY)
		{
			m_IdealActivity = ACT_GLIDE;
		}
		break;

	default: 
		CRCAllyMonster :: StartTask( pTask );
		break;
	}
}

void CMedic :: RunTask( Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_MEDIC_FACE_TOSS_DIR:
		{
			// project a point along the toss vector and turn to face that point.
			MakeIdealYaw( pev->origin + m_vecTossVelocity * 64 );
			ChangeYaw( pev->yaw_speed );

			if ( FacingIdeal() )
			{
				m_iTaskStatus = TASKSTATUS_COMPLETE;
			}
			break;
		}
	case TASK_MEDIC_HEAL:
		if ( m_fSequenceFinished )
		{
			TaskComplete();
		}
		else
		{
			if ( TargetDistance() > 90 )
				TaskComplete();
			pev->ideal_yaw = UTIL_VecToYaw( m_hTargetEnt->pev->origin - pev->origin );
			ChangeYaw( pev->yaw_speed );
		}
		break;
	default:
		{
			CRCAllyMonster :: RunTask( pTask );
			break;
		}
	}
}
//=========================================================
// GibMonster - make gun fly through the air.
//=========================================================
void CMedic :: GibMonster ( void )
{
	Vector	vecGunPos;
	Vector	vecGunAngles;

	if ( GetBodygroup( 3 ) != 2 )
	{// throw a gun if the grunt has one
		GetAttachment( 0, vecGunPos, vecGunAngles );
		
		CBaseEntity *pGun;
		if ( pev->weapons == MEDIC_PISTOL )
		{
			pGun = DropItem( "weapon_glock", vecGunPos, vecGunAngles );
		}
		if ( pev->weapons == MEDIC_EAGLE )
		{
			pGun = DropItem( "weapon_eagle", vecGunPos, vecGunAngles );
		}

		if ( pGun )
		{
			pGun->pev->velocity = Vector (RANDOM_FLOAT(-100,100), RANDOM_FLOAT(-100,100), RANDOM_FLOAT(200,300));
			pGun->pev->avelocity = Vector ( 0, RANDOM_FLOAT( 200, 400 ), 0 );
		}
	}
	CBaseMonster :: GibMonster();
}
//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards. 
//=========================================================
int CMedic :: ISoundMask ( void) 
{
	return	bits_SOUND_WORLD	|
			bits_SOUND_COMBAT	|
			bits_SOUND_CARCASS	|
			bits_SOUND_MEAT		|
			bits_SOUND_GARBAGE	|
			bits_SOUND_DANGER	|
			bits_SOUND_PLAYER;
}
//=========================================================
// CheckAmmo - overridden for the grunt because he actually
// uses ammo! (base class doesn't)
//=========================================================
void CMedic :: CheckAmmo ( void )
{
	if ( m_cAmmoLoaded <= 0 )
	{
		SetConditions(bits_COND_NO_AMMO_LOADED);
	}
}
//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CMedic :: Classify ( void )
{
	return	CLASS_PLAYER_ALLY;
}
//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CMedic :: SetYawSpeed ( void )
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:	
		ys = 150;		
		break;
	case ACT_RUN:	
		ys = 150;	
		break;
	case ACT_WALK:	
		ys = 180;		
		break;
	case ACT_RANGE_ATTACK1:	
		ys = 120;	
		break;
	case ACT_RANGE_ATTACK2:	
		ys = 120;	
		break;
	case ACT_MELEE_ATTACK1:	
		ys = 120;	
		break;
	case ACT_MELEE_ATTACK2:	
		ys = 120;	
		break;
	case ACT_TURN_LEFT:
	case ACT_TURN_RIGHT:	
		ys = 180;
		break;
	case ACT_GLIDE:
	case ACT_FLY:
		ys = 30;
		break;
	default:
		ys = 90;
		break;
	}

	pev->yaw_speed = ys;
}


//=========================================================
// PrescheduleThink - this function runs after conditions
// are collected and before scheduling code is run.
//=========================================================
void CMedic :: PrescheduleThink ( void )
{
	if ( InSquad() && m_hEnemy != NULL )
	{
		if ( HasConditions ( bits_COND_SEE_ENEMY ) )
		{
			// update the squad's last enemy sighting time.
			MySquadLeader()->m_flLastEnemySightTime = gpGlobals->time;
		}
		else
		{
			if ( gpGlobals->time - MySquadLeader()->m_flLastEnemySightTime > 5 )
			{
				// been a while since we've seen the enemy
				MySquadLeader()->m_fEnemyEluded = TRUE;
			}
		}
	}
	CBaseMonster :: PrescheduleThink();
}
//=========================================================
// FCanCheckAttacks - this is overridden for human grunts
// because they can throw/shoot grenades when they can't see their
// target and the base class doesn't check attacks if the monster
// cannot see its enemy.
//
// !!!BUGBUG - this gets called before a 3-round burst is fired
// which means that a friendly can still be hit with up to 2 rounds. 
// ALSO, grenades will not be tossed if there is a friendly in front,
// this is a bad bug. Friendly machine gun fire avoidance
// will unecessarily prevent the throwing of a grenade as well.
//=========================================================
BOOL CMedic :: FCanCheckAttacks ( void )
{
	if ( !HasConditions( bits_COND_ENEMY_TOOFAR ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//=========================================================
// CheckMeleeAttack1
//=========================================================
BOOL CMedic :: CheckMeleeAttack1 ( float flDot, float flDist )
{
	CBaseMonster *pEnemy;

	if ( m_hEnemy != NULL )
	{
		pEnemy = m_hEnemy->MyMonsterPointer();

		if ( !pEnemy )
		{
			return FALSE;
		}
	}

	if ( flDist <= 64 && flDot >= 0.7	&& 
		 pEnemy->Classify() != CLASS_ALIEN_BIOWEAPON &&
		 pEnemy->Classify() != CLASS_PLAYER_BIOWEAPON )
	{
		return TRUE;
	}
	return FALSE;
}

//=========================================================
// CheckRangeAttack1 - overridden for HGrunt, cause 
// FCanCheckAttacks() doesn't disqualify all attacks based
// on whether or not the enemy is occluded because unlike
// the base class, the HGrunt can attack when the enemy is
// occluded (throw grenade over wall, etc). We must 
// disqualify the machine gun attack if the enemy is occluded.
//=========================================================
BOOL CMedic :: CheckRangeAttack1 ( float flDot, float flDist )
{
	if ( !HasConditions( bits_COND_ENEMY_OCCLUDED ) && flDist <= 2048 && flDot >= 0.5 && NoFriendlyFire() && ( GetBodygroup( 3 ) != 2 ) && ( GetBodygroup( 3 ) != 3 ) )
	{
		TraceResult	tr;

		if ( !m_hEnemy->IsPlayer() && flDist <= 64  )
		{
			// kick nonclients, but don't shoot at them.
			return FALSE;
		}

		Vector vecSrc = GetGunPosition();

		// verify that a bullet fired from the gun will hit the enemy before the world.
		UTIL_TraceLine( vecSrc, m_hEnemy->BodyTarget(vecSrc), ignore_monsters, ignore_glass, ENT(pev), &tr);

		if ( tr.flFraction == 1.0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

//=========================================================
// CheckRangeAttack2 - this checks the Grunt's grenade
// attack. 
//=========================================================
BOOL CMedic :: CheckRangeAttack2 ( float flDot, float flDist )
{	
	// if the grunt isn't moving, it's ok to check.
	if ( m_flGroundSpeed != 0 )
	{
		m_fThrowGrenade = FALSE;
		return m_fThrowGrenade;
	}

	// assume things haven't changed too much since last time
	if (gpGlobals->time < m_flNextGrenadeCheck )
	{
		return m_fThrowGrenade;
	}

	if ( !FBitSet ( m_hEnemy->pev->flags, FL_ONGROUND ) && m_hEnemy->pev->waterlevel == 0 && m_vecEnemyLKP.z > pev->absmax.z  )
	{
		//!!!BUGBUG - we should make this check movetype and make sure it isn't FLY? Players who jump a lot are unlikely to 
		// be grenaded.
		// don't throw grenades at anything that isn't on the ground!
		m_fThrowGrenade = FALSE;
		return m_fThrowGrenade;
	}
	
	Vector vecTarget;

	// find feet
	if (RANDOM_LONG(0,1))
	{
		// magically know where they are
		vecTarget = Vector( m_hEnemy->pev->origin.x, m_hEnemy->pev->origin.y, m_hEnemy->pev->absmin.z );
	}
	else
	{
		// toss it to where you last saw them
		vecTarget = m_vecEnemyLKP;
	}
	// vecTarget = m_vecEnemyLKP + (m_hEnemy->BodyTarget( pev->origin ) - m_hEnemy->pev->origin);
	// estimate position
	// vecTarget = vecTarget + m_hEnemy->pev->velocity * 2;

	// are any of my squad members near the intended grenade impact area?
	if ( InSquad() )
	{
		if (SquadMemberInRange( vecTarget, 256 ))
		{
			// crap, I might blow my own guy up. Don't throw a grenade and don't check again for a while.
			m_flNextGrenadeCheck = gpGlobals->time + 1; // one full second.
			m_fThrowGrenade = FALSE;
			return m_fThrowGrenade;	//AJH need this or it is overridden later.
		}
	}
	
	if ( ( vecTarget - pev->origin ).Length2D() <= 256 )
	{
		// crap, I don't want to blow myself up
		m_flNextGrenadeCheck = gpGlobals->time + 1; // one full second.
		m_fThrowGrenade = FALSE;
		return m_fThrowGrenade;
	}

		
	Vector vecToss = VecCheckToss( pev, GetGunPosition(), vecTarget, 0.5 );

	if ( vecToss != g_vecZero )
	{
		m_vecTossVelocity = vecToss;

		// throw a hand grenade
		m_fThrowGrenade = TRUE;
		// don't check again for a while.
		m_flNextGrenadeCheck = gpGlobals->time; // 1/3 second.
	}
	else
	{
		// don't throw
		m_fThrowGrenade = FALSE;
		// don't check again for a while.
		m_flNextGrenadeCheck = gpGlobals->time + 1; // one full second.
	}
	return m_fThrowGrenade;
}
//=========================================================
//=========================================================
CBaseEntity *CMedic :: Kick( void )
{
	TraceResult tr;

	UTIL_MakeVectors( pev->angles );
	Vector vecStart = pev->origin;
	vecStart.z += pev->size.z * 0.5;
	Vector vecEnd = vecStart + (gpGlobals->v_forward * 70);

	UTIL_TraceHull( vecStart, vecEnd, dont_ignore_monsters, head_hull, ENT(pev), &tr );
	
	if ( tr.pHit )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		return pEntity;
	}

	return NULL;
}

//=========================================================
// GetGunPosition	return the end of the barrel
//=========================================================

Vector CMedic :: GetGunPosition( )
{
	if (m_fStanding )
	{
		return pev->origin + Vector( 0, 0, 60 );
	}
	else
	{
		return pev->origin + Vector( 0, 0, 48 );
	}
}

//=========================================================
// Shoot
//=========================================================
void CMedic :: Shoot ( void )
{
	if (m_hEnemy == NULL || !NoFriendlyFire() )
	{
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

	UTIL_MakeVectors ( pev->angles );

	Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40,90) + gpGlobals->v_up * RANDOM_FLOAT(75,200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
	EjectBrass ( vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL); 
	//FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_1DEGREES, 1024, BULLET_MONSTER_EAGLE ); // shoot +-5 degrees
	FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_1DEGREES, 1024, BULLET_MONSTER_9MM ); // shoot +-5 degrees

	pev->effects |= EF_MUZZLEFLASH;

	WeaponFlash ( vecShootOrigin );

	m_cAmmoLoaded--;// take away a bullet!

	Vector angDir = UTIL_VecToAngles( vecShootDir );
	SetBlending( 0, angDir.x );
}
//=========================================================
// Shoot
//=========================================================
void CMedic :: Pistol ( void )
{
	if (m_hEnemy == NULL || !NoFriendlyFire() )
	{
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

	UTIL_MakeVectors ( pev->angles );

	Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40,90) + gpGlobals->v_up * RANDOM_FLOAT(75,200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
	EjectBrass ( vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL); 
	FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_1DEGREES, 1024, BULLET_MONSTER_9MM ); // shoot +-5 degrees

	pev->effects |= EF_MUZZLEFLASH;
	
	WeaponFlash ( vecShootOrigin );

	m_cAmmoLoaded--;// take away a bullet!

	Vector angDir = UTIL_VecToAngles( vecShootDir );
	SetBlending( 0, angDir.x );
}
//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CMedic :: HandleAnimEvent( MonsterEvent_t *pEvent )
{
	Vector	vecShootDir;
	Vector	vecShootOrigin;

	switch( pEvent->event )
	{
		case MEDIC_AE_SHOWNEEDLE:
			SetBodygroup( GUN_GROUP, GUN_NEEDLE );
			break;

		case MEDIC_AE_SHOWGUN:
			if ( pev->weapons == MEDIC_EAGLE)
			{
				SetBodygroup( GUN_GROUP, GUN_EAGLE );
			}
			else
			{
				SetBodygroup( GUN_GROUP, GUN_PISTOL );
			}
			break;

		case MEDIC_AE_HIDEGUN:
			SetBodygroup( GUN_GROUP, GUN_NONE );
			break;

		case MEDIC_AE_HIDENEEDLE:
			SetBodygroup( GUN_GROUP, GUN_NONE );
			break;

		case MEDIC_AE_DROP_GUN:
			{
			Vector	vecGunPos;
			Vector	vecGunAngles;

			GetAttachment( 0, vecGunPos, vecGunAngles );

			// switch to body group with no gun.
			SetBodygroup( GUN_GROUP, GUN_NONE );

			// now spawn a gun.
				if ( pev->weapons == MEDIC_PISTOL )
				{
					DropItem( "weapon_glock", vecGunPos, vecGunAngles );
				}
				if ( pev->weapons == MEDIC_EAGLE )
				{
					DropItem( "weapon_eagle", vecGunPos, vecGunAngles );
				}
			}
			break;

		case MEDIC_AE_RELOAD:
			EMIT_SOUND( ENT(pev), CHAN_WEAPON, "hgrunt/gr_reload1.wav", 1, ATTN_NORM );
			m_cAmmoLoaded = m_cClipSize;
			ClearConditions(bits_COND_NO_AMMO_LOADED);
			break;

		case MEDIC_AE_GREN_TOSS:
		{
			UTIL_MakeVectors( pev->angles );
			// CGrenade::ShootTimed( pev, pev->origin + gpGlobals->v_forward * 34 + Vector (0, 0, 32), m_vecTossVelocity, 3.5 );
			CGrenade::ShootTimed( pev, GetGunPosition(), m_vecTossVelocity, 3.5 );

			m_fThrowGrenade = FALSE;
			m_flNextGrenadeCheck = gpGlobals->time + 6;// wait six seconds before even looking again to see if a grenade can be thrown.
			// !!!LATER - when in a group, only try to throw grenade if ordered.
		}
		break;

		case MEDIC_AE_GREN_DROP:
		{
			UTIL_MakeVectors( pev->angles );
			CGrenade::ShootTimed( pev, pev->origin + gpGlobals->v_forward * 17 - gpGlobals->v_right * 27 + gpGlobals->v_up * 6, g_vecZero, 3 );
		}
		break;

		case MEDIC_AE_BURST1:
		{
			if ( pev->weapons == MEDIC_EAGLE )
			{
				Shoot();
				EMIT_SOUND( ENT(pev), CHAN_WEAPON, "weapons/desert_eagle_fire.wav", 1, ATTN_NORM );
			}
			else
			{
				Pistol();
				EMIT_SOUND( ENT(pev), CHAN_WEAPON, "weapons/pl_gun3.wav", 1, ATTN_NORM );
			}
			CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 384, 0.3 );
		}
		break;

		case MEDIC_AE_BURST2:
		case MEDIC_AE_BURST3:
		{
			if ( pev->weapons == MEDIC_EAGLE )
				Shoot();
			else
				Pistol();
		}
		break;

		case MEDIC_AE_KICK:
		{
			CBaseEntity *pHurt = Kick();

			if ( pHurt )
			{
				// SOUND HERE!
				UTIL_MakeVectors( pev->angles );
				pHurt->pev->punchangle.x = 15;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * 100 + gpGlobals->v_up * 50;
				pHurt->TakeDamage( pev, pev, gSkillData.medicDmgKick, DMG_CLUB );
			}
		}
		break;

		case MEDIC_AE_CAUGHT_ENEMY:
		{
			if ( FOkToSpeak() )
			{
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
void CMedic :: Spawn()
{
	Precache( );

	SET_MODEL(ENT(pev), "models/hgrunt_medic.mdl");
	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_RED;
	pev->health			= gSkillData.medicHealth;
	pev->view_ofs		= Vector ( 0, 0, 50 );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_MonsterState		= MONSTERSTATE_NONE;
	m_flNextGrenadeCheck = gpGlobals->time + 1;
	m_flNextPainTime	= gpGlobals->time;
	m_flHealAnount		= gSkillData.medicHeal;

	m_afCapability		= bits_CAP_HEAR | bits_CAP_SQUAD | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;
	m_hHealTarget		= NULL;
	m_fEnemyEluded		= FALSE;
	m_fFirstEncounter	= TRUE;// this is true when the grunt spawns, because he hasn't encountered an enemy yet.
	m_cClipSize = MEDIC_CLIP_SIZE;
	m_HackedGunPos = Vector ( 0, 0, 55 );

	if (pev->weapons == 0)
	{
		pev->weapons = MEDIC_PISTOL;
	}

	if ( pev->weapons == MEDIC_PISTOL )
	{
		SetBodygroup( GUN_GROUP, GUN_PISTOL );
		m_cAmmoLoaded	= MEDIC_CLIP_SIZE;
	}

	if ( pev->weapons == MEDIC_EAGLE )
	{
		SetBodygroup( GUN_GROUP, GUN_EAGLE );
		m_cAmmoLoaded	= 7;
	}

	if ( pev->weapons == MEDIC_NEEDLE )
	{
		SetBodygroup( GUN_GROUP, GUN_NEEDLE );
		m_cAmmoLoaded	= 7;
	}

	if ( m_iHead == -1 )
	{
		m_iHead = RANDOM_LONG(0,1);// pick a head, any head
	}
	if ( m_iHead == 0 )
	{
		SetBodygroup( MEDIC_HEAD_GROUP, MEDIC_HEAD_WHITE );
	}
	if ( m_iHead == 1 )
	{
		SetBodygroup( MEDIC_HEAD_GROUP, MEDIC_HEAD_BLACK );
	}

	MonsterInit();
	StartMonster();
	SetUse(&CMedic :: FollowerUse );
}
void CMedic::HealerFollow( CBaseEntity *pHealTarget )
{
	if ( m_pCine )
		m_pCine->CancelScript();

	m_hTargetEnt = pHealTarget;
	m_hTalkTarget = pHealTarget;

	ClearSchedule();
}
//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CMedic :: Precache()
{
	PRECACHE_MODEL("models/hgrunt_medic.mdl");

	PRECACHE_SOUND("weapons/desert_eagle_fire.wav" );
	PRECACHE_SOUND("weapons/pl_gun3.wav" );

	PRECACHE_SOUND("fgrunt/gr_pain1.wav");
	PRECACHE_SOUND("fgrunt/gr_pain2.wav");
	PRECACHE_SOUND("fgrunt/gr_pain3.wav");
	PRECACHE_SOUND("fgrunt/gr_pain4.wav");
	PRECACHE_SOUND("fgrunt/gr_pain5.wav");
	PRECACHE_SOUND("fgrunt/gr_pain6.wav");

	PRECACHE_SOUND("fgrunt/death1.wav");
	PRECACHE_SOUND("fgrunt/death2.wav");
	PRECACHE_SOUND("fgrunt/death3.wav");
	PRECACHE_SOUND("fgrunt/death4.wav");
	PRECACHE_SOUND("fgrunt/death5.wav");
	PRECACHE_SOUND("fgrunt/death6.wav");

	PRECACHE_SOUND("hgrunt/gr_reload1.wav");

	PRECACHE_SOUND("zombie/claw_miss2.wav");// because we use the basemonster SWIPE animation event

	m_iBrassShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell

	TalkInit();
	CRCAllyMonster::Precache();
}	

// Init talk data
void CMedic :: TalkInit()
{
	CRCAllyMonster::TalkInit();

	// scientists speach group names (group names are in sentences.txt)

	m_szGrp[TLK_ANSWER]  =	"FG_ANSWER";
	m_szGrp[TLK_QUESTION] =	"FG_QUESTION";
	m_szGrp[TLK_IDLE] =		"FG_IDLE";
	m_szGrp[TLK_STARE] =		"FG_STARE";
	m_szGrp[TLK_USE] =		"FG_OK";
	m_szGrp[TLK_UNUSE] =	"FG_WAIT";
	m_szGrp[TLK_STOP] =		"FG_STOP";

	m_szGrp[TLK_NOSHOOT] =	"FG_SCARED";
	m_szGrp[TLK_HELLO] =	"FG_HELLO";

	m_szGrp[TLK_PLHURT1] =	"FG_CURE";
	m_szGrp[TLK_PLHURT2] =	"FG_CURE"; 
	m_szGrp[TLK_PLHURT3] =	"FG_CURE";

	m_szGrp[TLK_SMELL] =	"FG_SMELL";
	
	m_szGrp[TLK_WOUND] =	"FG_WOUND";
	m_szGrp[TLK_MORTAL] =	"FG_MORTAL";


	if ( m_iHead == 0 )
		m_voicePitch = 105;
	if ( m_iHead == 1 )
		m_voicePitch = 90;
}

static BOOL IsFacing( entvars_t *pevTest, const Vector &reference )
{
	Vector vecDir = (reference - pevTest->origin);
	vecDir.z = 0;
	vecDir = vecDir.Normalize();
	Vector forward, angle;
	angle = pevTest->v_angle;
	angle.x = 0;
	UTIL_MakeVectorsPrivate( angle, forward, NULL, NULL );
	// He's facing me, he meant it
	if ( DotProduct( forward, vecDir ) > 0.96 )	// +/- 15 degrees or so
	{
		return TRUE;
	}
	return FALSE;
}
	
//=========================================================
// PainSound
//=========================================================
void CMedic :: PainSound ( void )
{
	if ( gpGlobals->time > m_flNextPainTime )
	{
		switch ( RANDOM_LONG(0,5) )
		{
			case 0: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/gr_pain1.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
			case 1: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/gr_pain2.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
			case 2: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/gr_pain3.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
			case 3: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/gr_pain4.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
			case 4: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/gr_pain5.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
			case 5: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/gr_pain6.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
		}
		m_flNextPainTime = gpGlobals->time + 1;
	}
}

//=========================================================
// DeathSound 
//=========================================================
void CMedic :: DeathSound ( void )
{
	switch (RANDOM_LONG(0,5))
	{
	case 0: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/death1.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 1: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/death2.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 2: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/death3.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 3: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/death4.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 4: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/death5.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	case 5: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "fgrunt/death6.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
	}
}
//=========================================================
// TakeDamage - overridden for the grunt because the grunt
// needs to forget that he is in cover if he's hurt. (Obviously
// not in a safe place anymore).
//=========================================================
int CMedic :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	Forget( bits_MEMORY_INCOVER );

	// make sure friends talk about it if player hurts talkmonsters...
	int ret = CRCAllyMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	if ( !IsAlive() || pev->deadflag == DEAD_DYING )
		return ret;

	if ( m_MonsterState != MONSTERSTATE_PRONE && (pevAttacker->flags & FL_CLIENT) )
	{
		m_flPlayerDamage += flDamage;

		// This is a heurstic to determine if the player intended to harm me
		// If I have an enemy, we can't establish intent (may just be crossfire)
		if ( m_hEnemy == NULL )
		{
			// If the player was facing directly at me, or I'm already suspicious, get mad
			if ( (m_afMemory & bits_MEMORY_SUSPICIOUS) || IsFacing( pevAttacker, pev->origin ) )
			{
				// Alright, now I'm pissed!
				PlaySentence( "FG_MAD", 4, VOL_NORM, ATTN_NORM );

				Remember( bits_MEMORY_PROVOKED );
				StopFollowing( TRUE );
			}
			else
			{
				// Hey, be careful with that
				PlaySentence( "FG_SHOT", 4, VOL_NORM, ATTN_NORM );
				Remember( bits_MEMORY_SUSPICIOUS );
			}
		}
		else if ( !(m_hEnemy->IsPlayer()) && pev->deadflag == DEAD_NO )
		{
			PlaySentence( "FG_SHOT", 4, VOL_NORM, ATTN_NORM );
		}
	}

	return CRCAllyMonster :: TakeDamage ( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}
//=========================================================
// TraceAttack - make sure we're not taking it in the helmet
//=========================================================
void CMedic :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	// check for helmet shot
	if (ptr->iHitgroup == 11)
	{
		// absorb damage
		flDamage -= 20;
		if (flDamage <= 0)
		{
			UTIL_Ricochet( ptr->vecEndPos, 1.0 );
			flDamage = 0.01;
		}

		// it's head shot anyways
		ptr->iHitgroup = HITGROUP_HEAD;
	}
	CRCAllyMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}
//=========================================================
// AI Schedules Specific to this monster
//=========================================================

Schedule_t* CMedic :: GetScheduleOfType ( int Type )
{
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
	if ( iSequence > ACTIVITY_NOT_AVAILABLE )
	{
		if ( pev->sequence != iSequence || !m_fSequenceLoops )
		{
			pev->frame = 0;
		}

		pev->sequence		= iSequence;	// Set to the reset anim (if it's there)
		ResetSequenceInfo( );
		SetYawSpeed();
	}
	else
	{
		// Not available try to get default anim
		ALERT ( at_console, "%s has no sequence for act:%d\n", STRING(pev->classname), NewActivity );
		pev->sequence		= 0;	// Set to the reset anim (if it's there)
	}
}
//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *CMedic :: GetSchedule ( void )
{
	// grunts place HIGH priority on running away from danger sounds.
	if ( HasConditions(bits_COND_HEAR_SOUND) )
	{
		CSound *pSound;
		pSound = PBestSound();

		ASSERT( pSound != NULL );
		if ( pSound)
		{
			if (pSound->m_iType & bits_SOUND_DANGER)
			{
				// dangerous sound nearby!
				
				//!!!KELLY - currently, this is the grunt's signal that a grenade has landed nearby,
				// and the grunt should find cover from the blast
				// good place for "SHIT!" or some other colorful verbal indicator of dismay.
				// It's not safe to play a verbal order here "Scatter", etc cause 
				// this may only affect a single individual in a squad. 
				
				if (FOkToSpeak())
				{
					SENTENCEG_PlayRndSz( ENT(pev), "FG_GREN", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
					JustSpoke();
				}
				return GetScheduleOfType( SCHED_TAKE_COVER_FROM_BEST_SOUND );
			}
			/*
			if (!HasConditions( bits_COND_SEE_ENEMY ) && ( pSound->m_iType & (bits_SOUND_PLAYER | bits_SOUND_COMBAT) ))
			{
				MakeIdealYaw( pSound->m_vecOrigin );
			}
			*/
		}
	}
	// flying? If PRONE, barnacle has me. IF not, it's assumed I am rapelling. 
	if ( pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE )
	{
		if (pev->flags & FL_ONGROUND)
		{
			// just landed
			pev->movetype = MOVETYPE_STEP;
			return GetScheduleOfType ( SCHED_MEDIC_REPEL_LAND );
		}
		else
		{
			// repel down a rope, 
			if ( m_MonsterState == MONSTERSTATE_COMBAT )
				return GetScheduleOfType ( SCHED_MEDIC_REPEL_ATTACK );
			else
				return GetScheduleOfType ( SCHED_MEDIC_REPEL );
		}
	}
	if ( HasConditions( bits_COND_ENEMY_DEAD ) && FOkToSpeak() )
	{
		PlaySentence( "FG_KILL", 4, VOL_NORM, ATTN_NORM );
	}

	switch( m_MonsterState )
	{
	case MONSTERSTATE_COMBAT:
		{
// dead enemy
			if ( HasConditions( bits_COND_ENEMY_DEAD ) )
			{
				// call base class, all code to handle dead enemies is centralized there.
				return CBaseMonster :: GetSchedule();
			}

// new enemy
			if ( HasConditions(bits_COND_NEW_ENEMY) )
			{
				if ( InSquad() )
				{
					MySquadLeader()->m_fEnemyEluded = FALSE;

					if ( !IsLeader() )
					{
						if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) )
						{
							return GetScheduleOfType ( SCHED_MEDIC_SUPPRESS );
						}
						else
						{
							return GetScheduleOfType ( SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE );
						}
					}
					else 
					{
						//!!!KELLY - the leader of a squad of grunts has just seen the player or a 
						// monster and has made it the squad's enemy. You
						// can check pev->flags for FL_CLIENT to determine whether this is the player
						// or a monster. He's going to immediately start
						// firing, though. If you'd like, we can make an alternate "first sight" 
						// schedule where the leader plays a handsign anim
						// that gives us enough time to hear a short sentence or spoken command
						// before he starts pluggin away.
						if (FOkToSpeak())// && RANDOM_LONG(0,1))
						{
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
						
						if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) )
						{
							return GetScheduleOfType ( SCHED_MEDIC_SUPPRESS );
						}
						else
						{
							return GetScheduleOfType ( SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE );
						}
					}
				}
			}
// no ammo
			else if ( HasConditions ( bits_COND_NO_AMMO_LOADED ) )
			{
				//!!!KELLY - this individual just realized he's out of bullet ammo. 
				// He's going to try to find cover to run to and reload, but rarely, if 
				// none is available, he'll drop and reload in the open here. 
				return GetScheduleOfType ( SCHED_MEDIC_COVER_AND_RELOAD );
			}
			
// damaged just a little
			else if ( HasConditions( bits_COND_LIGHT_DAMAGE ) )
			{
				// if hurt:
				// 90% chance of taking cover
				// 10% chance of flinch.
				int iPercent = RANDOM_LONG(0,99);

				if ( iPercent <= 90 && m_hEnemy != NULL )
				{
					if (FOkToSpeak()) // && RANDOM_LONG(0,1))
					{
						//SENTENCEG_PlayRndSz( ENT(pev), "HG_COVER", HGRUNT_SENTENCE_VOLUME, GRUNT_ATTN, 0, m_voicePitch);
						m_iSentence = MEDIC_SENT_COVER;
						//JustSpoke();
					}
					// only try to take cover if we actually have an enemy!
					return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
				}
				else
				{
					return GetScheduleOfType( SCHED_SMALL_FLINCH );
				}
			}
// can kick
			else if ( HasConditions ( bits_COND_CAN_MELEE_ATTACK1 ) )
			{
				return GetScheduleOfType ( SCHED_MELEE_ATTACK1 );
			}
// can shoot
			else if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) )
			{
				if ( InSquad() )
				{
					// if the enemy has eluded the squad and a squad member has just located the enemy
					// and the enemy does not see the squad member, issue a call to the squad to waste a 
					// little time and give the player a chance to turn.
					if ( MySquadLeader()->m_fEnemyEluded && !HasConditions ( bits_COND_ENEMY_FACING_ME ) )
					{
						MySquadLeader()->m_fEnemyEluded = FALSE;
						return GetScheduleOfType ( SCHED_MEDIC_FOUND_ENEMY );
					}
				}
				if ( OccupySlot ( bits_SLOTS_FGRUNT_ENGAGE ) )
				{
					// try to take an available ENGAGE slot
					return GetScheduleOfType( SCHED_RANGE_ATTACK1 );
				}
				else if ( HasConditions ( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_FGRUNT_GRENADE ) )
				{
					// throw a grenade if can and no engage slots are available
					return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
				}
				else
				{
					// hide!
					return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
				}
			}
// can't see enemy
			else if ( HasConditions( bits_COND_ENEMY_OCCLUDED ) )
			{
				if ( HasConditions( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_FGRUNT_GRENADE ) )
				{
					//!!!KELLY - this grunt is about to throw or fire a grenade at the player. Great place for "fire in the hole"  "frag out" etc
					if (FOkToSpeak())
					{
						SENTENCEG_PlayRndSz( ENT(pev), "FG_THROW", VOL_NORM, ATTN_NORM, 0, m_voicePitch);
						JustSpoke();
					}
					return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
				}
				else if ( OccupySlot( bits_SLOTS_FGRUNT_ENGAGE ) )
				{
					return GetScheduleOfType( SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE );
				}
				else
				{
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
			
			if ( HasConditions( bits_COND_SEE_ENEMY ) && !HasConditions ( bits_COND_CAN_RANGE_ATTACK1 ) )
			{
				return GetScheduleOfType ( SCHED_MEDIC_ESTABLISH_LINE_OF_FIRE );
			}
		}
		break;

	case MONSTERSTATE_ALERT:	
	case MONSTERSTATE_IDLE:
		if ( HasConditions ( bits_COND_NO_AMMO_LOADED ) )
		{
			return GetScheduleOfType ( SCHED_RELOAD );
		}
		if ( HasConditions( bits_COND_MEDIC_HEAL ) && m_hHealTarget != NULL && !IsFollowing())// We have to heal somebody
		{
			// Unresolved
		}
		if ( m_hEnemy == NULL && IsFollowing() )
		{
			// If I'm already close enough to my target
			if ( TargetDistance() <= 128 )
			{
				if ( CanHeal() )	// Heal opportunistically
					return slMedicHeal;
				if ( HasConditions( bits_COND_CLIENT_PUSH ) )	// Player wants me to move
					return GetScheduleOfType( SCHED_MOVE_AWAY_FOLLOW );
			}
			if ( !m_hTargetEnt->IsAlive() )
			{
				// UNDONE: Comment about the recently dead player here?
				StopFollowing( FALSE );
				break;
			}
			else
			{
				if ( HasConditions( bits_COND_CLIENT_PUSH ) )
				{
					return GetScheduleOfType( SCHED_MOVE_AWAY_FOLLOW );
				}
				return GetScheduleOfType( SCHED_TARGET_FACE );
			}
		}

		if ( HasConditions( bits_COND_CLIENT_PUSH ) )
		{
			return GetScheduleOfType( SCHED_MOVE_AWAY );
		}

		break;
	}
	
	return CRCAllyMonster :: GetSchedule();
}
BOOL CMedic::CanHeal( void )
{ 
	ALERT(at_console, "Heal amount is %f\n", m_flHealAnount );
	if ( m_flHealAnount <= 0 )
	{
		if ( !m_fDepleteLine )
		{
			PlaySentence( "MG_NOTHEAL", 2, VOL_NORM, ATTN_IDLE );
			m_fDepleteLine = TRUE;
		}
		return FALSE;
	}

	if ( (m_healTime > gpGlobals->time) || (m_hTargetEnt == NULL) || (m_hTargetEnt->pev->health > (m_hTargetEnt->pev->max_health * 0.9)) )
	{
		return FALSE;
	}
	return TRUE;
}

void CMedic::Heal( void )
{
	if ( !CanHeal() )
		return;

	m_flHealAnount -= (m_hTargetEnt->pev->max_health - m_hTargetEnt->pev->health);
	Vector target = m_hTargetEnt->pev->origin - pev->origin;
	if ( target.Length() > 100 )
		return;

	m_hTargetEnt->TakeHealth( m_hTargetEnt->pev->max_health - m_hTargetEnt->pev->health, DMG_GENERIC );
}
MONSTERSTATE CMedic :: GetIdealState ( void )
{
	return CRCAllyMonster::GetIdealState();
}
void CMedic::DeclineFollowing( void )
{
	PlaySentence( "FG_STOP", 2, VOL_NORM, ATTN_NORM );
}
//=========================================================
// CMedicRepel - when triggered, spawns a
// repelling down a line.
//=========================================================

class CMedicRepel : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void EXPORT RepelUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int m_iSpriteTexture;	// Don't save, precache
};

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
	pBeam->pev->nextthink = gpGlobals->time + -4096.0 * tr.flFraction / pGrunt->pev->velocity.z + 0.5;

	UTIL_Remove( this );
}