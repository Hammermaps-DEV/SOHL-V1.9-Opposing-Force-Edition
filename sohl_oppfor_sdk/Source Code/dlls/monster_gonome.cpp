/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

//=========================================================
// Gonome.cpp
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"player.h"
#include	"soundent.h"
#include	"proj_gonomespit.h"
#include	"monster_gonome.h"

#define		GONOME_SPRINT_DIST	256 // how close the squid has to get before starting to sprint and refusing to swerve

#define		GONOME_TOLERANCE_MELEE1_RANGE	95
#define		GONOME_TOLERANCE_MELEE2_RANGE	85

#define		GONOME_TOLERANCE_MELEE1_DOT		0.7
#define		GONOME_TOLERANCE_MELEE2_DOT		0.7

#define		GONOME_MELEE_ATTACK_RADIUS		70
#define     GONOME_FLINCH_DELAY			    2

//=========================================================
// Monster's Anim Events Go Here
//=========================================================

#define GONOME_AE_SLASH_RIGHT	( 1 )
#define GONOME_AE_SLASH_LEFT	( 2 )
#define GONOME_AE_THROW			( 4 )

#define GONOME_AE_BITE1			( 19 )
#define GONOME_AE_BITE2			( 20 )
#define GONOME_AE_BITE3			( 21 )
#define GONOME_AE_BITE4			( 22 )

//=========================================================
// monster-specific schedule types
//=========================================================
enum
{
	SCHED_GONOME_SMELLFOOD = LAST_COMMON_SCHEDULE + 1,
	SCHED_GONOME_EAT,
	SCHED_GONOME_SNIFF_AND_EAT,
	SCHED_GONOME_WALLOW,
};

//=========================================================
// monster-specific tasks
//=========================================================
enum
{
	TASK_GONOME_SMELLFOOD = LAST_COMMON_SCHEDULE + 1,
};

//=========================================================
// CGonome
//=========================================================
LINK_ENTITY_TO_CLASS(monster_gonome, CGonome);

TYPEDESCRIPTION	CGonome::m_SaveData[] =
{
	DEFINE_FIELD(CGonome, m_fCanThreatDisplay, FIELD_BOOLEAN),
	DEFINE_FIELD(CGonome, m_flLastHurtTime, FIELD_TIME),
	DEFINE_FIELD(CGonome, m_flNextSpitTime, FIELD_TIME),
};

IMPLEMENT_SAVERESTORE(CGonome, CBaseMonster);

//=========================================================
// Monster Sounds
//=========================================================
const char *CGonome::pAttackHitSounds[] = {
	"zombie/claw_strike1.wav",
	"zombie/claw_strike2.wav",
	"zombie/claw_strike3.wav",
};

const char *CGonome::pAttackMissSounds[] = {
	"zombie/claw_miss1.wav",
	"zombie/claw_miss2.wav",
};

const char *CGonome::pAttackSounds[] = {
	"gonome/gonome_melee1.wav",
	"gonome/gonome_melee1.wav",
};

const char *CGonome::pIdleSounds[] = {
	"gonome/gonome_idle1.wav",
	"gonome/gonome_idle2.wav",
	"gonome/gonome_idle3.wav",
};

const char *CGonome::pPainSounds[] = {
	"gonome/gonome_pain1.wav",
	"gonome/gonome_pain2.wav",
	"gonome/gonome_pain3.wav",
	"gonome/gonome_pain4.wav",
};

const char *CGonome::pDeathSounds[] = {
	"gonome/gonome_death2.wav",
	"gonome/gonome_death3.wav",
	"gonome/gonome_death4.wav",
};

//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards. In the base class implementation,
// monsters care about all sounds, but no scents.
//=========================================================
int CGonome::ISoundMask(void)
{
	return	bits_SOUND_WORLD |
			bits_SOUND_COMBAT |
			bits_SOUND_CARCASS |
			bits_SOUND_MEAT |
			bits_SOUND_GARBAGE |
			bits_SOUND_PLAYER;
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CGonome::Classify(void)
{
	return m_iClass ? m_iClass : CLASS_ALIEN_MONSTER;
}

//=========================================================
// IgnoreConditions 
//=========================================================
int CGonome::IgnoreConditions(void)
{
	int iIgnore = CBaseMonster::IgnoreConditions();

	if ((m_Activity == ACT_MELEE_ATTACK1) || (m_Activity == ACT_MELEE_ATTACK1))
	{
		if (m_flNextFlinch >= UTIL_GlobalTimeBase())
			iIgnore |= (bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE);
	}

	if ((m_Activity == ACT_SMALL_FLINCH) || (m_Activity == ACT_BIG_FLINCH))
	{
		if (m_flNextFlinch < UTIL_GlobalTimeBase())
			m_flNextFlinch = UTIL_GlobalTimeBase() + GONOME_FLINCH_DELAY;
	}

	return iIgnore;
}

//=========================================================
// IRelationship - overridden for gonome
//=========================================================
int CGonome::IRelationship(CBaseEntity *pTarget)
{
	return CBaseMonster::IRelationship(pTarget);
}

//=========================================================
// TakeDamage - overridden for gonome so we can keep track
// of how much time has passed since it was last injured
//=========================================================
int CGonome::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	float flDist;
	Vector vecApex;

	// if the squid is running, has an enemy, was hurt by the enemy, hasn't been hurt in the last 3 seconds, and isn't too close to the enemy,
	// it will swerve. (whew).
	if (m_hEnemy != NULL && IsMoving() && pevAttacker == m_hEnemy->pev && UTIL_GlobalTimeBase() - m_flLastHurtTime > 3)
	{
		flDist = (pev->origin - m_hEnemy->pev->origin).Length2D();

		if (flDist > GONOME_SPRINT_DIST)
		{
			flDist = (pev->origin - m_Route[m_iRouteIndex].vecLocation).Length2D();// reusing flDist. 

			if (FTriangulate(pev->origin, m_Route[m_iRouteIndex].vecLocation, flDist * 0.5, m_hEnemy, &vecApex))
			{
				InsertWaypoint(vecApex, bits_MF_TO_DETOUR | bits_MF_DONT_SIMPLIFY);
			}
		}
	}

	return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}


//=========================================================
// CheckRangeAttack1
//=========================================================
BOOL CGonome::CheckRangeAttack1(float flDot, float flDist)
{
	if (IsMoving() && flDist >= 512)
	{
		// squid will far too far behind if he stops running to spit at this distance from the enemy.
		return FALSE;
	}

	if (flDist > 64 && flDist <= 784 && flDot >= 0.5 && UTIL_GlobalTimeBase() >= m_flNextSpitTime)
	{
		if (m_hEnemy != NULL)
		{
			if (fabs(pev->origin.z - m_hEnemy->pev->origin.z) > 256)
			{
				// don't try to spit at someone up really high or down really low.
				return FALSE;
			}
		}

		if (IsMoving())
		{
			// don't spit again for a long time, resume chasing enemy.
			m_flNextSpitTime = UTIL_GlobalTimeBase() + 5;
		}
		else
		{
			// not moving, so spit again pretty soon.
			m_flNextSpitTime = UTIL_GlobalTimeBase() + 0.5;
		}

		return TRUE;
	}

	return FALSE;
}

//=========================================================
// CheckMeleeAttack1 - bullsquid is a big guy, so has a longer
// melee range than most monsters. This is the tailwhip attack
//=========================================================
BOOL CGonome::CheckMeleeAttack1(float flDot, float flDist)
{
	if (flDist <= GONOME_TOLERANCE_MELEE1_RANGE &&
		flDot >= GONOME_TOLERANCE_MELEE1_DOT)
	{
		return TRUE;
	}

	return FALSE;
}

//=========================================================
// CheckMeleeAttack2 - bullsquid is a big guy, so has a longer
// melee range than most monsters. This is the bite attack.
// this attack will not be performed if the tailwhip attack
// is valid.
//=========================================================
BOOL CGonome::CheckMeleeAttack2(float flDot, float flDist)
{
	if (flDist <= GONOME_TOLERANCE_MELEE2_RANGE &&
		flDot >= GONOME_TOLERANCE_MELEE2_DOT &&
		!HasConditions(bits_COND_CAN_MELEE_ATTACK1))
	{
		return TRUE;
	}

	return FALSE;
}


//=========================================================
// IdleSound 
//=========================================================
#define GONOME_ATTN_IDLE	(float)1.5
void CGonome::IdleSound(void)
{
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pIdleSounds);
}

//=========================================================
// PainSound 
//=========================================================
void CGonome::PainSound(void)
{
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pPainSounds);
}

//=========================================================
// AlertSound
//=========================================================
void CGonome::AlertSound(void)
{
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pIdleSounds);
}

//=========================================================
// DeathSound 
//=========================================================
void CGonome::DeathSound(void) {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pDeathSounds);
}

//=========================================================
// AttackSound 
//=========================================================
void CGonome::AttackSound(void) {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pAttackSounds);
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CGonome::SetYawSpeed(void)
{
	int ys = 0;
	switch (m_Activity)
	{
	case	ACT_WALK:			ys = 120;	break;
	case	ACT_RUN:			ys = 120;	break;
	case	ACT_IDLE:			ys = 120;	break;
	case	ACT_RANGE_ATTACK1:	ys = 120;	break;
	default:
		ys = 120;
		break;
	}

	pev->yaw_speed = ys;
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CGonome::HandleAnimEvent(MonsterEvent_t *pEvent)
{
	switch (pEvent->event)
	{
		case GONOME_AE_THROW:
		{
			Vector	vecSpitOffset;
			Vector	vecSpitDir;

			UTIL_MakeVectors(pev->angles);

			// !!!HACKHACK - the spot at which the spit originates (in front of the mouth) was measured in 3ds and hardcoded here.
			// we should be able to read the position of bones at runtime for this info.

			Vector vecArmPos, vecArmAng;
			GetAttachment(0, vecArmPos, vecArmAng);

			//vecSpitOffset = (gpGlobals->v_right * 8 + gpGlobals->v_forward * 37 + gpGlobals->v_up * 23);
			//vecSpitOffset = (pev->origin + vecSpitOffset);
			vecSpitOffset = vecArmPos;
			vecSpitDir = ((m_hEnemy->pev->origin + m_hEnemy->pev->view_ofs) - vecSpitOffset).Normalize();

			vecSpitDir.x += RANDOM_FLOAT(-0.05, 0.05);
			vecSpitDir.y += RANDOM_FLOAT(-0.05, 0.05);
			vecSpitDir.z += RANDOM_FLOAT(-0.05, 0);

			// spew the spittle temporary ents.
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpitOffset);
			WRITE_BYTE(TE_SPRITE_SPRAY);
			WRITE_COORD(vecSpitOffset.x);	// pos
			WRITE_COORD(vecSpitOffset.y);
			WRITE_COORD(vecSpitOffset.z);
			WRITE_COORD(vecSpitDir.x);	// dir
			WRITE_COORD(vecSpitDir.y);
			WRITE_COORD(vecSpitDir.z);
			WRITE_SHORT(iGonomeSpitSprite);	// model
			WRITE_BYTE(15);			// count
			WRITE_BYTE(210);			// speed
			WRITE_BYTE(25);			// noise ( client will divide by 100 )
			MESSAGE_END();

			switch (RANDOM_LONG(0, 1)) {
				case 0:
					EMIT_SOUND(ENT(pev), CHAN_WEAPON, "bullchicken/bc_attack2.wav", 1, ATTN_NORM);
				break;
				case 1:
					EMIT_SOUND(ENT(pev), CHAN_WEAPON, "bullchicken/bc_attack3.wav", 1, ATTN_NORM);
				break;
			}

			CGonomeSpit::Shoot(pev, vecSpitOffset, vecSpitDir * 1200);
		}
		break;
		case GONOME_AE_SLASH_LEFT: {
			CBaseEntity *pHurt = CheckTraceHullAttack(GONOME_MELEE_ATTACK_RADIUS, gSkillData.gonomeDmgOneSlash, DMG_SLASH);
			if (pHurt) {
				if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT)) {
					pHurt->pev->punchangle.z = 20;
					pHurt->pev->punchangle.x = 20;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * 200;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 100;
				}
				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSounds);
			} else {
				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSounds);
			}
		}
		break;
		case GONOME_AE_SLASH_RIGHT: {
			CBaseEntity *pHurt = CheckTraceHullAttack(GONOME_MELEE_ATTACK_RADIUS, gSkillData.gonomeDmgOneSlash, DMG_SLASH);
			if (pHurt) {
				if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT)) {
					pHurt->pev->punchangle.z = -20;
					pHurt->pev->punchangle.x = 20;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * -200;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 100;
				}
				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSounds);
			} else {
				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSounds);
			}
		}
		break;

		case GONOME_AE_BITE1:
		case GONOME_AE_BITE2:
		case GONOME_AE_BITE3:
		case GONOME_AE_BITE4:
		{
			int iPitch;

			// SOUND HERE!
			CBaseEntity *pHurt = CheckTraceHullAttack(GONOME_TOLERANCE_MELEE2_RANGE, gSkillData.gonomeDmgOneBite, DMG_SLASH);

			if (pHurt)
			{
				// croonchy bite sound
				iPitch = RANDOM_FLOAT(90, 110);
				switch (RANDOM_LONG(0, 1))
				{
				case 0:
					EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "bullchicken/bc_bite2.wav", 1, ATTN_NORM, 0, iPitch);
					break;
				case 1:
					EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "bullchicken/bc_bite3.wav", 1, ATTN_NORM, 0, iPitch);
					break;
				}

				pHurt->pev->punchangle.z = RANDOM_LONG(-10, 10);
				pHurt->pev->punchangle.x = RANDOM_LONG(-35, -45);
				pHurt->pev->velocity = pHurt->pev->velocity - gpGlobals->v_forward * 50;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 50;
				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSounds);
			} else {
				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSounds);
			}
		}
		break;
		default:
			CBaseMonster::HandleAnimEvent(pEvent);
	}
}

//=========================================================
// Spawn
//=========================================================
void CGonome::Spawn()
{
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/gonome.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_GREEN;
	pev->effects = 0;

	if (pev->health == 0)
		pev->health = gSkillData.gonomeHealth;

	m_flFieldOfView = 0.2;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState = MONSTERSTATE_NONE;

	m_fCanThreatDisplay = TRUE;
	m_flNextSpitTime = UTIL_GlobalTimeBase();

	MonsterInit();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CGonome::Precache()
{
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/gonome.mdl");

	iGonomeSpitSprite = PRECACHE_MODEL("sprites/blood_tinyspit.spr");// client side spittle.

	PRECACHE_MODEL("sprites/blood_chnk.spr");// spit projectile.

	PRECACHE_SOUND("gonome/gonome_jumpattack.wav");

	PRECACHE_SOUND("gonome/gonome_run.wav");
	PRECACHE_SOUND("gonome/gonome_eat.wav");

	PRECACHE_SOUND("bullchicken/bc_acid1.wav");

	PRECACHE_SOUND("bullchicken/bc_bite2.wav");
	PRECACHE_SOUND("bullchicken/bc_bite3.wav");

	PRECACHE_SOUND("bullchicken/bc_spithit1.wav");
	PRECACHE_SOUND("bullchicken/bc_spithit2.wav");

	PRECACHE_SOUND("bullchicken/bc_attack2.wav");
	PRECACHE_SOUND("bullchicken/bc_attack3.wav");

	PRECACHE_SOUND_ARRAY(pAttackHitSounds);
	PRECACHE_SOUND_ARRAY(pAttackMissSounds);
	PRECACHE_SOUND_ARRAY(pAttackSounds);
	PRECACHE_SOUND_ARRAY(pIdleSounds);
	PRECACHE_SOUND_ARRAY(pPainSounds);
	PRECACHE_SOUND_ARRAY(pDeathSounds);
}

//========================================================
// RunAI - overridden for gonome because there are things
// that need to be checked every think.
//========================================================
void CGonome::RunAI(void)
{
	// first, do base class stuff
	CBaseMonster::RunAI();

	if (m_hEnemy != NULL && m_Activity == ACT_RUN)
	{
		// chasing enemy. Sprint for last bit
		if ((pev->origin - m_hEnemy->pev->origin).Length2D() < GONOME_SPRINT_DIST)
		{
			pev->framerate = 1.25;
		}
	}

}

//========================================================
// AI Schedules Specific to this monster
//=========================================================

// primary range attack
Task_t	tlGonomeRangeAttack1[] =
{
	{ TASK_STOP_MOVING,			0 },
	{ TASK_FACE_IDEAL,			(float)0 },
	{ TASK_RANGE_ATTACK1,		(float)0 },
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
};

Schedule_t	slGonomeRangeAttack1[] =
{
	{
		tlGonomeRangeAttack1,
		HL_ARRAYSIZE(tlGonomeRangeAttack1),
	bits_COND_NEW_ENEMY |
	bits_COND_ENEMY_DEAD |
	bits_COND_HEAVY_DAMAGE |
	bits_COND_ENEMY_OCCLUDED |
	bits_COND_NO_AMMO_LOADED,
	0,
	"Gonome Range Attack1"
	},
};

// Chase enemy schedule
Task_t tlGonomeChaseEnemy1[] =
{
	{ TASK_SET_FAIL_SCHEDULE,	(float)SCHED_RANGE_ATTACK1 },// !!!OEM - this will stop nasty gonome oscillation.
	{ TASK_GET_PATH_TO_ENEMY,	(float)0 },
	{ TASK_RUN_PATH,			(float)0 },
	{ TASK_WAIT_FOR_MOVEMENT,	(float)0 },
};

Schedule_t slGonomeChaseEnemy[] =
{
	{
		tlGonomeChaseEnemy1,
		HL_ARRAYSIZE(tlGonomeChaseEnemy1),
	bits_COND_NEW_ENEMY |
	bits_COND_ENEMY_DEAD |
	bits_COND_SMELL_FOOD |
	bits_COND_CAN_RANGE_ATTACK1 |
	bits_COND_CAN_MELEE_ATTACK1 |
	bits_COND_CAN_MELEE_ATTACK2 |
	bits_COND_TASK_FAILED |
	bits_COND_HEAR_SOUND,

	bits_SOUND_DANGER |
	bits_SOUND_MEAT,
	"Gonome Chase Enemy"
	},
};


// gonome walks to something tasty and eats it.
Task_t tlGonomeEat[] =
{
	{ TASK_STOP_MOVING,				(float)0 },
	{ TASK_EAT,						(float)10 },// this is in case the gonome can't get to the food
	{ TASK_STORE_LASTPOSITION,		(float)0 },
	{ TASK_GET_PATH_TO_BESTSCENT,	(float)0 },
	{ TASK_WALK_PATH,				(float)0 },
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0 },
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT },
	{ TASK_EAT,						(float)50 },
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0 },
	{ TASK_WALK_PATH,				(float)0 },
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0 },
	{ TASK_CLEAR_LASTPOSITION,		(float)0 },
};

Schedule_t slGonomeEat[] =
{
	{
		tlGonomeEat,
		HL_ARRAYSIZE(tlGonomeEat),
	bits_COND_LIGHT_DAMAGE |
	bits_COND_HEAVY_DAMAGE |
	bits_COND_NEW_ENEMY	,

	// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
	// here or the monster won't detect these sounds at ALL while running this schedule.
	bits_SOUND_MEAT |
	bits_SOUND_CARCASS,
	"GonomeEat"
	}
};

// this is a bit different than just Eat. We use this schedule when the food is far away, occluded, or behind
// the gonome. This schedule plays a sniff animation before going to the source of food.
Task_t tlGonomeSniffAndEat[] =
{
	{ TASK_STOP_MOVING,				(float)0 },
	{ TASK_EAT,						(float)10 },// this is in case the gonome can't get to the food
	{ TASK_PLAY_SEQUENCE,			(float)ACT_DETECT_SCENT },
	{ TASK_STORE_LASTPOSITION,		(float)0 },
	{ TASK_GET_PATH_TO_BESTSCENT,	(float)0 },
	{ TASK_WALK_PATH,				(float)0 },
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0 },
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE,			(float)ACT_EAT },
	{ TASK_EAT,						(float)50 },
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0 },
	{ TASK_WALK_PATH,				(float)0 },
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0 },
	{ TASK_CLEAR_LASTPOSITION,		(float)0 },
};

Schedule_t slGonomeSniffAndEat[] =
{
	{
		tlGonomeSniffAndEat,
		HL_ARRAYSIZE(tlGonomeSniffAndEat),
	bits_COND_LIGHT_DAMAGE |
	bits_COND_HEAVY_DAMAGE |
	bits_COND_NEW_ENEMY	,

	// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
	// here or the monster won't detect these sounds at ALL while running this schedule.
	bits_SOUND_MEAT |
	bits_SOUND_CARCASS,
	"GonomeSniffAndEat"
	}
};

// gonome does this to stinky things. 
Task_t tlGonomeWallow[] =
{
	{ TASK_STOP_MOVING,				(float)0 },
	{ TASK_EAT,						(float)10 },// this is in case the gonome can't get to the stinkiness
	{ TASK_STORE_LASTPOSITION,		(float)0 },
	{ TASK_GET_PATH_TO_BESTSCENT,	(float)0 },
	{ TASK_WALK_PATH,				(float)0 },
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0 },
	{ TASK_PLAY_SEQUENCE,			(float)ACT_INSPECT_FLOOR },
	{ TASK_EAT,						(float)50 },// keeps gonome from eating or sniffing anything else for a while.
	{ TASK_GET_PATH_TO_LASTPOSITION,(float)0 },
	{ TASK_WALK_PATH,				(float)0 },
	{ TASK_WAIT_FOR_MOVEMENT,		(float)0 },
	{ TASK_CLEAR_LASTPOSITION,		(float)0 },
};

Schedule_t slGonomeWallow[] =
{
	{
		tlGonomeWallow,
		HL_ARRAYSIZE(tlGonomeWallow),
	bits_COND_LIGHT_DAMAGE |
	bits_COND_HEAVY_DAMAGE |
	bits_COND_NEW_ENEMY	,

	// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
	// here or the monster won't detect these sounds at ALL while running this schedule.
	bits_SOUND_GARBAGE,

	"GonomeWallow"
	}
};

DEFINE_CUSTOM_SCHEDULES(CGonome)
{
	slGonomeRangeAttack1,
		slGonomeChaseEnemy,
		slGonomeEat,
		slGonomeSniffAndEat,
		slGonomeWallow
};

IMPLEMENT_CUSTOM_SCHEDULES(CGonome, CBaseMonster);

//=========================================================
// GetSchedule 
//=========================================================
Schedule_t *CGonome::GetSchedule(void)
{
	switch (m_MonsterState)
	{
	case MONSTERSTATE_ALERT:
	{

		if (HasConditions(bits_COND_SMELL_FOOD))
		{
			CSound		*pSound;

			pSound = PBestScent();

			if (pSound && (!FInViewCone(&pSound->m_vecOrigin) || !FVisible(pSound->m_vecOrigin)))
			{
				// scent is behind or occluded
				return GetScheduleOfType(SCHED_GONOME_SNIFF_AND_EAT);
			}

			// food is right out in the open. Just go get it.
			return GetScheduleOfType(SCHED_GONOME_EAT);
		}

		if (HasConditions(bits_COND_SMELL))
		{
			// there's something stinky. 
			CSound		*pSound;

			pSound = PBestScent();
			if (pSound)
				return GetScheduleOfType(SCHED_GONOME_WALLOW);
		}

		break;
	}
	case MONSTERSTATE_COMBAT:
	{
		// dead enemy
		if (HasConditions(bits_COND_ENEMY_DEAD))
		{
			// call base class, all code to handle dead enemies is centralized there.
			return CBaseMonster::GetSchedule();
		}

		if (HasConditions(bits_COND_NEW_ENEMY))
		{
			if (m_fCanThreatDisplay && IRelationship(m_hEnemy) == R_HT)
			{
				return GetScheduleOfType(SCHED_WAKE_ANGRY);
			}
		}

		if (HasConditions(bits_COND_SMELL_FOOD))
		{
			CSound		*pSound;

			pSound = PBestScent();

			if (pSound && (!FInViewCone(&pSound->m_vecOrigin) || !FVisible(pSound->m_vecOrigin)))
			{
				// scent is behind or occluded
				return GetScheduleOfType(SCHED_GONOME_SNIFF_AND_EAT);
			}

			// food is right out in the open. Just go get it.
			return GetScheduleOfType(SCHED_GONOME_EAT);
		}

		if (HasConditions(bits_COND_CAN_RANGE_ATTACK1))
		{
			return GetScheduleOfType(SCHED_RANGE_ATTACK1);
		}

		if (HasConditions(bits_COND_CAN_MELEE_ATTACK1))
		{
			return GetScheduleOfType(SCHED_MELEE_ATTACK1);
		}

		if (HasConditions(bits_COND_CAN_MELEE_ATTACK2))
		{
			return GetScheduleOfType(SCHED_MELEE_ATTACK2);
		}

		return GetScheduleOfType(SCHED_CHASE_ENEMY);

		break;
	}
	}

	return CBaseMonster::GetSchedule();
}

//=========================================================
// GetScheduleOfType
//=========================================================
Schedule_t* CGonome::GetScheduleOfType(int Type)
{
	switch (Type)
	{
	case SCHED_RANGE_ATTACK1:
		return &slGonomeRangeAttack1[0];
		break;
	case SCHED_GONOME_EAT:
		return &slGonomeEat[0];
		break;
	case SCHED_GONOME_SNIFF_AND_EAT:
		return &slGonomeSniffAndEat[0];
		break;
	case SCHED_GONOME_WALLOW:
		return &slGonomeWallow[0];
		break;
	case SCHED_CHASE_ENEMY:
		return &slGonomeChaseEnemy[0];
		break;
	}

	return CBaseMonster::GetScheduleOfType(Type);
}

//=========================================================
// Start task - selects the correct activity and performs
// any necessary calculations to start the next task on the
// schedule.  OVERRIDDEN for bullsquid because it needs to
// know explicitly when the last attempt to chase the enemy
// failed, since that impacts its attack choices.
//=========================================================
void CGonome::StartTask(Task_t *pTask)
{
	m_iTaskStatus = TASKSTATUS_RUNNING;

	switch (pTask->iTask)
	{
		case TASK_MELEE_ATTACK1:
		{
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "gonome/gonome_melee1.wav", 1, ATTN_NORM);
			CBaseMonster::StartTask(pTask);
		}
		break;
		case TASK_MELEE_ATTACK2:
		{
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "gonome/gonome_melee2.wav", 1, ATTN_NORM);
			CBaseMonster::StartTask(pTask);
		}
		break;
		case TASK_GET_PATH_TO_ENEMY:
		{
			if (BuildRoute(m_hEnemy->pev->origin, bits_MF_TO_ENEMY, m_hEnemy))
			{
				m_iTaskStatus = TASKSTATUS_COMPLETE;
			}
			else
			{
				ALERT(at_aiconsole, "GetPathToEnemy failed!!\n");
				TaskFail();
			}
		}
		break;
		default:
			CBullsquid::StartTask(pTask);
		break;

	}
}

//=========================================================
// RunTask
//=========================================================
void CGonome::RunTask(Task_t *pTask)
{
	CBaseMonster::RunTask(pTask);
}

//=========================================================
// GetIdealState - Overridden for Gonome to deal with
// the feature that makes it lose interest in headcrabs for 
// a while if something injures it. 
//=========================================================
MONSTERSTATE CGonome::GetIdealState(void)
{
	int	iConditions;
	iConditions = IScheduleFlags();
	m_IdealMonsterState = CBaseMonster::GetIdealState();
	return m_IdealMonsterState;
}
