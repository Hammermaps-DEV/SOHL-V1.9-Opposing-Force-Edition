/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
// voltigore
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"nodes.h"
#include	"effects.h"
#include	"decals.h"
#include	"soundent.h"
#include	"game.h"
#include    "monster_alien_babyvoltigore.h"

//=========================================================
// CBabyAlienVoltigore
//=========================================================

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		BABY_VOLTIGORE_AE_RUN			( 14 )
#define		VOLTIGORE_AE_PUNCH_BOTH		( 12 )
#define		VOLTIGORE_AE_PUNCH_SINGLE	( 13 )

LINK_ENTITY_TO_CLASS(monster_alien_babyvoltigore, CBabyVoltigore);

//=========================================================
// Spawn
//=========================================================
void CBabyVoltigore::Spawn()
{
	Precache();

	SET_MODEL(ENT(pev), "models/baby_voltigore.mdl");
	UTIL_SetSize(pev, Vector(-40, -40, 0), Vector(40, 40, 64));

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_GREEN;
	pev->effects = 0;
	pev->health = 20 /*gSkillData.babyVoltigoreHealth*/;
	m_flFieldOfView = 0.2;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState = MONSTERSTATE_NONE;

	m_flNextSpitTime = gpGlobals->time;

	pev->view_ofs = Vector(0, 0, 64);

	MonsterInit();
}

//=========================================================
//=========================================================
void CBabyVoltigore::Precache(void)
{
	pev->model = ALLOC_STRING("models/baby_voltigore.mdl");
	CVoltigore::Precache();
}

void CBabyVoltigore::HandleAnimEvent(MonsterEvent_t* pEvent)
{
	switch (pEvent->event)
	{
	case BABY_VOLTIGORE_AE_RUN:
		//EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pFootstepSounds), RANDOM_FLOAT(0.5, 0.6), ATTN_NORM, 0, RANDOM_LONG(85, 120));
		break;

	case VOLTIGORE_AE_PUNCH_SINGLE:
	{
		CBaseEntity *pHurt = CheckTraceHullAttack(70, 1/*gSkillData.babyVoltigoreDmgPunch*/, DMG_CLUB | DMG_ALWAYSGIB);
		if (pHurt)
		{
			pHurt->pev->punchangle.z = -10;
			pHurt->pev->punchangle.x = 10;
			pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * -100;
			pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 50;

			//EMIT_SOUND(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pMeleeHitSounds), RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);

			PrintBloodDecal(pHurt, pHurt->pev->origin, pHurt->pev->velocity, RANDOM_FLOAT(40, 45));
		}
		else
		{
			//EMIT_SOUND(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pMeleeMissSounds), RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);
		}
	}
	break;

	case VOLTIGORE_AE_PUNCH_BOTH:
	{
		CBaseEntity *pHurt = CheckTraceHullAttack(70, 1 /*gSkillData.babyVoltigoreDmgPunch*/, DMG_CLUB | DMG_ALWAYSGIB);
		if (pHurt)
		{
			pHurt->pev->punchangle.x = 20;
			pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * 100;
			pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 50;

			//EMIT_SOUND(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pMeleeHitSounds), RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);

			PrintBloodDecal(pHurt, pHurt->pev->origin, Vector(0, 0, -1), 50);
		}
		else
		{
			//EMIT_SOUND(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pMeleeMissSounds), RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);
		}
	}
	break;
	default:
		CVoltigore::HandleAnimEvent(pEvent);
		break;
	}
}

//=========================================================
// Start task - selects the correct activity and performs
// any necessary calculations to start the next task on the
// schedule.  OVERRIDDEN for voltigore because it needs to
// know explicitly when the last attempt to chase the enemy
// failed, since that impacts its attack choices.
//=========================================================
void CBabyVoltigore::StartTask(Task_t *pTask)
{
	m_iTaskStatus = TASKSTATUS_RUNNING;

	switch (pTask->iTask)
	{
	case TASK_MELEE_ATTACK1:
	case TASK_MELEE_ATTACK2:
	{
		//EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pAttackMeleeSounds), RANDOM_FLOAT(0.5, 0.6), ATTN_NONE, 0, RANDOM_LONG(100, 105));
		CBaseMonster::StartTask(pTask);
	}
	break;

	default:
		CBaseMonster::StartTask(pTask);
		break;
	}
}
