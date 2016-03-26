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
// Squadmonster ally functions
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"rcallymonster.h"
#include	"defaultai.h"
#include	"scripted.h"
#include	"soundent.h"
#include	"animation.h"
#include	"plane.h"
#include	"weapons.h"

//=========================================================
// Talking monster base class
// Used for scientists and barneys
//=========================================================
float CRCAllyMonster::g_talkWaitTime = 0;		// time delay until it's ok to speak: used so that two NPCs don't talk at once

TYPEDESCRIPTION	CRCAllyMonster::m_SaveData[] = {
	DEFINE_FIELD(CRCAllyMonster, m_bitsSaid, FIELD_INTEGER),
	DEFINE_FIELD(CRCAllyMonster, m_nSpeak, FIELD_INTEGER),
	DEFINE_FIELD(CRCAllyMonster, m_useTime, FIELD_TIME),
	DEFINE_FIELD(CRCAllyMonster, m_iszUse, FIELD_STRING),
	DEFINE_FIELD(CRCAllyMonster, m_iszUnUse, FIELD_STRING),
	DEFINE_FIELD(CRCAllyMonster, m_iszDecline, FIELD_STRING), //LRC
	DEFINE_FIELD(CRCAllyMonster, m_iszSpeakAs, FIELD_STRING), //LRC
	DEFINE_FIELD(CRCAllyMonster, m_flLastSaidSmelled, FIELD_TIME),
	DEFINE_FIELD(CRCAllyMonster, m_flStopTalkTime, FIELD_TIME),
	DEFINE_FIELD(CRCAllyMonster, m_hTalkTarget, FIELD_EHANDLE),
	DEFINE_FIELD(CRCAllyMonster, m_hHealTarget, FIELD_EHANDLE),
	DEFINE_FIELD(CRCAllyMonster, m_hSquadLeader, FIELD_EHANDLE),
	DEFINE_ARRAY(CRCAllyMonster, m_hSquadMember, FIELD_EHANDLE, MAXRC_SQUAD_MEMBERS - 1),
	DEFINE_FIELD(CRCAllyMonster, m_fEnemyEluded, FIELD_BOOLEAN),
	DEFINE_FIELD(CRCAllyMonster, m_flLastEnemySightTime, FIELD_TIME),
	DEFINE_FIELD(CRCAllyMonster, m_iMySlot, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE( CRCAllyMonster, CBaseMonster );

// array of friend names
char *CRCAllyMonster::m_szFriends[TLK_CFRIENDS] = {
	"monster_barney",
	"monster_barniel",
	"monster_otis",
	"monster_scientist",
	"monster_sitting_scientist",
	"monster_cleansuit_scientist",
	"monster_sitting_cleansuit_scientist",
	"monster_human_grunt_ally",
	"monster_human_torch_ally",
	"monster_human_medic_ally"
};

//=========================================================
// Monster Sounds
//=========================================================
const char *CRCAllyMonster::pPainSounds[] = {
	"fgrunt/gr_pain1.wav",
	"fgrunt/gr_pain2.wav",
	"fgrunt/gr_pain3.wav",
	"fgrunt/gr_pain4.wav",
	"fgrunt/gr_pain5.wav",
	"fgrunt/gr_pain6.wav"
};

const char *CRCAllyMonster::pDeathSounds[] = {
	"fgrunt/death1.wav",
	"fgrunt/death2.wav",
	"fgrunt/death3.wav",
	"fgrunt/death4.wav",
	"fgrunt/death5.wav",
	"fgrunt/death6.wav"
};

const char *CRCAllyMonster::pAttackSoundsSAW[] = {
	"weapons/saw_fire1.wav",
	"weapons/saw_fire2.wav",
	"weapons/saw_fire3.wav"
};

const char *CRCAllyMonster::pAttackSounds9MM[] = {
	"hgrunt/gr_mgun1.wav",
	"hgrunt/gr_mgun2.wav"
};

//=========================================================
// AI Schedules Specific to talking monsters
//=========================================================
Task_t	tlIdleResponseTS[] =
{
	{ TASK_SET_ACTIVITY,	(float)ACT_IDLE	},// Stop and listen
	{ TASK_WAIT,			(float)0.5		},// Wait until sure it's me they are talking to
	{ TASK_TLK_EYECONTACT,	(float)0		},// Wait until speaker is done
	{ TASK_TLK_RESPOND,		(float)0		},// Wait and then say my response
	{ TASK_TLK_IDEALYAW,	(float)0		},// look at who I'm talking to
	{ TASK_FACE_IDEAL,		(float)0		}, 
	{ TASK_SET_ACTIVITY,	(float)ACT_IDLE	},
	{ TASK_TLK_EYECONTACT,	(float)0		},// Wait until speaker is done
};

Schedule_t	slIdleResponseTS[] =
{
	{ 
		tlIdleResponseTS,
		HL_ARRAYSIZE ( tlIdleResponseTS ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_HEAVY_DAMAGE,
		0,
		"Idle Response"

	},
};

Task_t	tlIdleSpeakTS[] =
{
	{ TASK_TLK_SPEAK,		(float)0		},// question or remark
	{ TASK_TLK_IDEALYAW,	(float)0		},// look at who I'm talking to
	{ TASK_FACE_IDEAL,		(float)0		}, 
	{ TASK_SET_ACTIVITY,	(float)ACT_IDLE	},
	{ TASK_TLK_EYECONTACT,	(float)0		},
	{ TASK_WAIT_RANDOM,		(float)0.5		},
};

Schedule_t	slIdleSpeakTS[] =
{
	{ 
		tlIdleSpeakTS,
		HL_ARRAYSIZE ( tlIdleSpeakTS ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_CLIENT_PUSH	|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_HEAVY_DAMAGE,
		0,
		"Idle Speak"
	},
};

Task_t	tlIdleSpeakTSWait[] =
{
	{ TASK_SET_ACTIVITY,	(float)ACT_IDLE	},// Stop and talk
	{ TASK_TLK_SPEAK,		(float)0		},// question or remark
	{ TASK_TLK_EYECONTACT,	(float)0		},// 
	{ TASK_WAIT,			(float)2		},// wait - used when sci is in 'use' mode to keep head turned
};

Schedule_t	slIdleSpeakTSWait[] =
{
	{ 
		tlIdleSpeakTSWait,
		HL_ARRAYSIZE ( tlIdleSpeakTSWait ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_CLIENT_PUSH	|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_HEAVY_DAMAGE,
		0,
		"Idle Speak Wait"
	},
};

Task_t	tlIdleHelloTS[] =
{
	{ TASK_SET_ACTIVITY,	(float)ACT_IDLE	},// Stop and talk
	{ TASK_TLK_HELLO,		(float)0		},// Try to say hello to player
	{ TASK_TLK_EYECONTACT,	(float)0		},
	{ TASK_WAIT,			(float)0.5		},// wait a bit
	{ TASK_TLK_HELLO,		(float)0		},// Try to say hello to player
	{ TASK_TLK_EYECONTACT,	(float)0		},
	{ TASK_WAIT,			(float)0.5		},// wait a bit
	{ TASK_TLK_HELLO,		(float)0		},// Try to say hello to player
	{ TASK_TLK_EYECONTACT,	(float)0		},
	{ TASK_WAIT,			(float)0.5		},// wait a bit
	{ TASK_TLK_HELLO,		(float)0		},// Try to say hello to player
	{ TASK_TLK_EYECONTACT,	(float)0		},
	{ TASK_WAIT,			(float)0.5		},// wait a bit

};

Schedule_t	slIdleHelloTS[] =
{
	{ 
		tlIdleHelloTS,
		HL_ARRAYSIZE ( tlIdleHelloTS ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_CLIENT_PUSH	|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_PROVOKED,

		bits_SOUND_COMBAT,
		"Idle Hello"
	},
};

Task_t	tlIdleStopShootingTS[] =
{
	{ TASK_TLK_STOPSHOOTING,	(float)0		},// tell player to stop shooting friend
};

Schedule_t	slIdleStopShootingTS[] =
{
	{ 
		tlIdleStopShootingTS,
		HL_ARRAYSIZE ( tlIdleStopShootingTS ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_HEAR_SOUND,
		0,
		"Idle Stop Shooting"
	},
};

Task_t	tlMoveAwayTS[] =
{
	{ TASK_SET_FAIL_SCHEDULE,		(float)SCHED_MOVE_AWAY_FAIL },
	{ TASK_STORE_LASTPOSITION,		(float)0		},
	{ TASK_MOVE_AWAY_PATH,			(float)100		},
	{ TASK_WALK_PATH_FOR_UNITS,		(float)100		},
	{ TASK_STOP_MOVING,				(float)0		},
	{ TASK_FACE_PLAYER,				(float)0.5 },
};

Schedule_t	slMoveAwayTS[] =
{
	{
		tlMoveAwayTS,
		HL_ARRAYSIZE ( tlMoveAwayTS ),
		0,
		0,
		"MoveAway"
	},
};


Task_t	tlMoveAwayTSFail[] =
{
	{ TASK_STOP_MOVING,				(float)0		},
	{ TASK_FACE_PLAYER,				(float)0.5		},
};

Schedule_t	slMoveAwayTSFail[] =
{
	{
		tlMoveAwayTSFail,
		HL_ARRAYSIZE ( tlMoveAwayTSFail ),
		0,
		0,
		"MoveAwayFail"
	},
};



Task_t	tlMoveAwayTSFollow[] =
{
	{ TASK_SET_FAIL_SCHEDULE,		(float)SCHED_TARGET_FACE },
	{ TASK_STORE_LASTPOSITION,		(float)0		},
	{ TASK_MOVE_AWAY_PATH,			(float)100				},
	{ TASK_WALK_PATH_FOR_UNITS,		(float)100		},
	{ TASK_STOP_MOVING,				(float)0		},
	{ TASK_SET_SCHEDULE,			(float)SCHED_TARGET_FACE },
};

Schedule_t	slMoveAwayTSFollow[] =
{
	{
		tlMoveAwayTSFollow,
		HL_ARRAYSIZE ( tlMoveAwayTSFollow ),
		0,
		0,
		"MoveAwayFollow"
	},
};

Task_t	tlTlkIdleWatchClientTS[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE	},
	{ TASK_TLK_LOOK_AT_CLIENT,	(float)6		},
};

Task_t	tlTlkIdleWatchClientTSStare[] =
{
	{ TASK_STOP_MOVING,			0				},
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE	},
	{ TASK_TLK_CLIENT_STARE,	(float)6		},
	{ TASK_TLK_STARE,			(float)0		},
	{ TASK_TLK_IDEALYAW,		(float)0		},// look at who I'm talking to
	{ TASK_FACE_IDEAL,			(float)0		}, 
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE	},
	{ TASK_TLK_EYECONTACT,		(float)0		},
};

Schedule_t	slTlkIdleWatchClientTS[] =
{
	{ 
		tlTlkIdleWatchClientTS,
		HL_ARRAYSIZE ( tlTlkIdleWatchClientTS ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND	|
		bits_COND_SMELL			|
		bits_COND_CLIENT_PUSH	|
		bits_COND_CLIENT_UNSEEN	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_PROVOKED,
		bits_SOUND_COMBAT		|
		bits_SOUND_DANGER		|
		bits_SOUND_MEAT			|// scents
		bits_SOUND_CARCASS		|
		bits_SOUND_GARBAGE,
		"TlkIdleWatchClient"
	},

	{ 
		tlTlkIdleWatchClientTSStare,
		HL_ARRAYSIZE ( tlTlkIdleWatchClientTSStare ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_HEAR_SOUND	|
		bits_COND_SMELL			|
		bits_COND_CLIENT_PUSH	|
		bits_COND_CLIENT_UNSEEN	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_PROVOKED,
		bits_SOUND_COMBAT		|// sound flags - change these, and you'll break the talking code.
		bits_SOUND_DANGER		|
		bits_SOUND_MEAT			|// scents
		bits_SOUND_CARCASS		|
		bits_SOUND_GARBAGE,
		"TlkIdleWatchClientStare"
	},
};


Task_t	tlTlkIdleEyecontactTS[] =
{
	{ TASK_TLK_IDEALYAW,	(float)0		},// look at who I'm talking to
	{ TASK_FACE_IDEAL,		(float)0		}, 
	{ TASK_SET_ACTIVITY,	(float)ACT_IDLE	},
	{ TASK_TLK_EYECONTACT,	(float)0		},// Wait until speaker is done
};

Schedule_t	slTlkIdleEyecontactTS[] =
{
	{ 
		tlTlkIdleEyecontactTS,
		HL_ARRAYSIZE ( tlTlkIdleEyecontactTS ), 
		bits_COND_NEW_ENEMY		|
		bits_COND_CLIENT_PUSH	|
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_MEDIC_HEAL	|
		bits_COND_HEAVY_DAMAGE,
		0,
		"TlkIdleEyecontact"
	},
};
DEFINE_CUSTOM_SCHEDULES( CRCAllyMonster )
{
	slIdleResponseTS,
	slIdleSpeakTS,
	slIdleHelloTS,
	slIdleSpeakTSWait,
	slIdleStopShootingTS,
	slMoveAwayTS,
	slMoveAwayTSFollow,
	slMoveAwayTSFail,
	slTlkIdleWatchClientTS,
	&slTlkIdleWatchClientTS[ 1 ],
	slTlkIdleEyecontactTS,
};

IMPLEMENT_CUSTOM_SCHEDULES( CRCAllyMonster, CBaseMonster );


void CRCAllyMonster :: SetActivity ( Activity newActivity )
{
	if (newActivity == ACT_IDLE && IsTalking() )
		newActivity = ACT_IDLE;
	
	if ( newActivity == ACT_IDLE && (LookupActivity ( ACT_IDLE ) == ACTIVITY_NOT_AVAILABLE))
		newActivity = ACT_IDLE;

	CBaseMonster::SetActivity( newActivity );
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CRCAllyMonster::Classify(void) {
	return m_iClass ? m_iClass : CLASS_PLAYER_ALLY;
}

void CRCAllyMonster :: StartTask( Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_TLK_SPEAK:
		// ask question or make statement
		FIdleSpeak();
		TaskComplete();
		break;

	case TASK_TLK_RESPOND:
		// respond to question
		IdleRespond();
		TaskComplete();
		break;

	case TASK_TLK_HELLO:
		// greet player
		FIdleHello();
		TaskComplete();
		break;
	

	case TASK_TLK_STARE:
		// let the player know I know he's staring at me.
		FIdleStare();
		TaskComplete();
		break;

	case TASK_FACE_PLAYER:
	case TASK_TLK_LOOK_AT_CLIENT:
	case TASK_TLK_CLIENT_STARE:
		// track head to the client for a while.
		m_flWaitFinished = UTIL_GlobalTimeBase() + pTask->flData;
		break;

	case TASK_TLK_EYECONTACT:
		break;

	case TASK_TLK_IDEALYAW:
		if (m_hTalkTarget != NULL)
		{
			pev->yaw_speed = 60;
			float yaw = VecToYaw(m_hTalkTarget->pev->origin - pev->origin) - pev->angles.y;

			if (yaw > 180) yaw -= 360;
			if (yaw < -180) yaw += 360;

			if (yaw < 0)
			{
				pev->ideal_yaw = V_min( yaw + 45, 0 ) + pev->angles.y;
			}
			else
			{
				pev->ideal_yaw = V_max( yaw - 45, 0 ) + pev->angles.y;
			}
		}
		TaskComplete();
		break;

	case TASK_TLK_HEADRESET:
		// reset head position after looking at something
		m_hTalkTarget = NULL;
		TaskComplete();
		break;

	case TASK_TLK_STOPSHOOTING:
		// tell player to stop shooting
		PlaySentence( m_szGrp[TLK_NOSHOOT], RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_NORM );
		TaskComplete();
		break;

	case TASK_CANT_FOLLOW:
		StopFollowing( FALSE );
		PlaySentence( m_szGrp[TLK_STOP], RANDOM_FLOAT(2, 2.5), VOL_NORM, ATTN_NORM );
		TaskComplete();
		break;

	case TASK_WALK_PATH_FOR_UNITS:
		m_movementActivity = ACT_WALK;
		break;

	case TASK_MOVE_AWAY_PATH:
		{
			Vector dir = pev->angles;
			dir.y = pev->ideal_yaw + 180;
			Vector move;

			UTIL_MakeVectorsPrivate( dir, move, NULL, NULL );
			dir = pev->origin + move * pTask->flData;
			if ( MoveToLocation( ACT_WALK, 2, dir ) )
			{
				TaskComplete();
			}
			else if ( FindCover( pev->origin, pev->view_ofs, 0, CoverRadius() ) )
			{
				// then try for plain ole cover
				m_flMoveWaitFinished = UTIL_GlobalTimeBase() + 2;
				TaskComplete();
			}
			else
			{
				// nowhere to go?
				TaskFail();
			}
		}
		break;

	case TASK_PLAY_SCRIPT:
		m_hTalkTarget = NULL;
		CBaseMonster::StartTask( pTask );
		break;

	default:
		CBaseMonster::StartTask( pTask );
	}
}

void CRCAllyMonster :: RunTask( Task_t *pTask )
{
	switch( pTask->iTask )
	{
	case TASK_TLK_CLIENT_STARE:
	case TASK_TLK_LOOK_AT_CLIENT:

		edict_t *pPlayer;

		// track head to the client for a while.
		if ( m_MonsterState == MONSTERSTATE_IDLE		&& 
			 !IsMoving()								&&
			 !IsTalking()								)
		{
			// Get edict for one player
			pPlayer = g_engfuncs.pfnPEntityOfEntIndex( 1 );

			if ( pPlayer )
			{
				IdleHeadTurn( pPlayer->v.origin );
			}
		}
		else
		{
			// started moving or talking
			TaskFail();
			return;
		}

		if ( pTask->iTask == TASK_TLK_CLIENT_STARE )
		{
			// fail out if the player looks away or moves away.
			if ( ( pPlayer->v.origin - pev->origin ).Length2D() > TLK_STARE_DIST )
			{
				// player moved away.
				TaskFail();
			}

			UTIL_MakeVectors( pPlayer->v.angles );
			if ( UTIL_DotPoints( pPlayer->v.origin, pev->origin, gpGlobals->v_forward ) < m_flFieldOfView )
			{
				// player looked away
				TaskFail();
			}
		}

		if ( UTIL_GlobalTimeBase() > m_flWaitFinished )
		{
			TaskComplete();
		}
		break;

	case TASK_FACE_PLAYER:
		{
			// Get edict for one player
			edict_t *pPlayer = g_engfuncs.pfnPEntityOfEntIndex( 1 );

			if ( pPlayer )
			{
				MakeIdealYaw ( pPlayer->v.origin );
				ChangeYaw ( pev->yaw_speed );
				IdleHeadTurn( pPlayer->v.origin );
				if ( UTIL_GlobalTimeBase() > m_flWaitFinished && FlYawDiff() < 10 )
				{
					TaskComplete();
				}
			}
			else
			{
				TaskFail();
			}
		}
		break;

	case TASK_TLK_EYECONTACT:
		if (!IsMoving() && IsTalking() && m_hTalkTarget != NULL)
		{
			// ALERT( at_console, "waiting %f\n", m_flStopTalkTime - UTIL_GlobalTimeBase() );
			IdleHeadTurn( m_hTalkTarget->pev->origin );
		}
		else
		{
			TaskComplete();
		}
		break;

	case TASK_WALK_PATH_FOR_UNITS:
		{
			float distance;

			distance = (m_vecLastPosition - pev->origin).Length2D();

			// Walk path until far enough away
			if ( distance > pTask->flData || MovementIsComplete() )
			{
				TaskComplete();
				RouteClear();		// Stop moving
			}
		}
		break;
	case TASK_WAIT_FOR_MOVEMENT:
		if (IsTalking() && m_hTalkTarget != NULL)
		{
			// ALERT(at_console, "walking, talking\n");
			IdleHeadTurn( m_hTalkTarget->pev->origin );
		}
		else
		{
			IdleHeadTurn( pev->origin );
			// override so that during walk, a scientist may talk and greet player
			FIdleHello();
			if (RANDOM_LONG(0,m_nSpeak * 20) == 0)
			{
				FIdleSpeak();
			}
		}

		CBaseMonster::RunTask( pTask );
		if (TaskIsComplete())
			IdleHeadTurn( pev->origin );
		break;

	default:
		if (IsTalking() && m_hTalkTarget != NULL)
		{
			IdleHeadTurn( m_hTalkTarget->pev->origin );
		}
		else
		{
			SetBoneController( 0, 0 );
		}
		CBaseMonster::RunTask( pTask );
	}
}

//=========================================================
// Killed
//=========================================================
void CRCAllyMonster :: Killed( entvars_t *pevAttacker, int iGib ) {
	VacateSlot();

	if (InSquad()) {
		MySquadLeader()->SquadRemove(this);
	}

	if ( (pevAttacker->flags & FL_CLIENT) && m_MonsterState != MONSTERSTATE_PRONE ) {
		AlertFriends();
	}

	m_hTargetEnt = NULL;
	StopTalking();
	SetUse( NULL );
	CBaseMonster::Killed( pevAttacker, iGib );
}

//=========================================================
// EnumFriends
//=========================================================
CBaseEntity	*CRCAllyMonster::EnumFriends( CBaseEntity *pPrevious, int listNumber, BOOL bTrace ) {
	CBaseEntity *pFriend = pPrevious;
	char *pszFriend;
	TraceResult tr;
	Vector vecCheck;

	pszFriend = m_szFriends[ FriendNumber(listNumber) ];
	while (pFriend = UTIL_FindEntityByClassname( pFriend, pszFriend ))
	{
		if (pFriend == this || !pFriend->IsAlive())
			// don't talk to self or dead people
			continue;
		if ( bTrace )
		{
			vecCheck = pFriend->pev->origin;
			vecCheck.z = pFriend->pev->absmax.z;

			UTIL_TraceLine( pev->origin, vecCheck, ignore_monsters, ENT(pev), &tr);
		}
		else
			tr.flFraction = 1.0;

		if (tr.flFraction == 1.0)
		{
			return pFriend;
		}
	}

	return NULL;
}

//=========================================================
// AlertFriends
//=========================================================
void CRCAllyMonster::AlertFriends( void ) {
	CBaseEntity *pFriend = NULL;
	int i;

	// for each friend in this bsp...
	for ( i = 0; i < TLK_CFRIENDS; i++ ) {
		while (pFriend = EnumFriends( pFriend, i, TRUE )) {
			CBaseMonster *pMonster = pFriend->MyMonsterPointer();
			if ( pMonster->IsAlive() ) {
				// don't provoke a friend that's playing a death animation. They're a goner
				pMonster->m_afMemory |= bits_MEMORY_PROVOKED;
			}
		}
	}
}

void CRCAllyMonster::ShutUpFriends( void )
{
	CBaseEntity *pFriend = NULL;
	int i;

	// for each friend in this bsp...
	for ( i = 0; i < TLK_CFRIENDS; i++ )
	{
		while (pFriend = EnumFriends( pFriend, i, TRUE ))
		{
			CBaseMonster *pMonster = pFriend->MyMonsterPointer();
			if ( pMonster )
			{
				pMonster->SentenceStop();
			}
		}
	}
}

float CRCAllyMonster::TargetDistance( void )
{
	// If we lose the player, or he dies, return a really large distance
	if ( m_hTargetEnt == NULL || !m_hTargetEnt->IsAlive() )
		return 1e6;

	return (m_hTargetEnt->pev->origin - pev->origin).Length();
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CRCAllyMonster :: HandleAnimEvent( MonsterEvent_t *pEvent )
{
	switch( pEvent->event )
	{		
	case SCRIPT_EVENT_SENTENCE_RND1:		// Play a named sentence group 25% of the time
		if (RANDOM_LONG(0,99) < 75)
			break;
		// fall through...
	case SCRIPT_EVENT_SENTENCE:				// Play a named sentence group
		ShutUpFriends();
		PlaySentence( pEvent->options, RANDOM_FLOAT(2.8, 3.4), VOL_NORM, ATTN_IDLE );
		//ALERT(at_console, "script event speak\n");
		break;

	default:
		CBaseMonster::HandleAnimEvent( pEvent );
		break;
	}
}

//=========================================================
// Init talk data
//=========================================================
void CRCAllyMonster :: TalkInit( void ) {
	CRCAllyMonster::g_talkWaitTime = 0;

	if (m_iszSpeakAs)  {
		char szBuf[64];
		strcpy(szBuf,STRING(m_iszSpeakAs));
		strcat(szBuf,"_");
		char *szAssign = &(szBuf[strlen(szBuf)]);

		//LRC - this is pretty dodgy; test with save/restore.
		strcpy(szAssign,"ANSWER");
		m_szGrp[TLK_ANSWER]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"QUESTION");
		m_szGrp[TLK_QUESTION]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"IDLE");
		m_szGrp[TLK_IDLE]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"STARE");
		m_szGrp[TLK_STARE]  = STRING(ALLOC_STRING(szBuf));
		if (pev->spawnflags & SF_MONSTER_PREDISASTER) //LRC
			strcpy(szAssign,"PFOLLOW");
		else
			strcpy(szAssign,"OK");
		m_szGrp[TLK_USE]  = STRING(ALLOC_STRING(szBuf));
		if (pev->spawnflags & SF_MONSTER_PREDISASTER) //LRC
			strcpy(szAssign,"PWAIT");
		else
			strcpy(szAssign,"WAIT");
		m_szGrp[TLK_UNUSE]  = STRING(ALLOC_STRING(szBuf));
		if (pev->spawnflags & SF_MONSTER_PREDISASTER) //LRC
			strcpy(szAssign,"POK");
		else
			strcpy(szAssign,"NOTOK");
		m_szGrp[TLK_DECLINE]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"STOP");
		m_szGrp[TLK_STOP]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"NOSHOOT");
		m_szGrp[TLK_NOSHOOT]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"HELLO");
		m_szGrp[TLK_HELLO]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"PLHURT1");
		m_szGrp[TLK_PLHURT1]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"PLHURT2");
		m_szGrp[TLK_PLHURT2]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"PLHURT3");
		m_szGrp[TLK_PLHURT3]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"PHELLO");
		m_szGrp[TLK_PHELLO]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"PIDLE");
		m_szGrp[TLK_PIDLE]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"PQUESTION");
		m_szGrp[TLK_PQUESTION]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"SMELL");
		m_szGrp[TLK_SMELL]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"WOUND");
		m_szGrp[TLK_WOUND]  = STRING(ALLOC_STRING(szBuf));
		strcpy(szAssign,"MORTAL");
		m_szGrp[TLK_MORTAL]  = STRING(ALLOC_STRING(szBuf));
	} else {
		m_szGrp[TLK_ANSWER] = "FG_ANSWER";
		m_szGrp[TLK_QUESTION] = "FG_QUESTION";
		m_szGrp[TLK_IDLE] = "FG_IDLE";
		m_szGrp[TLK_STARE] = "FG_STARE";
		m_szGrp[TLK_USE] = "FG_OK";
		m_szGrp[TLK_UNUSE] = "FG_WAIT";
		m_szGrp[TLK_STOP] = "FG_STOP";

		m_szGrp[TLK_NOSHOOT] = "FG_SCARED";
		m_szGrp[TLK_HELLO] = "FG_HELLO";

		m_szGrp[TLK_PLHURT1] = "FG_CURE";
		m_szGrp[TLK_PLHURT2] = "FG_CURE";
		m_szGrp[TLK_PLHURT3] = "FG_CURE";

		m_szGrp[TLK_SMELL] = "FG_SMELL";

		m_szGrp[TLK_WOUND] = "FG_WOUND";
		m_szGrp[TLK_MORTAL] = "FG_MORTAL";
	}

	m_voicePitch = (90 + RANDOM_LONG(0, 10));
}	
//=========================================================
// FindNearestFriend
// Scan for nearest, visible friend. If fPlayer is true, look for
// nearest player
//=========================================================
CBaseEntity *CRCAllyMonster :: FindNearestFriend(BOOL fPlayer)
{
	CBaseEntity *pFriend = NULL;
	CBaseEntity *pNearest = NULL;
	float range = 10000000.0;
	TraceResult tr;
	Vector vecStart = pev->origin;
	Vector vecCheck;
	int i;
	char *pszFriend;
	int cfriends;

	vecStart.z = pev->absmax.z;
	
	if (fPlayer)
		cfriends = 1;
	else
		cfriends = TLK_CFRIENDS;

	// for each type of friend...

	for (i = cfriends-1; i > -1; i--)
	{
		if (fPlayer)
			pszFriend = "player";
		else
			pszFriend = m_szFriends[FriendNumber(i)];

		if (!pszFriend)
			continue;

		// for each friend in this bsp...
		while (pFriend = UTIL_FindEntityByClassname( pFriend, pszFriend ))
		{
			if (pFriend == this || !pFriend->IsAlive())
				// don't talk to self or dead people
				continue;

			CBaseMonster *pMonster = pFriend->MyMonsterPointer();

			// If not a monster for some reason, or in a script, or prone
			if ( !pMonster || pMonster->m_MonsterState == MONSTERSTATE_SCRIPT || pMonster->m_MonsterState == MONSTERSTATE_PRONE )
				continue;

			vecCheck = pFriend->pev->origin;
			vecCheck.z = pFriend->pev->absmax.z;

			// if closer than previous friend, and in range, see if he's visible

			if (range > (vecStart - vecCheck).Length())
			{
				UTIL_TraceLine(vecStart, vecCheck, ignore_monsters, ENT(pev), &tr);

				if (tr.flFraction == 1.0)
				{
					// visible and in range, this is the new nearest scientist
					if ((vecStart - vecCheck).Length() < TALKRANGE_MIN)
					{
						pNearest = pFriend;
						range = (vecStart - vecCheck).Length();
					}
				}
			}
		}
	}
	return pNearest;
}

int CRCAllyMonster :: GetVoicePitch( void )
{
	return m_voicePitch + RANDOM_LONG(0,3);
}

void CRCAllyMonster :: Touch( CBaseEntity *pOther )
{
	// Did the player touch me?
	if ( pOther->IsPlayer() )
	{
		// Ignore if pissed at player
		if ( m_afMemory & bits_MEMORY_PROVOKED )
			return;

		// Stay put during speech
		if ( IsTalking() )
			return;

		// Heuristic for determining if the player is pushing me away
		float speed = V_fabs(pOther->pev->velocity.x) + fabs(pOther->pev->velocity.y);
		if ( speed > 50 )
		{
			SetConditions( bits_COND_CLIENT_PUSH );
			MakeIdealYaw( pOther->pev->origin );
		}
	}
}

//=========================================================
// IdleRespond
// Respond to a previous question
//=========================================================
void CRCAllyMonster :: IdleRespond( void )
{
	int pitch = GetVoicePitch();
	
	// play response
	PlaySentence( m_szGrp[TLK_ANSWER], RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE );
}

//=========================================================
// someone else is talking - don't speak
//=========================================================
int CRCAllyMonster :: FOkToSpeak( void ) {
	// if in the grip of a barnacle, don't speak
	if ( m_MonsterState == MONSTERSTATE_PRONE || m_IdealMonsterState == MONSTERSTATE_PRONE ) {
		return FALSE;
	}

	// if not alive, certainly don't speak
	if ( pev->deadflag != DEAD_NO ) {
		return FALSE;
	}

	// if someone else is talking, don't speak
	if (UTIL_GlobalTimeBase() <= CRCAllyMonster::g_talkWaitTime)
		return FALSE;

	if ( pev->spawnflags & SF_MONSTER_GAG )
		return FALSE;

	// if player is not in pvs, don't speak
	if (!IsAlive() || FNullEnt(FIND_CLIENT_IN_PVS(edict())))
		return FALSE;

	// don't talk if you're in combat
	if (m_hEnemy != NULL && FVisible( m_hEnemy ))
		return FALSE;

	return TRUE;
}

int CRCAllyMonster::CanPlaySentence( BOOL fDisregardState ) 
{ 
	if ( fDisregardState )
		return CBaseMonster::CanPlaySentence( fDisregardState );
	return FOkToSpeak(); 
}

//=========================================================
// FIdleStare
//=========================================================
int CRCAllyMonster :: FIdleStare( void )
{
	if (!FOkToSpeak())
		return FALSE;

	PlaySentence( m_szGrp[TLK_STARE], RANDOM_FLOAT(5, 7.5), VOL_NORM, ATTN_IDLE );

	m_hTalkTarget = FindNearestFriend( TRUE );
	return TRUE;
}

//=========================================================
// IdleHello
// Try to greet player first time he's seen
//=========================================================
int CRCAllyMonster :: FIdleHello( void )
{
	if (!FOkToSpeak())
		return FALSE;

	// if this is first time scientist has seen player, greet him
	if (!FBitSet(m_bitsSaid, bit_saidHelloPlayer))
	{
		// get a player
		CBaseEntity *pPlayer = FindNearestFriend(TRUE);

		if (pPlayer)
		{
			if (FInViewCone(pPlayer) && FVisible(pPlayer))
			{
				m_hTalkTarget = pPlayer;

				if (FBitSet(pev->spawnflags, SF_MONSTER_PREDISASTER))
					PlaySentence( m_szGrp[TLK_PHELLO], RANDOM_FLOAT(3, 3.5), VOL_NORM,  ATTN_IDLE );
				else
					PlaySentence( m_szGrp[TLK_HELLO], RANDOM_FLOAT(3, 3.5), VOL_NORM,  ATTN_IDLE );

				SetBits(m_bitsSaid, bit_saidHelloPlayer);
				
				return TRUE;
			}
		}
	}
	return FALSE;
}

// turn head towards supplied origin
void CRCAllyMonster :: IdleHeadTurn( Vector &vecFriend )
{
	 // turn head in desired direction only if ent has a turnable head
	if (m_afCapability & bits_CAP_TURN_HEAD)
	{
		float yaw = VecToYaw(vecFriend - pev->origin) - pev->angles.y;

		if (yaw > 180) yaw -= 360;
		if (yaw < -180) yaw += 360;

		// turn towards vector
		SetBoneController( 0, yaw );
	}
}

//=========================================================
// FIdleSpeak
// ask question of nearby friend, or make statement
//=========================================================
int CRCAllyMonster :: FIdleSpeak ( void )
{ 
	// try to start a conversation, or make statement
	int pitch;
	const char *szIdleGroup;
	const char *szQuestionGroup;
	float duration;

	if (!FOkToSpeak())
		return FALSE;

	// set idle groups based on pre/post disaster
	if (FBitSet(pev->spawnflags, SF_MONSTER_PREDISASTER))
	{
		szIdleGroup = m_szGrp[TLK_PIDLE];
		szQuestionGroup = m_szGrp[TLK_PQUESTION];
		// set global min delay for next conversation
		duration = RANDOM_FLOAT(4.8, 5.2);
	}
	else
	{
		szIdleGroup = m_szGrp[TLK_IDLE];
		szQuestionGroup = m_szGrp[TLK_QUESTION];
		// set global min delay for next conversation
		duration = RANDOM_FLOAT(2.8, 3.2);

	}

	pitch = GetVoicePitch();
		
	// player using this entity is alive and wounded?
	CBaseEntity *pTarget = m_hTargetEnt;

	if ( pTarget != NULL )
	{
		if ( pTarget->IsPlayer() )
		{
			if ( pTarget->IsAlive() )
			{
				m_hTalkTarget = m_hTargetEnt;
				if (!FBitSet(m_bitsSaid, bit_saidDamageHeavy) && 
					(m_hTargetEnt->pev->health <= m_hTargetEnt->pev->max_health / 8))
				{
					//EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, m_szGrp[TLK_PLHURT3], 1.0, ATTN_IDLE, 0, pitch);
					PlaySentence( m_szGrp[TLK_PLHURT3], duration, VOL_NORM, ATTN_IDLE );
					SetBits(m_bitsSaid, bit_saidDamageHeavy);
					return TRUE;
				}
				else if (!FBitSet(m_bitsSaid, bit_saidDamageMedium) && 
					(m_hTargetEnt->pev->health <= m_hTargetEnt->pev->max_health / 4))
				{
					//EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, m_szGrp[TLK_PLHURT2], 1.0, ATTN_IDLE, 0, pitch);
					PlaySentence( m_szGrp[TLK_PLHURT2], duration, VOL_NORM, ATTN_IDLE );
					SetBits(m_bitsSaid, bit_saidDamageMedium);
					return TRUE;
				}
				else if (!FBitSet(m_bitsSaid, bit_saidDamageLight) &&
					(m_hTargetEnt->pev->health <= m_hTargetEnt->pev->max_health / 2))
				{
					//EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, m_szGrp[TLK_PLHURT1], 1.0, ATTN_IDLE, 0, pitch);
					PlaySentence( m_szGrp[TLK_PLHURT1], duration, VOL_NORM, ATTN_IDLE );
					SetBits(m_bitsSaid, bit_saidDamageLight);
					return TRUE;
				}
			}
			else
			{
				//!!!KELLY - here's a cool spot to have the talkmonster talk about the dead player if we want.
				// "Oh dear, Gordon Freeman is dead!" -Scientist
				// "Damn, I can't do this without you." -Barney
			}
		}
	}

	// if there is a friend nearby to speak to, play sentence, set friend's response time, return
	CBaseEntity *pFriend = FindNearestFriend(FALSE);

	if (pFriend && !(pFriend->IsMoving()) && (RANDOM_LONG(0,99) < 75))
	{
		PlaySentence( szQuestionGroup, duration, VOL_NORM, ATTN_IDLE );
		//SENTENCEG_PlayRndSz( ENT(pev), szQuestionGroup, 1.0, ATTN_IDLE, 0, pitch );

		// force friend to answer
		CRCAllyMonster *pTalkMonster = (CRCAllyMonster *)pFriend;
		m_hTalkTarget = pFriend;
		pTalkMonster->SetAnswerQuestion( this ); // UNDONE: This is EVIL!!!
		pTalkMonster->m_flStopTalkTime = m_flStopTalkTime;

		m_nSpeak++;
		return TRUE;
	}

	// otherwise, play an idle statement, try to face client when making a statement.
	if ( RANDOM_LONG(0,1) )
	{
		//SENTENCEG_PlayRndSz( ENT(pev), szIdleGroup, 1.0, ATTN_IDLE, 0, pitch );
		CBaseEntity *pFriend = FindNearestFriend(TRUE);

		if ( pFriend )
		{
			m_hTalkTarget = pFriend;
			PlaySentence( szIdleGroup, duration, VOL_NORM, ATTN_IDLE );
			m_nSpeak++;
			return TRUE;
		}
	}

	// didn't speak
	Talk( 0 );
	CRCAllyMonster::g_talkWaitTime = 0;
	return FALSE;
}

void CRCAllyMonster::PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener )
{
	if ( !bConcurrent )
		ShutUpFriends();

	ClearConditions( bits_COND_CLIENT_PUSH );	// Forget about moving!  I've got something to say!
	m_useTime = UTIL_GlobalTimeBase() + duration;
	PlaySentence( pszSentence, duration, volume, attenuation );

	m_hTalkTarget = pListener;
}

void CRCAllyMonster::PlaySentence( const char *pszSentence, float duration, float volume, float attenuation )
{
	if ( !pszSentence )
		return;

	Talk ( duration );

	CRCAllyMonster::g_talkWaitTime = UTIL_GlobalTimeBase() + duration + 2.0;
	if ( pszSentence[0] == '!' )
		EMIT_SOUND_DYN( edict(), CHAN_VOICE, pszSentence, volume, attenuation, 0, GetVoicePitch());
	else
		SENTENCEG_PlayRndSz( edict(), pszSentence, volume, attenuation, 0, GetVoicePitch() );

	// If you say anything, don't greet the player - you may have already spoken to them
	SetBits(m_bitsSaid, bit_saidHelloPlayer);
}

//=========================================================
// Talk - set a timer that tells us when the monster is done
// talking.
//=========================================================
void CRCAllyMonster :: Talk( float flDuration )
{
	if ( flDuration <= 0 )
	{
		// no duration :( 
		m_flStopTalkTime = UTIL_GlobalTimeBase() + 3;
	}
	else
	{
		m_flStopTalkTime = UTIL_GlobalTimeBase() + flDuration;
	}
}

// Prepare this talking monster to answer question
void CRCAllyMonster :: SetAnswerQuestion( CRCAllyMonster *pSpeaker )
{
	if ( !m_pCine )
		ChangeSchedule( slIdleResponseTS );
	m_hTalkTarget = (CBaseMonster *)pSpeaker;
}

//=========================================================
// TraceAttack - Double damage fix
//=========================================================
void CRCAllyMonster::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
		return;
	}

	if (pev->takedamage) {
		if (IsAlive() && RANDOM_LONG(0, 4) <= 2) { PainSound(); }
		if (pev->spawnflags & SF_MONSTER_INVINCIBLE) {
			CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
			if (pEnt->IsPlayer()) { CBaseMonster::TraceAttack(pevAttacker, 0, vecDir, ptr, bitsDamageType); }
			if (pevAttacker->owner) {
				pEnt = CBaseEntity::Instance(pevAttacker->owner);
				if (pEnt->IsPlayer()) { CBaseMonster::TraceAttack(pevAttacker, 0, vecDir, ptr, bitsDamageType); }
			}
		}

		switch (ptr->iHitgroup) {
		case HITGROUP_HEAD:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_HEAD\n", STRING(pev->classname));

			if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_CLUB)) {
				flDamage -= 20;
				if (flDamage <= 0) {
					UTIL_Ricochet(ptr->vecEndPos, 1.0);
					flDamage = 0.01;
				}
			} else {
				flDamage = m_flHitgroupHead*flDamage;
			}
			ptr->iHitgroup = HITGROUP_HEAD;
			break;
		case HITGROUP_CHEST:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_CHEST\n", STRING(pev->classname));
			flDamage = m_flHitgroupChest*flDamage;
			break;
		case HITGROUP_STOMACH:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_STOMACH\n", STRING(pev->classname));
			flDamage = m_flHitgroupStomach*flDamage;
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
		}
	}

	SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);// a little surface blood.
	TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
	CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}

extern Schedule_t	slChaseEnemyFailed[];

Schedule_t* CRCAllyMonster :: GetScheduleOfType ( int iType)
{
	switch(iType)
	{
	case SCHED_MOVE_AWAY:
		return slMoveAwayTS;

	case SCHED_MOVE_AWAY_FOLLOW:
		return slMoveAwayTSFollow;

	case SCHED_MOVE_AWAY_FAIL:
		return slMoveAwayTSFail;

	case SCHED_TARGET_FACE:
		// speak during 'use'
		if (RANDOM_LONG(0,99) < 2)
			//ALERT ( at_console, "target chase speak\n" );
			return slIdleSpeakTSWait;
		else
			return slIdleStand;

	case SCHED_CHASE_ENEMY_FAILED:
		{
			return &slChaseEnemyFailed[ 0 ];
		}

	case SCHED_IDLE_STAND:
		{	
			// if never seen player, try to greet him
			if (!FBitSet(m_bitsSaid, bit_saidHelloPlayer))
			{
				return slIdleHelloTS;
			}

			// sustained light wounds?
			if (!FBitSet(m_bitsSaid, bit_saidWoundLight) && (pev->health <= (pev->max_health * 0.75)))
			{
				//SENTENCEG_PlayRndSz( ENT(pev), m_szGrp[TLK_WOUND], 1.0, ATTN_IDLE, 0, GetVoicePitch() );
				//CRCAllyMonster::g_talkWaitTime = UTIL_GlobalTimeBase() + RANDOM_FLOAT(2.8, 3.2);
				PlaySentence( m_szGrp[TLK_WOUND], RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE );
				SetBits(m_bitsSaid, bit_saidWoundLight);
				return slIdleStand;
			}
			// sustained heavy wounds?
			else if (!FBitSet(m_bitsSaid, bit_saidWoundHeavy) && (pev->health <= (pev->max_health * 0.5)))
			{
				//SENTENCEG_PlayRndSz( ENT(pev), m_szGrp[TLK_MORTAL], 1.0, ATTN_IDLE, 0, GetVoicePitch() );
				//CRCAllyMonster::g_talkWaitTime = UTIL_GlobalTimeBase() + RANDOM_FLOAT(2.8, 3.2);
				PlaySentence( m_szGrp[TLK_MORTAL], RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE );
				SetBits(m_bitsSaid, bit_saidWoundHeavy);
				return slIdleStand;
			}

			// talk about world
			if (FOkToSpeak() && RANDOM_LONG(0,m_nSpeak * 2) == 0)
			{
				//ALERT ( at_console, "standing idle speak\n" );
				return slIdleSpeakTS;
			}
			
			if ( !IsTalking() && HasConditions ( bits_COND_SEE_CLIENT ) && RANDOM_LONG( 0, 6 ) == 0 )
			{
				edict_t *pPlayer = g_engfuncs.pfnPEntityOfEntIndex( 1 );
				if ( pPlayer )
				{
					// watch the client.
					UTIL_MakeVectors ( pPlayer->v.angles );
					if ( ( pPlayer->v.origin - pev->origin ).Length2D() < TLK_STARE_DIST	&& 
						 UTIL_DotPoints( pPlayer->v.origin, pev->origin, gpGlobals->v_forward ) >= m_flFieldOfView )
					{
						// go into the special STARE schedule if the player is close, and looking at me too.
						return &slTlkIdleWatchClientTS[ 1 ];
					}

					return slTlkIdleWatchClientTS;
				} else {
					return CBaseMonster::GetScheduleOfType(iType);
				}
			}
			else
			{
				if (IsTalking())
					// look at who we're talking to
					return slTlkIdleEyecontactTS;
				else
					// regular standing idle
					return slIdleStand;
			}
		}
		break;
		default:
			return CBaseMonster::GetScheduleOfType(iType);
	}
}

//=========================================================
// IsTalking - am I saying a sentence right now?
//=========================================================
BOOL CRCAllyMonster :: IsTalking( void )
{
	if ( m_flStopTalkTime > UTIL_GlobalTimeBase() ) {
		return TRUE;
	}

	return FALSE;
}

//=========================================================
// PrescheduleThink - this function runs after conditions
// are collected and before scheduling code is run.
//=========================================================
void CRCAllyMonster :: PrescheduleThink ( void ) {
	if (InSquad() && m_hEnemy != NULL) {
		if (HasConditions(bits_COND_SEE_ENEMY)) {
			// update the squad's last enemy sighting time.
			MySquadLeader()->m_flLastEnemySightTime = UTIL_GlobalTimeBase();
		} else {
			if (UTIL_GlobalTimeBase() - MySquadLeader()->m_flLastEnemySightTime > 5) {
				// been a while since we've seen the enemy
				MySquadLeader()->m_fEnemyEluded = TRUE;
			}
		}
	}

	if ( !HasConditions ( bits_COND_SEE_CLIENT ) ) {
		SetConditions ( bits_COND_CLIENT_UNSEEN );
	}
}

// try to smell something
void CRCAllyMonster :: TrySmellTalk( void )
{
	if ( !FOkToSpeak() )
		return;

	// clear smell bits periodically
	if ( UTIL_GlobalTimeBase() > m_flLastSaidSmelled  )
	{
//		ALERT ( at_aiconsole, "Clear smell bits\n" );
		ClearBits(m_bitsSaid, bit_saidSmelled);
	}
	// smelled something?
	if (!FBitSet(m_bitsSaid, bit_saidSmelled) && HasConditions ( bits_COND_SMELL ))
	{
		PlaySentence( m_szGrp[TLK_SMELL], RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE );
		m_flLastSaidSmelled = UTIL_GlobalTimeBase() + 60;// don't talk about the stinky for a while.
		SetBits(m_bitsSaid, bit_saidSmelled);
	}
}

//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards. 
//=========================================================
int CRCAllyMonster::ISoundMask(void) {
	return	bits_SOUND_WORLD |
		bits_SOUND_COMBAT |
		bits_SOUND_CARCASS |
		bits_SOUND_MEAT |
		bits_SOUND_GARBAGE |
		bits_SOUND_DANGER |
		bits_SOUND_PLAYER;
}

//=========================================================
// IRelationship - overridden because Male Assassins are 
// Human Grunt's nemesis.
//=========================================================
int CRCAllyMonster::IRelationship( CBaseEntity *pTarget ) {
	//LRC- only hate alien grunts if my behaviour hasn't been overridden
	if (!m_iClass && FClassnameIs(pTarget->pev, "monster_alien_grunt") ||
		(FClassnameIs(pTarget->pev, "monster_gargantua")) ||
		FClassnameIs(pTarget->pev, "monster_male_assassin")) {
		return R_NM;
	}

	if ( pTarget->IsPlayer() )
		if ( m_afMemory & bits_MEMORY_PROVOKED )
			return R_HT;

	return CBaseMonster::IRelationship( pTarget );
}


void CRCAllyMonster::StopFollowing( BOOL clearSchedule )
{
	if ( IsFollowing() )
	{
		if ( !(m_afMemory & bits_MEMORY_PROVOKED) )
		{
			PlaySentence( m_szGrp[TLK_UNUSE], RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE );
			m_hTalkTarget = m_hTargetEnt;
		}

		if ( m_movementGoal == MOVEGOAL_TARGETENT )
			RouteClear(); // Stop him from walking toward the player
		m_hTargetEnt = NULL;
		if ( clearSchedule )
			ClearSchedule();
		if ( m_hEnemy != NULL )
			m_IdealMonsterState = MONSTERSTATE_COMBAT;
	}
}


void CRCAllyMonster::StartFollowing( CBaseEntity *pLeader )
{
	if ( m_pCine )
		m_pCine->CancelScript();

	if ( m_hEnemy != NULL )
		m_IdealMonsterState = MONSTERSTATE_ALERT;

	m_hTargetEnt = pLeader;
	PlaySentence( m_szGrp[TLK_USE], RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE );
	m_hTalkTarget = m_hTargetEnt;
	ClearConditions( bits_COND_CLIENT_PUSH );
	ClearSchedule();
}


BOOL CRCAllyMonster::CanFollow( void )
{
	if (m_MonsterState == MONSTERSTATE_SCRIPT)
	{
		if (!m_pCine->CanInterrupt())
			return FALSE;
	}

	if (!IsAlive())
		return FALSE;

	return TRUE;
}

//LRC- rewritten
void CRCAllyMonster :: FollowerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Don't allow use during a scripted_sentence
	if (m_useTime > UTIL_GlobalTimeBase())
		return;

	//ALERT(at_console,"Talkmonster was Used: ");

	// CanFollow is now true if the monster could physically follow anyone
	if (pCaller != NULL && pCaller->IsPlayer() && CanFollow())
	{
		if (!IsFollowing())
		{
			// Pre-disaster followers can't be used unless they've got a master to override their behaviour...
			if (IsLockedByMaster() || (pev->spawnflags & SF_MONSTER_PREDISASTER && !m_sMaster))
			{
				//ALERT(at_console,"Decline\n");
				DeclineFollowing();
			}
			else
			{
				if (m_afMemory & bits_MEMORY_PROVOKED)
				{
					//ALERT(at_console,"Fail\n");
					ALERT(at_aiconsole, "I'm not following you, you evil person!\n");
				}
				else
				{
					//ALERT(at_console,"Start\n");
					StartFollowing(pCaller);
					SetBits(m_bitsSaid, bit_saidHelloPlayer);	// Don't say hi after you've started following
				}
			}
		}
		else
		{
			//ALERT(at_console,"Stop\n");
			StopFollowing(TRUE);
		}
	}
}

void CRCAllyMonster :: GruntHealerCall( CBaseEntity *pGrunt )
{
	// Don't allow use during a scripted_sentence
	if ( m_useTime > UTIL_GlobalTimeBase() )
		return;

	ALERT ( at_console, "Call for the medic!\n" );

	SetConditions( bits_COND_MEDIC_HEAL );// Set this condition for the medic to recognise.
	m_hHealTarget = pGrunt;// The grunt we need to heal.
	m_hTargetEnt = m_hHealTarget;
	m_hTalkTarget = m_hTargetEnt;
}

void CRCAllyMonster::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "UseSentence"))
	{
		m_iszUse = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "UnUseSentence"))
	{
		m_iszUnUse = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "RefusalSentence")) //LRC
	{
		m_iszDecline = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "SpeakAs")) //LRC
	{
		m_iszSpeakAs = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue(pkvd);
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CRCAllyMonster::Precache( void ) {
	if (m_iszUse)
		m_szGrp[TLK_USE] = STRING(m_iszUse);

	if (m_iszUnUse)
		m_szGrp[TLK_UNUSE] = STRING(m_iszUnUse);

	if (m_iszDecline) //LRC
		m_szGrp[TLK_DECLINE] = STRING(m_iszDecline);

	PRECACHE_SOUND_ARRAY(pAttackSounds9MM);
	PRECACHE_SOUND_ARRAY(pAttackSoundsSAW);
	PRECACHE_SOUND_ARRAY(pPainSounds);
	PRECACHE_SOUND_ARRAY(pDeathSounds);

	PRECACHE_SOUND("fgrunt/medic.wav");
	PRECACHE_SOUND("hgrunt/gr_reload1.wav");
	PRECACHE_SOUND("weapons/sbarrel1.wav");
	PRECACHE_SOUND("weapons/dbarrel1.wav");
	PRECACHE_SOUND("zombie/claw_miss2.wav");
	PRECACHE_SOUND("weapons/dryfire1.wav"); //LRC
	PRECACHE_SOUND("weapons/desert_eagle_fire.wav");
	PRECACHE_SOUND("weapons/pl_gun3.wav");

	m_iBrassShell   = PRECACHE_MODEL("models/shell.mdl");// brass shell
	m_iShotgunShell = PRECACHE_MODEL("models/shotgunshell.mdl");// shotgun shell
	m_iM249Shell    = PRECACHE_MODEL("models/saw_shell.mdl");// saw shell
	m_iM249Link     = PRECACHE_MODEL("models/saw_link.mdl");// saw link

	TalkInit();
}

//=========================================================
// OccupySlot - if any slots of the passed slots are 
// available, the monster will be assigned to one.
//=========================================================
BOOL CRCAllyMonster :: OccupySlot( int iDesiredSlots )
{
	int i;
	int iMask;
	int iSquadSlots;

	if ( !InSquad() ) {
		return TRUE;
	}

	if ( SquadEnemySplit() )
	{
		// if the squad members aren't all fighting the same enemy, slots are disabled
		// so that a squad member doesn't get stranded unable to engage his enemy because
		// all of the attack slots are taken by squad members fighting other enemies.
		m_iMySlot = bits_SLOT_SQUAD_SPLIT;
		return TRUE;
	}

	CRCAllyMonster *pSquadLeader = MySquadLeader();

	if ( !( iDesiredSlots ^ pSquadLeader->m_afSquadSlots ) )
	{
		// none of the desired slots are available. 
		return FALSE;
	}

	iSquadSlots = pSquadLeader->m_afSquadSlots;

	for ( i = 0; i < NUM_SLOTS; i++ )
	{
		iMask = 1<<i;
		if ( iDesiredSlots & iMask ) // am I looking for this bit?
		{
			if ( !(iSquadSlots & iMask) )	// Is it already taken?
			{
				// No, use this bit
				pSquadLeader->m_afSquadSlots |= iMask;
				m_iMySlot = iMask;
//				ALERT ( at_aiconsole, "Took slot %d - %d\n", i, m_hSquadLeader->m_afSquadSlots );
				return TRUE;
			}
		}
	}

	return FALSE;
}

//=========================================================
// VacateSlot 
//=========================================================
void CRCAllyMonster :: VacateSlot()
{
	if ( m_iMySlot != bits_NO_SLOT && InSquad() )
	{
//		ALERT ( at_aiconsole, "Vacated Slot %d - %d\n", m_iMySlot, m_hSquadLeader->m_afSquadSlots );
		MySquadLeader()->m_afSquadSlots &= ~m_iMySlot;
		m_iMySlot = bits_NO_SLOT;
	}
}

//=========================================================
// ScheduleChange
//=========================================================
void CRCAllyMonster :: ScheduleChange ( void )
{
	VacateSlot();
}

// These functions are still awaiting conversion to CRCAllyMonster 


//=========================================================
//
// SquadRemove(), remove pRemove from my squad.
// If I am pRemove, promote m_pSquadNext to leader
//
//=========================================================
void CRCAllyMonster :: SquadRemove( CRCAllyMonster *pRemove )
{
	ASSERT( pRemove!=NULL );
	ASSERT( this->IsLeader() );
	ASSERT( pRemove->m_hSquadLeader == this );

	// If I'm the leader, get rid of my squad
	if (pRemove == MySquadLeader())
	{
		for (int i = 0; i < MAXRC_SQUAD_MEMBERS-1;i++)
		{
			CRCAllyMonster *pMember = MySquadMember(i);
			if (pMember)
			{
				pMember->m_hSquadLeader = NULL;
				m_hSquadMember[i] = NULL;
			}
		}
	}
	else
	{
		CRCAllyMonster *pSquadLeader = MySquadLeader();
		if (pSquadLeader)
		{
			for (int i = 0; i < MAXRC_SQUAD_MEMBERS-1;i++)
			{
				if (pSquadLeader->m_hSquadMember[i] == this)
				{
					pSquadLeader->m_hSquadMember[i] = NULL;
					break;
				}
			}
		}
	}

	pRemove->m_hSquadLeader = NULL;
}

//=========================================================
//
// SquadAdd(), add pAdd to my squad
//
//=========================================================
BOOL CRCAllyMonster :: SquadAdd( CRCAllyMonster *pAdd )
{
	ASSERT( pAdd!=NULL );
	ASSERT( !pAdd->InSquad() );
	ASSERT( this->IsLeader() );

	for (int i = 0; i < MAXRC_SQUAD_MEMBERS-1; i++)
	{
		if (m_hSquadMember[i] == NULL)
		{
			m_hSquadMember[i] = pAdd;
			pAdd->m_hSquadLeader = this;
			return TRUE;
		}
	}
	return FALSE;
	// should complain here
}


//=========================================================
// 
// SquadPasteEnemyInfo - called by squad members that have
// current info on the enemy so that it can be stored for 
// members who don't have current info.
//
//=========================================================
void CRCAllyMonster :: SquadPasteEnemyInfo ( void )
{
	CRCAllyMonster *pSquadLeader = MySquadLeader( );
	if (pSquadLeader)
		pSquadLeader->m_vecEnemyLKP = m_vecEnemyLKP;
}

//=========================================================
//
// SquadCopyEnemyInfo - called by squad members who don't
// have current info on the enemy. Reads from the same fields
// in the leader's data that other squad members write to,
// so the most recent data is always available here.
//
//=========================================================
void CRCAllyMonster :: SquadCopyEnemyInfo ( void )
{
	CRCAllyMonster *pSquadLeader = MySquadLeader( );
	if (pSquadLeader)
		m_vecEnemyLKP = pSquadLeader->m_vecEnemyLKP;
}

//=========================================================
// 
// SquadMakeEnemy - makes everyone in the squad angry at
// the same entity.
//
//=========================================================
void CRCAllyMonster :: SquadMakeEnemy ( CBaseEntity *pEnemy )
{
	if (!InSquad())
		return;

	if ( !pEnemy )
	{
		ALERT ( at_console, "ERROR: SquadMakeEnemy() - pEnemy is NULL!\n" );
		return;
	}

	CRCAllyMonster *pSquadLeader = MySquadLeader( );
	for (int i = 0; i < MAXRC_SQUAD_MEMBERS; i++)
	{
		CRCAllyMonster *pMember = pSquadLeader->MySquadMember(i);
		if (pMember)
		{
			// reset members who aren't activly engaged in fighting
			if (pMember->m_hEnemy != pEnemy && !pMember->HasConditions( bits_COND_SEE_ENEMY))
			{
				if ( pMember->m_hEnemy != NULL) 
				{
					// remember their current enemy
					pMember->PushEnemy( pMember->m_hEnemy, pMember->m_vecEnemyLKP );
				}
				// give them a new enemy
				pMember->m_hEnemy = pEnemy;
				pMember->m_vecEnemyLKP = pEnemy->pev->origin;
				pMember->SetConditions ( bits_COND_NEW_ENEMY );
			}
		}
	}
}


//=========================================================
//
// SquadCount(), return the number of members of this squad
// callable from leaders & followers
//
//=========================================================
int CRCAllyMonster :: SquadCount( void )
{
	if (!InSquad())
		return 0;

	CRCAllyMonster *pSquadLeader = MySquadLeader();
	int squadCount = 0;
	for (int i = 0; i < MAXRC_SQUAD_MEMBERS; i++)
	{
		if (pSquadLeader->MySquadMember(i) != NULL)
			squadCount++;
	}

	return squadCount;
}


//=========================================================
//
// SquadRecruit(), get some monsters of my classification and
// link them as a group.  returns the group size
//
//=========================================================
int CRCAllyMonster :: SquadRecruit( int searchRadius, int maxMembers )
{
	int squadCount;
	int iMyClass = Classify();// cache this monster's class


	// Don't recruit if I'm already in a group
	if ( InSquad() )
		return 0;

	if ( maxMembers < 2 )
		return 0;

	// I am my own leader
	m_hSquadLeader = this;
	squadCount = 1;

	CBaseEntity *pEntity = NULL;

	if ( !FStringNull( pev->netname ) )
	{
		// I have a netname, so unconditionally recruit everyone else with that name.
		pEntity = UTIL_FindEntityByString( pEntity, "netname", STRING( pev->netname ) );
		while ( pEntity )
		{
			CRCAllyMonster *pRecruit = pEntity->MyTalkSquadMonsterPointer();

			if ( pRecruit )
			{
				if ( !pRecruit->InSquad() && pRecruit->Classify() == iMyClass && pRecruit != this )
				{
					// minimum protection here against user error.in worldcraft. 
					if (!SquadAdd( pRecruit ))
						break;
					squadCount++;
				}
			}
	
			pEntity = UTIL_FindEntityByString( pEntity, "netname", STRING( pev->netname ) );
		}
	}
	else 
	{
		while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, searchRadius )) != NULL)
		{
			CRCAllyMonster *pRecruit = pEntity->MyTalkSquadMonsterPointer( );

			if ( pRecruit && pRecruit != this && pRecruit->IsAlive() && !pRecruit->m_pCine )
			{
				// Can we recruit this guy?
				if ( !pRecruit->InSquad() && pRecruit->Classify() == iMyClass &&
				   ( (iMyClass != CLASS_ALIEN_MONSTER) || FStrEq(STRING(pev->classname), STRING(pRecruit->pev->classname))) &&
				    FStringNull( pRecruit->pev->netname ) )
				{
					TraceResult tr;
					UTIL_TraceLine( pev->origin + pev->view_ofs, pRecruit->pev->origin + pev->view_ofs, ignore_monsters, pRecruit->edict(), &tr );// try to hit recruit with a traceline.
					if ( tr.flFraction == 1.0 )
					{
						if (!SquadAdd( pRecruit ))
							break;

						squadCount++;
					}
				}
			}
		}
	}

	// no single member squads
	if (squadCount == 1)
	{
		m_hSquadLeader = NULL;
	}

	return squadCount;
}

//=========================================================
// CheckEnemy
//=========================================================
int CRCAllyMonster :: CheckEnemy ( CBaseEntity *pEnemy )
{
	int iUpdatedLKP;

	iUpdatedLKP = CBaseMonster :: CheckEnemy ( m_hEnemy );
	
	// communicate with squad members about the enemy IF this individual has the same enemy as the squad leader.
	if ( InSquad() && (CBaseEntity *)m_hEnemy == MySquadLeader()->m_hEnemy )
	{
		if ( iUpdatedLKP )
		{
			// have new enemy information, so paste to the squad.
			SquadPasteEnemyInfo();
		}
		else
		{
			// enemy unseen, copy from the squad knowledge.
			SquadCopyEnemyInfo();
		}
	}

	return iUpdatedLKP;
}

//=========================================================
// StartMonster
//=========================================================
void CRCAllyMonster :: StartMonster( void )
{
	CBaseMonster :: StartMonster();

	if ( ( m_afCapability & bits_CAP_SQUAD ) && !InSquad() )
	{
		if ( !FStringNull( pev->netname ) )
		{
			// if I have a groupname, I can only recruit if I'm flagged as leader
			if ( !( pev->spawnflags & SF_MONSTER_SQUADLEADER) )
			{
				return;
			}
		}

		// try to form squads now.
		int iSquadSize = SquadRecruit( 1024, 5 );

		if ( iSquadSize )
		{
		  ALERT ( at_aiconsole, "Squad of %d %s formed\n", iSquadSize, STRING( pev->classname ) );
		}
	}
}

//=========================================================
// NoFriendlyFire - checks for possibility of friendly fire
//
// Builds a large box in front of the grunt and checks to see 
// if any squad members are in that box. 
//=========================================================
BOOL CRCAllyMonster::NoFriendlyFire(void) {
	return NoFriendlyFire(FALSE); //default: don't like the player
}

BOOL CRCAllyMonster::NoFriendlyFire(BOOL playerAlly) {
	if (!playerAlly && !InSquad()) {
		return TRUE;
	}

	CPlane	backPlane;
	CPlane  leftPlane;
	CPlane	rightPlane;

	Vector	vecLeftSide;
	Vector	vecRightSide;
	Vector	v_left;

	if ( m_hEnemy != NULL ) {
		UTIL_MakeVectors ( UTIL_VecToAngles( m_hEnemy->Center() - pev->origin ) );
	} else {
		// if there's no enemy, pretend there's a friendly in the way, so the grunt won't shoot.
		return FALSE;
	}
	
	vecLeftSide = pev->origin - ( gpGlobals->v_right * ( pev->size.x * 1.5 ) );
	vecRightSide = pev->origin + ( gpGlobals->v_right * ( pev->size.x * 1.5 ) );
	v_left = gpGlobals->v_right * -1;

	leftPlane.InitializePlane ( gpGlobals->v_right, vecLeftSide );
	rightPlane.InitializePlane ( v_left, vecRightSide );
	backPlane.InitializePlane ( gpGlobals->v_forward, pev->origin );

/*
	ALERT ( at_console, "LeftPlane: %f %f %f : %f\n", leftPlane.m_vecNormal.x, leftPlane.m_vecNormal.y, leftPlane.m_vecNormal.z, leftPlane.m_flDist );
	ALERT ( at_console, "RightPlane: %f %f %f : %f\n", rightPlane.m_vecNormal.x, rightPlane.m_vecNormal.y, rightPlane.m_vecNormal.z, rightPlane.m_flDist );
	ALERT ( at_console, "BackPlane: %f %f %f : %f\n", backPlane.m_vecNormal.x, backPlane.m_vecNormal.y, backPlane.m_vecNormal.z, backPlane.m_flDist );
*/

	if ( !m_afMemory & bits_MEMORY_PROVOKED ) {
		edict_t		*pentPlayer = FIND_CLIENT_IN_PVS( edict() );
		if (!FNullEnt(pentPlayer) &&
			backPlane.PointInFront  ( pentPlayer->v.origin ) &&
			leftPlane.PointInFront  ( pentPlayer->v.origin ) && 
			rightPlane.PointInFront ( pentPlayer->v.origin ) ) {
			// the player is in the check volume! Don't shoot!
			return FALSE;
		}
	}

	CRCAllyMonster *pSquadLeader = MySquadLeader();
	for (int i = 0; i < MAXRC_SQUAD_MEMBERS; i++) {
		CRCAllyMonster *pMember = pSquadLeader->MySquadMember(i);
		if (pMember && pMember != this) {
			if ( backPlane.PointInFront  ( pMember->pev->origin ) &&
				 leftPlane.PointInFront  ( pMember->pev->origin ) && 
				 rightPlane.PointInFront ( pMember->pev->origin) ) {
				// this guy is in the check volume! Don't shoot!
				return FALSE;
			}
		}
	}

	if (playerAlly) {
		edict_t	*pentPlayer = FIND_CLIENT_IN_PVS(edict());
		if (!FNullEnt(pentPlayer) &&
			backPlane.PointInFront(pentPlayer->v.origin) &&
			leftPlane.PointInFront(pentPlayer->v.origin) &&
			rightPlane.PointInFront(pentPlayer->v.origin)) {
			// the player is in the check volume! Don't shoot!
			return FALSE;
		}
	}

	return TRUE;
}

//=========================================================
// GetIdealState - surveys the Conditions information available
// and finds the best new state for a monster.
//=========================================================
MONSTERSTATE CRCAllyMonster::GetIdealState(void) {
	int iConditions = IScheduleFlags();

	// If no schedule conditions, the new ideal state is probably the reason we're in here.
	switch (m_MonsterState) {
	case MONSTERSTATE_IDLE:
	case MONSTERSTATE_ALERT:
		if (HasConditions(bits_COND_NEW_ENEMY) && InSquad()) {
			SquadMakeEnemy(m_hEnemy);
		}
		break;
	}

	return CBaseMonster::GetIdealState();
}

//=========================================================
// FValidateCover - determines whether or not the chosen
// cover location is a good one to move to. (currently based
// on proximity to others in the squad)
//=========================================================
BOOL CRCAllyMonster :: FValidateCover ( const Vector &vecCoverLocation )
{
	if ( !InSquad() )
	{
		return TRUE;
	}

	if (SquadMemberInRange( vecCoverLocation, 128 ))
	{
		// another squad member is too close to this piece of cover.
		return FALSE;
	}

	return TRUE;
}

//=========================================================
// SquadEnemySplit- returns TRUE if not all squad members
// are fighting the same enemy. 
//=========================================================
BOOL CRCAllyMonster :: SquadEnemySplit ( void )
{
	if (!InSquad())
		return FALSE;

	CRCAllyMonster	*pSquadLeader = MySquadLeader();
	CBaseEntity		*pEnemy	= pSquadLeader->m_hEnemy;

	for (int i = 0; i < MAXRC_SQUAD_MEMBERS; i++)
	{
		CRCAllyMonster *pMember = pSquadLeader->MySquadMember(i);
		if (pMember != NULL && pMember->m_hEnemy != NULL && pMember->m_hEnemy != pEnemy)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//=========================================================
// FValidateCover - determines whether or not the chosen
// cover location is a good one to move to. (currently based
// on proximity to others in the squad)
//=========================================================
BOOL CRCAllyMonster :: SquadMemberInRange ( const Vector &vecLocation, float flDist ) {
	if (!InSquad())
		return FALSE;

	CRCAllyMonster *pSquadLeader = MySquadLeader();

	for (int i = 0; i < MAXRC_SQUAD_MEMBERS; i++) {
		CRCAllyMonster *pSquadMember = pSquadLeader->MySquadMember(i);
		if (pSquadMember && (vecLocation - pSquadMember->pev->origin ).Length2D() <= flDist)
			return TRUE;
	}

	return FALSE;
}

//=========================================================
// Kick
//=========================================================
CBaseEntity *CRCAllyMonster::Kick(void) {
	TraceResult tr;

	UTIL_MakeVectors(pev->angles);
	Vector vecStart = pev->origin;
	vecStart.z += pev->size.z * 0.5;
	Vector vecEnd = vecStart + (gpGlobals->v_forward * 70);

	UTIL_TraceHull(vecStart, vecEnd, dont_ignore_monsters, head_hull, ENT(pev), &tr);

	if (tr.pHit) {
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		return pEntity;
	}

	return NULL;
}

//=========================================================
// PainSound
//=========================================================
void CRCAllyMonster::PainSound(void) {
	if (UTIL_GlobalTimeBase() < m_flNextPainTime)
		return;

	m_flNextPainTime = UTIL_GlobalTimeBase() + RANDOM_FLOAT(0.5, 0.75);
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pPainSounds);
}

//=========================================================
// DeathSound 
//=========================================================
void CRCAllyMonster::DeathSound(void) {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pDeathSounds);
}

//=========================================================
// CheckAmmo - overridden for the grunt because he actually
// uses ammo! (base class doesn't)
//=========================================================
void CRCAllyMonster::CheckAmmo(void) {
	if (m_cAmmoLoaded <= 0) {
		SetConditions(bits_COND_NO_AMMO_LOADED);
	}
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
BOOL CRCAllyMonster::FCanCheckAttacks(void) {
	if (!HasConditions(bits_COND_ENEMY_TOOFAR)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

//=========================================================
// CheckMeleeAttack1
//=========================================================
BOOL CRCAllyMonster::CheckMeleeAttack1(float flDot, float flDist) {
	CBaseMonster *pEnemy;
	if (m_hEnemy != NULL) {
		pEnemy = m_hEnemy->MyMonsterPointer();

		if (!pEnemy) {
			return FALSE;
		}
	}

	if (flDist <= 64 && flDot >= 0.7	&&
		pEnemy->Classify() != CLASS_ALIEN_BIOWEAPON &&
		pEnemy->Classify() != CLASS_PLAYER_BIOWEAPON) {
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
BOOL CRCAllyMonster::CheckRangeAttack1(float flDot, float flDist) {
	if (!HasConditions(bits_COND_ENEMY_OCCLUDED) && flDist <= 2048 
		&& flDot >= 0.5 && NoFriendlyFire() 
		&& m_fLockShootTime < UTIL_GlobalTimeBase()) {
		if (!m_hEnemy->IsPlayer() && flDist <= 64) {
			// kick nonclients who are close enough, but don't shoot at them.
			return FALSE;
		}

		Vector vecSrc = GetGunPosition();
		TraceResult	tr;
		// verify that a bullet fired from the gun will hit the enemy before the world.
		UTIL_TraceLine(vecSrc, m_hEnemy->BodyTarget(vecSrc), ignore_monsters, ignore_glass, ENT(pev), &tr);
		if (tr.flFraction == 1.0) {
			return TRUE;
		}
	}

	return FALSE;
}

//=========================================================
// DeclineFollowing 
//=========================================================
void CRCAllyMonster::DeclineFollowing(void) {
	PlaySentence("FG_STOP", 2, VOL_NORM, ATTN_NORM);
}

//=========================================================
// GetGunPosition	return the end of the barrel
//=========================================================
Vector CRCAllyMonster::GetGunPosition() {
	if (m_fStanding) {
		return pev->origin + Vector(0, 0, 60);
	} else {
		return pev->origin + Vector(0, 0, 48);
	}
}

//=========================================================
// Shoot MP5
//=========================================================
void CRCAllyMonster::ShootMP5(void) {
	if ((m_hEnemy == NULL && m_pCine == NULL) || !NoFriendlyFire()) {
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

	if (m_cAmmoLoaded > 0) {
		UTIL_MakeVectors(pev->angles);

		Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40, 90) + gpGlobals->v_up * RANDOM_FLOAT(75, 200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
		EjectBrass(vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL);
		FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_4DEGREES, 2048, BULLET_MONSTER_MP5); // shoot +-5 degrees
		EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackSounds9MM);
		WeaponFlash(vecShootOrigin);

		pev->effects |= EF_MUZZLEFLASH;

		m_cAmmoLoaded--;// take away a bullet!
	} else {
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/dryfire1.wav", 1, ATTN_NORM);
	}

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);
	Vector angDir = UTIL_VecToAngles(vecShootDir);
	SetBlending(0, angDir.x);
}

//=========================================================
// Shoot Shotgun
//=========================================================
void CRCAllyMonster::ShootShotgun(void) {
	if ((m_hEnemy == NULL && m_pCine == NULL) || !NoFriendlyFire()) {
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

	if (m_cAmmoLoaded > 0) {
		UTIL_MakeVectors(pev->angles);

		Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40, 90) + gpGlobals->v_up * RANDOM_FLOAT(75, 200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
		EjectBrass(vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iShotgunShell, TE_BOUNCE_SHOTSHELL);
		FireBullets(gSkillData.hgruntShotgunPellets, vecShootOrigin, vecShootDir, VECTOR_CONE_15DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0); // shoot +-7.5 degrees
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/sbarrel1.wav", 1, ATTN_NORM);
		WeaponFlash(vecShootOrigin);

		pev->effects |= EF_MUZZLEFLASH;
		m_cAmmoLoaded--;
	} else {
		 EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/dryfire1.wav", 1, ATTN_NORM);
	}

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);
	Vector angDir = UTIL_VecToAngles(vecShootDir);
	SetBlending(0, angDir.x);
}

void CRCAllyMonster::ShootShotgunDouble(void) {
	if ((m_hEnemy == NULL && m_pCine == NULL) || !NoFriendlyFire()) {
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

	if (m_cAmmoLoaded >= 2) {
		UTIL_MakeVectors(pev->angles);

		Vector vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40, 90) + gpGlobals->v_up * RANDOM_FLOAT(75, 200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
		EjectBrass(vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iShotgunShell, TE_BOUNCE_SHOTSHELL);
		FireBullets((gSkillData.hgruntShotgunPellets * 2), vecShootOrigin, vecShootDir, VECTOR_CONE_9DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0); // shoot +-7.5 degrees
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/dbarrel1.wav", 1, ATTN_NORM);
		WeaponFlash(vecShootOrigin);

		pev->effects |= EF_MUZZLEFLASH;
		m_cAmmoLoaded = (m_cAmmoLoaded - 2);
	} else {
		 EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/dryfire1.wav", 1, ATTN_NORM);
	}

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);
	Vector angDir = UTIL_VecToAngles(vecShootDir);
	SetBlending(0, angDir.x);
}

//=========================================================
// Shoot M249
//=========================================================
void CRCAllyMonster::ShootM249(void) {
	if ((m_hEnemy == NULL && m_pCine == NULL) || !NoFriendlyFire()) {
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

	if (m_cAmmoLoaded > 0) {
		UTIL_MakeVectors(pev->angles);

		Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40, 90) + gpGlobals->v_up * RANDOM_FLOAT(75, 200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);

		if (m_flLinkToggle >= 2) {
			EjectBrass(vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iM249Link, TE_BOUNCE_SHELL);
			m_flLinkToggle = 0;
		} else {
			EjectBrass(vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iM249Shell, TE_BOUNCE_SHELL);
			m_flLinkToggle++;
		}

		FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_3DEGREES, 2048, BULLET_MONSTER_556); // shoot +-5 degrees
		WeaponFlash(vecShootOrigin);
		EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackSoundsSAW);

		pev->effects |= EF_MUZZLEFLASH;
		m_cAmmoLoaded--;
	} else {
		 EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/dryfire1.wav", 1, ATTN_NORM);
	}

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);
	Vector angDir = UTIL_VecToAngles(vecShootDir);
	SetBlending(0, angDir.x);
}

//=========================================================
// Shoot Desert Eagle
//=========================================================
void CRCAllyMonster::ShootDesertEagle(void) {
	if ((m_hEnemy == NULL && m_pCine == NULL) || !NoFriendlyFire()) {
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

	if (m_cAmmoLoaded > 0) {
		UTIL_MakeVectors(pev->angles);

		int pitchShift = RANDOM_LONG(0, 20);
		if (pitchShift > 10)
			pitchShift = 0;
		else
			pitchShift -= 5;

		Vector vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40, 90) + gpGlobals->v_up * RANDOM_FLOAT(75, 200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
		EjectBrass(vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL);
		FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_1DEGREES, 1024, BULLET_MONSTER_357 ); // shoot +-5 degrees
		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/desert_eagle_fire.wav", 1, ATTN_NORM, 0, 100 + pitchShift);

		pev->effects |= EF_MUZZLEFLASH;

		WeaponFlash(vecShootOrigin);
		m_cAmmoLoaded--;// take away a bullet!
	} else {
		 EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/dryfire1.wav", 1, ATTN_NORM);
	}

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);
	Vector angDir = UTIL_VecToAngles(vecShootDir);
	SetBlending(0, angDir.x);
}
//=========================================================
// Shoot 9mm Glock
//=========================================================
void CRCAllyMonster::ShootGlock(void) {
	if ((m_hEnemy == NULL && m_pCine == NULL) || !NoFriendlyFire()) {
		return;
	}

	Vector vecShootOrigin = GetGunPosition();
	Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

	if (m_cAmmoLoaded > 0) {
		UTIL_MakeVectors(pev->angles);

		Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40, 90) + gpGlobals->v_up * RANDOM_FLOAT(75, 200) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
		EjectBrass(vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL);
		FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_1DEGREES, 1024, BULLET_MONSTER_9MM); // shoot +-5 degrees
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/pl_gun3.wav", 1, ATTN_NORM);

		pev->effects |= EF_MUZZLEFLASH;

		WeaponFlash(vecShootOrigin);

		m_cAmmoLoaded--;// take away a bullet!
	} else {
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/dryfire1.wav", 1, ATTN_NORM);
	}

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);
	Vector angDir = UTIL_VecToAngles(vecShootDir);
	SetBlending(0, angDir.x);
}

//=========================================================
// CheckRangeAttack2 - this checks the Grunt's grenade
// attack. 
//=========================================================
BOOL CRCAllyMonster::CheckRangeAttack2(float flDot, float flDist) {
	// if the grunt isn't moving, it's ok to check.
	if (m_flGroundSpeed != 0) {
		m_fThrowGrenade = FALSE;
		return m_fThrowGrenade;
	}

	// assume things haven't changed too much since last time
	if (UTIL_GlobalTimeBase() < m_flNextGrenadeCheck) {
		return m_fThrowGrenade;
	}

	if (!FBitSet(m_hEnemy->pev->flags, FL_ONGROUND) && m_hEnemy->pev->waterlevel == 0 && m_vecEnemyLKP.z > pev->absmax.z) {
		//!!!BUGBUG - we should make this check movetype and make sure it isn't FLY? Players who jump a lot are unlikely to 
		// be grenaded.
		// don't throw grenades at anything that isn't on the ground!
		m_fThrowGrenade = FALSE;
		return m_fThrowGrenade;
	}

	Vector vecTarget;

	// find feet
	if (RANDOM_LONG(0, 1)) {
		// magically know where they are
		vecTarget = Vector(m_hEnemy->pev->origin.x, m_hEnemy->pev->origin.y, m_hEnemy->pev->absmin.z);
	}
	else {
		// toss it to where you last saw them
		vecTarget = m_vecEnemyLKP;
	}

	// are any of my squad members near the intended grenade impact area?
	// are any of my squad members near the intended grenade impact area?
	if (InSquad()) {
		if (SquadMemberInRange(vecTarget, 256)) {
			// crap, I might blow my own guy up. Don't throw a grenade and don't check again for a while.
			m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 1; // one full second.
			m_fThrowGrenade = FALSE;
			return m_fThrowGrenade;	//AJH need this or it is overridden later.
		}
	}

	if ((vecTarget - pev->origin).Length2D() <= 256) {
		// crap, I don't want to blow myself up
		m_flNextGrenadeCheck = UTIL_GlobalTimeBase() + 1; // one full second.
		m_fThrowGrenade = FALSE;
		return m_fThrowGrenade;
	}


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

	return m_fThrowGrenade;
}

//=========================================================
// TakeDamage - overridden for the grunt because the grunt
// needs to forget that he is in cover if he's hurt. (Obviously
// not in a safe place anymore).
//=========================================================
int CRCAllyMonster::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) {
	if (m_fImmortal)
		flDamage = 0;

	if (pev->spawnflags & SF_MONSTER_INVINCIBLE) {
		if (m_flDebug)
			ALERT(at_console, "%s:TakeDamage:SF_MONSTER_INVINCIBLE\n", STRING(pev->classname));

		CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
		if (pEnt->IsPlayer()) {
			pev->health = pev->max_health / 2;
			if (flDamage > 0) //Override all damage
				SetConditions(bits_COND_LIGHT_DAMAGE);

			if (flDamage >= 20) //Override all damage
				SetConditions(bits_COND_HEAVY_DAMAGE);

			return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
		}

		if (pevAttacker->owner) {
			pEnt = CBaseEntity::Instance(pevAttacker->owner);
			if (pEnt->IsPlayer()) {
				pev->health = pev->max_health / 2;
				if (flDamage > 0) //Override all damage
					SetConditions(bits_COND_LIGHT_DAMAGE);

				if (flDamage >= 20) //Override all damage
					SetConditions(bits_COND_HEAVY_DAMAGE);

				return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
			}
		}
	}

	Forget(bits_MEMORY_INCOVER);
	if (!IsAlive() || pev->deadflag == DEAD_DYING || m_iPlayerReact) {
		return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	}

	if (pevAttacker && m_MonsterState != MONSTERSTATE_PRONE && (pevAttacker->flags & FL_CLIENT)) {
		CBaseEntity *pFriend = FindNearestFriend(FALSE);
		if (pFriend && pFriend->IsAlive()) {
			// only if not dead or dying!
			CRCAllyMonster *pTalkMonster = (CRCAllyMonster *)pFriend;
			pTalkMonster->ChangeSchedule(slIdleStopShootingTS);
		}

		m_flPlayerDamage += flDamage;
		if (m_hEnemy == NULL) {
			if ((m_afMemory & bits_MEMORY_SUSPICIOUS) || UTIL_IsFacing(pevAttacker, pev->origin)) {
				if (m_iszSpeakAs) {
					char szBuf[32];
					strcpy(szBuf, STRING(m_iszSpeakAs));
					strcat(szBuf, "_MAD");
					PlaySentence(szBuf, 4, VOL_NORM, ATTN_NORM);
				} else {
					PlaySentence("FG_MAD", 4, VOL_NORM, ATTN_NORM);
				}

				Remember(bits_MEMORY_PROVOKED);
				StopFollowing(TRUE);
			} else {
				if (m_iszSpeakAs) {
					char szBuf[32];
					strcpy(szBuf, STRING(m_iszSpeakAs));
					strcat(szBuf, "_SHOT");
					PlaySentence(szBuf, 4, VOL_NORM, ATTN_NORM);
				} else {
					PlaySentence("FG_SHOT", 4, VOL_NORM, ATTN_NORM);
				}
				Remember(bits_MEMORY_SUSPICIOUS);
			}
		} else if (!(m_hEnemy->IsPlayer()) && pev->deadflag == DEAD_NO) {
			if (m_iszSpeakAs) {
				char szBuf[32];
				strcpy(szBuf, STRING(m_iszSpeakAs));
				strcat(szBuf, "_SHOT");
				PlaySentence(szBuf, 4, VOL_NORM, ATTN_NORM);
			} else {
				PlaySentence("FG_SHOT", 4, VOL_NORM, ATTN_NORM);
			}
		}
	}

	return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}