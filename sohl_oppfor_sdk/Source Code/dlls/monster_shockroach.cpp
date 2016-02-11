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
// NPC: Shock-Roach * http://half-life.wikia.com/wiki/Shock_Roach
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00004 / Date: 17.10.2015
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"game.h"
#include	"player.h"
#include	"weapons.h"
#include	"monster_shockroach.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	SR_AE_JUMPATTACK	0x02

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS(monster_shockroach, CShockRoach);

TYPEDESCRIPTION	CShockRoach::m_SaveData[] = {
	DEFINE_FIELD(CShockRoach, m_flDie, FIELD_TIME),
};

IMPLEMENT_SAVERESTORE(CShockRoach, CHeadCrab);

//=========================================================
// Monster Sounds
//=========================================================
const char *CShockRoach::pIdleSounds[] = {
	"shockroach/shock_idle1.wav",
	"shockroach/shock_idle2.wav",
	"shockroach/shock_idle3.wav",
};

const char *CShockRoach::pAttackSounds[] = {
	"shockroach/shock_jump1.wav",
	"shockroach/shock_jump2.wav",
};

//=========================================================
// Spawn Shock-Roach
//=========================================================
void CShockRoach::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/w_shock_rifle.mdl");

	UTIL_SetSize(pev, Vector(-12, -12, 0), Vector(12, 12, 24));

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_GREEN;
	pev->effects = 0;

	if (pev->health == 0)
		pev->health = gSkillData.sroachHealth;

	pev->view_ofs = Vector(0, 0, 20);// position of the eyes relative to monster's origin.
	pev->yaw_speed = 5;// should we put this in the monster's changeanim function since turn rates may vary with state/anim?
	m_flFieldOfView = 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState = MONSTERSTATE_NONE;
	m_flDie = UTIL_GlobalTimeBase() + gSkillData.sroachLifespan;
	m_flPitch = 100; //basic voice pitch for this monster

	MonsterInit();
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CShockRoach::Classify(void) {
	return m_iClass ? m_iClass : CLASS_ALIEN_BIOWEAPON;
}

//=========================================================
// Read options from bsp * put in .fgd file as option *
// [dies] = 1/0 *can die*
// [weapon] = 1/0 *can equip as weapon* 
//=========================================================
void CShockRoach::KeyValue(KeyValueData *pkvd) {
	if (FStrEq(pkvd->szKeyName, "dies")) {
		m_iDies = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	} else if (FStrEq(pkvd->szKeyName, "weapon")) {
		m_iWeapon = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	} else {
		CBaseMonster::KeyValue(pkvd);
	}
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CShockRoach::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/w_shock_rifle.mdl");

	PRECACHE_SOUND_ARRAY(pIdleSounds);
	PRECACHE_SOUND_ARRAY(pAttackSounds);

	PRECACHE_SOUND("shockroach/shock_angry.wav");
	PRECACHE_SOUND("shockroach/shock_flinch.wav");
	PRECACHE_SOUND("shockroach/shock_die.wav");
	PRECACHE_SOUND("shockroach/shock_bite.wav");
	PRECACHE_SOUND("shockroach/shock_walk.wav");
}

//=========================================================
// LeapTouch - this is the headcrab's touch function when it
// is in the air
//=========================================================
void CShockRoach::LeapTouch(CBaseEntity *pOther) {
	if (!pOther->pev->takedamage) {
		return;
	}

	if (pOther->Classify() == Classify()) {
		return;
	}

	// Don't hit if back on ground
	if (!FBitSet(pev->flags, FL_ONGROUND)) {
		EMIT_SOUND_DYN(edict(), CHAN_WEAPON, "shockroach/shock_bite.wav", GetSoundVolue(), ATTN_IDLE, 0, GetVoicePitch(RANDOM_LONG(-5, 5)));

		/* //Uncomment this when "WEAPON_SHOCKRIFLE" is interpreted!!!
		// Give the shockrifle weapon to the player, if not already in possession.
		CBasePlayer* pPlayer = dynamic_cast<CBasePlayer*>(pOther);
		if (m_iWeapon && pPlayer && !(pPlayer->pev->weapons & (1 << WEAPON_SHOCKRIFLE)))
		{
			pPlayer->GiveNamedItem("weapon_shockrifle");
			pPlayer->pev->weapons |= (1 << WEAPON_SHOCKRIFLE);
			UTIL_Remove(this);
			return;
		}
		*/
		pOther->TakeDamage(pev, pev, gSkillData.sroachDmgBite, DMG_SLASH);
	}

	SetTouch(NULL);
}
//=========================================================
// PrescheduleThink
//=========================================================
void CShockRoach::PrescheduleThink(void) {
	// Kill Shock-Roach when in out a specific period
	if (m_iDies && UTIL_GlobalTimeBase() >= m_flDie) {
		pev->health = -1;
		Killed(pev, 0);
		return;
	}

	CHeadCrab::PrescheduleThink();
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CShockRoach::HandleAnimEvent(MonsterEvent_t *pEvent) {
	switch (pEvent->event) {
		case SR_AE_JUMPATTACK: {
			ClearBits(pev->flags, FL_ONGROUND);
			UTIL_SetOrigin(this, pev->origin + Vector(0, 0, 1));// take him off ground so engine doesn't instantly reset onground 
			UTIL_MakeVectors(pev->angles);

			Vector vecJumpDir;
			if (m_hEnemy != NULL) {
				float gravity = g_psv_gravity->value;
				if (gravity <= 1)
					gravity = 1;

				// How fast does the headcrab need to travel to reach that height given gravity?
				float height = (m_hEnemy->pev->origin.z + m_hEnemy->pev->view_ofs.z - pev->origin.z);
				if (height < 16) {
					height = 16;
				}

				float speed = sqrt(2 * gravity * height);
				float time = speed / gravity;

				// Scale the sideways velocity to get there at the right time
				vecJumpDir = (m_hEnemy->pev->origin + m_hEnemy->pev->view_ofs - pev->origin);
				vecJumpDir = vecJumpDir * (1.0 / time);

				// Speed to offset gravity at the desired height
				vecJumpDir.z = speed;

				// Don't jump too far/fast
				float distance = vecJumpDir.Length();
				if (distance > 650) {
					vecJumpDir = vecJumpDir * (650.0 / distance);
				}
			}
			else {
				// jump hop, don't care where
				vecJumpDir = Vector(gpGlobals->v_forward.x, gpGlobals->v_forward.y, gpGlobals->v_up.z) * 350;
			}

			pev->velocity = vecJumpDir;
			m_flNextAttack = UTIL_GlobalTimeBase() + 2;
		}
		break;
		default: {
			CBaseMonster::HandleAnimEvent(pEvent);
		}
		break;
	}
}

//=========================================================
// IdleSound
//=========================================================
void CShockRoach::IdleSound(void) {
	EMIT_SOUND_DYN(edict(), CHAN_VOICE, RANDOM_SOUND_ARRAY(pIdleSounds), GetSoundVolue(), ATTN_IDLE, 0, GetVoicePitch(RANDOM_LONG(-5, 5)));
}

//=========================================================
// AlertSound 
//=========================================================
void CShockRoach::AlertSound(void) {
	EMIT_SOUND_DYN(edict(), CHAN_VOICE, "shockroach/shock_angry.wav", GetSoundVolue(), ATTN_IDLE, 0, GetVoicePitch(RANDOM_LONG(-5, 14)));
}

//=========================================================
// PainSound 
//=========================================================
void CShockRoach::PainSound(void) {
	EMIT_SOUND_DYN(edict(), CHAN_VOICE, "shockroach/shock_flinch.wav", GetSoundVolue(), ATTN_IDLE, 0, GetVoicePitch(RANDOM_LONG(-5, 14)));
}

//=========================================================
// DeathSound 
//=========================================================
void CShockRoach::DeathSound(void) {
	EMIT_SOUND_DYN(edict(), CHAN_VOICE, "shockroach/shock_die.wav", GetSoundVolue(), ATTN_IDLE, 0, GetVoicePitch(1));
}

//=========================================================
// AttackSound 
//=========================================================
void CShockRoach::AttackSound(void) {
	EMIT_SOUND_DYN(edict(), CHAN_VOICE, RANDOM_SOUND_ARRAY(pAttackSounds), GetSoundVolue(), ATTN_IDLE, 0, GetVoicePitch(RANDOM_LONG(-5, 5)));
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
void CShockRoach::StartTask(Task_t *pTask) {
	m_iTaskStatus = TASKSTATUS_RUNNING;
	switch (pTask->iTask) {
		case TASK_RANGE_ATTACK1: {
			m_IdealActivity = ACT_RANGE_ATTACK1;
			SetTouch(&CShockRoach::LeapTouch);
		}
		break;
		default: {
			CHeadCrab::StartTask(pTask);
		}
		break;
	}
}