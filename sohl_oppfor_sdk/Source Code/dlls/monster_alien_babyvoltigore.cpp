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
// voltigore
//=========================================================
#include	"extdll.h"
#include	"plane.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"nodes.h"
#include	"schedule.h"
#include	"animation.h"
#include	"squadmonster.h"
#include	"weapons.h"
#include	"talkmonster.h"
#include	"soundent.h"
#include	"effects.h"
#include	"customentity.h"
#include	"scripted.h" //LRC
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

	m_flNextSpitTime = UTIL_GlobalTimeBase();

	pev->view_ofs = Vector(0, 0, 64);

	MonsterInit();
}

//=========================================================
//=========================================================
void CBabyVoltigore::Precache()
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
