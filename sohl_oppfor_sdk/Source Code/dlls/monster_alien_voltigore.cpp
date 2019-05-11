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
// NPC: Voltigore * http://half-life.wikia.com/wiki/Voltigore
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
#include    "monster_alien_voltigore.h"
#include    "proj_voltigore_energy_ball.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		VOLTIGORE_AE_ZAP_SHOOT		( 1 )
#define		VOLTIGORE_AE_ZAP_POWERUP	( 3 )
#define		VOLTIGORE_AE_ZAP_DONE		( 5 )
#define		VOLTIGORE_AE_CLAW			( 13)
#define		VOLTIGORE_AE_CLAWRAKE		( 12)
#define		VOLTIGORE_AE_RUN			( 14)
#define		VOLTIGORE_AE_STEP	        ( 1008 )
#define		VOLTIGORE_AE_GIB			( 2002 )

#define		VOLTIGORE_SPRINT_DIST	256 // how close the voltigore has to get before starting to sprint and refusing to swerve

//=========================================================
// Monster-specific schedule types
//=========================================================
enum {
	SCHED_VOLTIGORE_SMELLFOOD = LAST_COMMON_SCHEDULE + 1,
	SCHED_VOLTIGORE_EAT,
	SCHED_VOLTIGORE_SNIFF_AND_EAT,
	SCHED_VOLTIGORE_WALLOW,
};

//=========================================================
// monster-specific tasks
//=========================================================
enum {
	TASK_VOLTIGORE_NEWTASK = LAST_COMMON_TASK + 1,
};

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS(monster_alien_voltigore, CVoltigore);

TYPEDESCRIPTION	CVoltigore::m_SaveData[] = {
	DEFINE_FIELD(CVoltigore, m_flLastHurtTime, FIELD_TIME),
	DEFINE_FIELD(CVoltigore, m_flNextSpitTime, FIELD_TIME),
	DEFINE_FIELD(CVoltigore, m_fShouldUpdateBeam, FIELD_BOOLEAN),
	DEFINE_ARRAY(CVoltigore, m_pBeam, FIELD_CLASSPTR, 3),
	DEFINE_FIELD(CVoltigore, m_glowBrightness, FIELD_INTEGER),
	DEFINE_FIELD(CVoltigore, m_pBeamGlow, FIELD_CLASSPTR),
};

IMPLEMENT_SAVERESTORE(CVoltigore, CBaseMonster);

//=========================================================
// Monster Sounds
//=========================================================
const char* CVoltigore::pAlertSounds[] = {
	"voltigore/voltigore_alert1.wav",
	"voltigore/voltigore_alert2.wav",
	"voltigore/voltigore_alert3.wav",
};

const char* CVoltigore::pAttackMeleeSounds[] = {
	"voltigore/voltigore_attack_melee1.wav",
	"voltigore/voltigore_attack_melee2.wav",
};

const char* CVoltigore::pMeleeHitSounds[] = {
	"zombie/claw_strike1.wav",
	"zombie/claw_strike2.wav",
	"zombie/claw_strike3.wav",
};

const char* CVoltigore::pMeleeMissSounds[] = {
	"zombie/claw_miss1.wav",
	"zombie/claw_miss2.wav",
};

const char* CVoltigore::pComSounds[] = {
	"voltigore/voltigore_communicate1.wav",
	"voltigore/voltigore_communicate2.wav",
	"voltigore/voltigore_communicate3.wav",
};

const char* CVoltigore::pDeathSounds[] = {
	"voltigore/voltigore_die1.wav",
	"voltigore/voltigore_die2.wav",
	"voltigore/voltigore_die3.wav",
};

const char* CVoltigore::pFootstepSounds[] = {
	"voltigore/voltigore_footstep1.wav",
	"voltigore/voltigore_footstep2.wav",
	"voltigore/voltigore_footstep3.wav",
};

const char* CVoltigore::pIdleSounds[] = {
	"voltigore/voltigore_idle1.wav",
	"voltigore/voltigore_idle2.wav",
	"voltigore/voltigore_idle3.wav",
};

const char* CVoltigore::pPainSounds[] = {
	"voltigore/voltigore_pain1.wav",
	"voltigore/voltigore_pain2.wav",
	"voltigore/voltigore_pain3.wav",
	"voltigore/voltigore_pain4.wav",
};

const char* CVoltigore::pGruntSounds[] = {
	"voltigore/voltigore_run_grunt1.wav",
	"voltigore/voltigore_run_grunt2.wav",
};

//=========================================================
// Spawn Voltigore
//=========================================================
void CVoltigore::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/voltigore.mdl");

	UTIL_SetSize(pev, Vector(-80, -80, 0), Vector(80, 80, 90));

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_GREEN;
	pev->effects = 0;
	pev->health = gSkillData.voltigoreHealth;
	m_flFieldOfView = 0.2;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState = MONSTERSTATE_NONE;
	m_flNextSpitTime = UTIL_GlobalTimeBase();
	m_flDebug = false; //Debug Massages

	pev->view_ofs = Vector(0, 0, 90);

	m_fShouldUpdateBeam = FALSE;
	m_pBeamGlow = NULL;

	GlowOff();
	CreateGlow();

	m_flHitgroupHead = gSkillData.voltigoreHead;
	m_flHitgroupChest = gSkillData.voltigoreChest;
	m_flHitgroupStomach = gSkillData.voltigoreStomach;
	m_flHitgroupArm = gSkillData.voltigoreArm;
	m_flHitgroupLeg = gSkillData.voltigoreLeg;
	m_flDmgPunch = gSkillData.voltigoreDmgPunch;

	MonsterInit();
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CVoltigore::Classify(void) {
	return m_iClass ? m_iClass : CLASS_ALIEN_MONSTER;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CVoltigore::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/voltigore.mdl");

	m_iVgib = PRECACHE_MODEL("models/vgibs.mdl");

	PRECACHE_SOUND_ARRAY(pAlertSounds);
	PRECACHE_SOUND_ARRAY(pAttackMeleeSounds);
	PRECACHE_SOUND_ARRAY(pMeleeHitSounds);
	PRECACHE_SOUND_ARRAY(pMeleeMissSounds);
	PRECACHE_SOUND_ARRAY(pComSounds);
	PRECACHE_SOUND_ARRAY(pDeathSounds);
	PRECACHE_SOUND_ARRAY(pFootstepSounds);
	PRECACHE_SOUND_ARRAY(pIdleSounds);
	PRECACHE_SOUND_ARRAY(pPainSounds);
	PRECACHE_SOUND_ARRAY(pGruntSounds);

	PRECACHE_SOUND("voltigore/voltigore_attack_shock.wav");
	PRECACHE_SOUND("voltigore/voltigore_eat.wav");

	PRECACHE_SOUND("debris/beamstart1.wav");

	PRECACHE_MODEL("sprites/lgtning.spr");
	PRECACHE_MODEL("sprites/glow04.spr");
	PRECACHE_MODEL("sprites/blueflare1.spr");

	PRECACHE_SOUND("debris/zap4.wav");

	PRECACHE_MODEL("sprites/glow_prp.spr");
	UTIL_PrecacheOther("voltigore_energy_ball");
}

//=========================================================
// TraceAttack - Damage based on Hitgroups
//=========================================================
void CVoltigore::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
		return;
	}

	if (pev->takedamage) {
		if (IsAlive() && RANDOM_LONG(0, 4) <= 2) { PainSound(); }
		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_64) {
			CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
			if (pEnt->IsPlayer()) { CBaseMonster::TraceAttack(pevAttacker, 0, vecDir, ptr, bitsDamageType); return; }
			if (pevAttacker->owner) {
				pEnt = CBaseEntity::Instance(pevAttacker->owner);
				if (pEnt->IsPlayer()) { CBaseMonster::TraceAttack(pevAttacker, 0, vecDir, ptr, bitsDamageType); return; }
			}
		}

		switch (ptr->iHitgroup) {
		case HITGROUP_HEAD:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_HEAD\n", STRING(pev->classname));
			flDamage = m_flHitgroupHead * flDamage;
			break;
		case HITGROUP_CHEST:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_CHEST\n", STRING(pev->classname));
			flDamage = m_flHitgroupChest * flDamage;
			break;
		case HITGROUP_STOMACH:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_STOMACH\n", STRING(pev->classname));
			flDamage = m_flHitgroupStomach * flDamage;
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_ARM\n", STRING(pev->classname));
			flDamage = m_flHitgroupArm * flDamage;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_LEG\n", STRING(pev->classname));
			flDamage = m_flHitgroupLeg * flDamage;
			break;
		}
	}

	SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);// a little surface blood.
	TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
	CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}

//=========================================================
// IgnoreConditions 
//=========================================================
int CVoltigore::IgnoreConditions(void) {
	int iIgnore = CBaseMonster::IgnoreConditions();
	if (UTIL_GlobalTimeBase() - m_flLastHurtTime <= 20) {
		// haven't been hurt in 20 seconds, so let the voltigore care about stink. 
		iIgnore = bits_COND_SMELL | bits_COND_SMELL_FOOD;
	}

	return iIgnore;
}

//=========================================================
// TakeDamage - overridden for voltigore so we can keep track
// of how much time has passed since it was last injured
//=========================================================
int CVoltigore::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) {
	float flDist;
	Vector vecApex;

	// if the voltigore is running, has an enemy, was hurt by the enemy, hasn't been hurt in the last 3 seconds, and isn't too close to the enemy,
	// it will swerve. (whew).
	if (m_hEnemy != NULL && IsMoving() && pevAttacker == m_hEnemy->pev && UTIL_GlobalTimeBase() - m_flLastHurtTime > 3) {
		flDist = (pev->origin - m_hEnemy->pev->origin).Length2D();

		if (flDist > VOLTIGORE_SPRINT_DIST) {
			flDist = Vector(pev->origin - m_Route[m_iRouteIndex].vecLocation).Length2D();// reusing flDist. 

			if (FTriangulate(pev->origin, m_Route[m_iRouteIndex].vecLocation, flDist * 0.5, m_hEnemy, &vecApex)) {
				InsertWaypoint(vecApex, bits_MF_TO_DETOUR | bits_MF_DONT_SIMPLIFY);
			}
		}
	}

	return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

//=========================================================
// CheckRangeAttack1
//=========================================================
bool CVoltigore::CheckRangeAttack1(float flDot, float flDist) {
	if (!CanThrowEnergyBall()) {
		return false;
	}

	if (IsMoving() && flDist >= 512) {
		// voltigore will far too far behind if he stops running to spit at this distance from the enemy.
		return false;
	}

	if (flDist > 64 && flDist <= 784 && flDot >= 0.5 && UTIL_GlobalTimeBase() >= m_flNextSpitTime) {
		if (m_hEnemy != NULL) {
			if (fabs(pev->origin.z - m_hEnemy->pev->origin.z) > 256) {
				// don't try to spit at someone up really high or down really low.
				return false;
			}
		}

		if (IsMoving()) {
			// don't spit again for a long time, resume chasing enemy.
			m_flNextSpitTime = UTIL_GlobalTimeBase() + 5;
		}
		else {
			// not moving, so spit again pretty soon.
			m_flNextSpitTime = UTIL_GlobalTimeBase() + 0.5;
		}

		return true;
	}

	return false;
}

//=========================================================
//=========================================================
void CVoltigore::RunAI(void) {
	CBaseMonster::RunAI();
	if (m_fShouldUpdateBeam) {
		UpdateBeams();
	}

	GlowUpdate();
}

//=========================================================
// CheckMeleeAttack1 - voltigore is a big guy, so has a longer
// melee range than most monsters. This is the tailwhip attack
//=========================================================
bool CVoltigore::CheckMeleeAttack1(float flDot, float flDist) {
	if (flDist <= fabs(pev->mins.x - pev->maxs.x) && flDot >= 0.7) {
		return true;
	}

	return false;
}

//=========================================================
// CheckMeleeAttack2 - voltigore is a big guy, so has a longer
// melee range than most monsters. This is the bite attack.
// this attack will not be performed if the tailwhip attack
// is valid.
//=========================================================
bool CVoltigore::CheckMeleeAttack2(float flDot, float flDist) {
	if (!HasConditions(bits_COND_CAN_MELEE_ATTACK1)) {
		return true;
	}

	return false;
}

//=========================================================
//  FValidateHintType 
//=========================================================
BOOL CVoltigore::FValidateHintType(short sHint) {
	int i;
	static short sVoltigoreHints[] = {
		HINT_WORLD_HUMAN_BLOOD,
	};

	for (i = 0; i < HL_ARRAYSIZE(sVoltigoreHints); i++) {
		if (sVoltigoreHints[i] == sHint)
		{
			return TRUE;
		}
	}

	ALERT(at_aiconsole, "Couldn't validate hint type");
	return FALSE;
}

//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards. In the base class implementation,
// monsters care about all sounds, but no scents.
//=========================================================
int CVoltigore::ISoundMask(void) {
	return	bits_SOUND_WORLD |
		bits_SOUND_COMBAT |
		bits_SOUND_CARCASS |
		bits_SOUND_MEAT |
		bits_SOUND_GARBAGE |
		bits_SOUND_PLAYER;
}

//=========================================================
// IdleSound 
//=========================================================
void CVoltigore::IdleSound(void) {
	EMIT_SOUND(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pIdleSounds), VOL_NORM, ATTN_NORM);
}

//=========================================================
// PainSound 
//=========================================================
void CVoltigore::PainSound(void) {
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pPainSounds), VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(85, 120));
}

//=========================================================
// AlertSound
//=========================================================
void CVoltigore::AlertSound(void) {
	if (m_hEnemy != NULL) {
		SENTENCEG_PlayRndSz(ENT(pev), "VLT_ALERT", 0.85, ATTN_NORM, 0, RANDOM_LONG(85, 120));
		CallForHelp("monster_voltigore", 512, m_hEnemy, m_vecEnemyLKP);
	}

	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pAlertSounds), 1, ATTN_NORM, 0, RANDOM_LONG(140, 160));
}

//=========================================================
// CallForHelp
//=========================================================
void CVoltigore::CallForHelp(char *szClassname, float flDist, EHANDLE hEnemy, Vector &vecLocation) {
	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByString(pEntity, "netname", STRING(pev->netname))) != NULL) {
		float d = (pev->origin - pEntity->pev->origin).Length();
		if (d < flDist) {
			CBaseMonster *pMonster = pEntity->MyMonsterPointer();
			if (pMonster)
			{
				pMonster->m_afMemory |= bits_MEMORY_PROVOKED;
				pMonster->PushEnemy(hEnemy, vecLocation);
			}
		}
	}
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CVoltigore::SetYawSpeed(void) {
	int ys = 0;
	switch (m_Activity) {
	case	ACT_WALK:			ys = 90;	break;
	case	ACT_RUN:			ys = 90;	break;
	case	ACT_IDLE:			ys = 90;	break;
	case	ACT_RANGE_ATTACK1:	ys = 90;	break;
	default:
		ys = 90;
		break;
	}

	pev->yaw_speed = ys;
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CVoltigore::HandleAnimEvent(MonsterEvent_t *pEvent) {
	switch (pEvent->event) {
	case VOLTIGORE_AE_ZAP_SHOOT: {
		Vector	vecSpitOffset;
		Vector	vecSpitDir;

		UTIL_MakeVectors(pev->angles);
		vecSpitOffset = (gpGlobals->v_right * 8 + gpGlobals->v_forward * 37 + gpGlobals->v_up * 23);
		vecSpitOffset = (pev->origin + vecSpitOffset);
		vecSpitDir = Vector((m_hEnemy->pev->origin + m_hEnemy->pev->view_ofs) - vecSpitOffset).Normalize();

		vecSpitDir.x += RANDOM_FLOAT(-0.05, 0.05);
		vecSpitDir.y += RANDOM_FLOAT(-0.05, 0.05);
		vecSpitDir.z += RANDOM_FLOAT(-0.05, 0);

		// do stuff for this event.
		AttackSound();
		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "voltigore/voltigore_attack_shock.wav", 1, ATTN_NORM, 0, 100 + m_iBeams * 10);
		pev->skin = m_iBeams / 2;

		CVoltigoreEnergyBall::Shoot(pev, vecSpitOffset, vecSpitDir * 1000);

		// turn the beam glow off.
		ClearBeams();
		GlowOff();
		m_fShouldUpdateBeam = FALSE;
	}
								 break;
	case VOLTIGORE_AE_CLAW: {
		CBaseEntity *pHurt = CheckTraceHullAttack(120, m_flDmgPunch, DMG_CLUB | DMG_ALWAYSGIB);
		if (pHurt) {
			if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT)) {
				pHurt->pev->punchangle.z = -50;
				pHurt->pev->punchangle.x = 35;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * -200;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * 160;
				UTIL_ScreenShake(pHurt->pev->origin, 8.0, 1.5, 0.7, 2);
			}

			PrintBloodDecal(pHurt, pHurt->pev->origin, pHurt->pev->velocity, RANDOM_FLOAT(80, 90));
			EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pMeleeHitSounds);
		}
		else
			EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pMeleeMissSounds);
	}
							break;
	case VOLTIGORE_AE_CLAWRAKE: {
		CBaseEntity *pHurt = CheckTraceHullAttack(120, m_flDmgPunch, DMG_CLUB | DMG_ALWAYSGIB);
		if (pHurt) {
			if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT)) {
				pHurt->pev->punchangle.x = 40;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * 250;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 150;
				UTIL_ScreenShake(pHurt->pev->origin, 8.0, 1.5, 0.7, 80);
			}

			PrintBloodDecal(pHurt, pHurt->pev->origin, Vector(0, 0, -1), 90);
			EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pMeleeHitSounds);
		}
		else
			EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pMeleeMissSounds);
	}
								break;
	case VOLTIGORE_AE_GIB:
	case VOLTIGORE_AE_STEP: {
		UTIL_ScreenShake(pev->origin, 8.0, 1.5, 0.7, 80);
	}
							break;
	default:
		CBaseMonster::HandleAnimEvent(pEvent);
		break;
	}
}

//=========================================================
// DeathSound
//=========================================================
void CVoltigore::DeathSound(void) {
	EMIT_SOUND(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pDeathSounds), VOL_NORM, ATTN_NORM);
}

//=========================================================
// AttackSound
//=========================================================
void CVoltigore::AttackSound(void) {
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "voltigore/voltigore_attack_shock.wav", VOL_NORM, ATTN_NORM);
}

//========================================================
// AI Schedules Specific to this monster
//=========================================================

// primary range attack
Task_t	tlVoltigoreRangeAttack1[] = {
	{ TASK_STOP_MOVING, 0 },
	{ TASK_FACE_IDEAL, (float)0 },
	{ TASK_RANGE_ATTACK1, (float)0 },
	{ TASK_SET_ACTIVITY, (float)ACT_IDLE },
};

Schedule_t	slVoltigoreRangeAttack1[] = {
	{
		tlVoltigoreRangeAttack1,
		HL_ARRAYSIZE(tlVoltigoreRangeAttack1),
	bits_COND_NEW_ENEMY |
	bits_COND_ENEMY_DEAD |
	bits_COND_HEAVY_DAMAGE |
	bits_COND_ENEMY_OCCLUDED |
	bits_COND_NO_AMMO_LOADED,
	0,
	"Voltigore Range Attack1"
	},
};

// Chase enemy schedule
Task_t tlVoltigoreChaseEnemy1[] = {
	{ TASK_SET_FAIL_SCHEDULE, (float)SCHED_RANGE_ATTACK1 },// !!!OEM - this will stop nasty voltigore oscillation.
	{ TASK_GET_PATH_TO_ENEMY, (float)0 },
	{ TASK_RUN_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
};

Schedule_t slVoltigoreChaseEnemy[] = {
	{
		tlVoltigoreChaseEnemy1,
		HL_ARRAYSIZE(tlVoltigoreChaseEnemy1),
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
	"Voltigore Chase Enemy"
	},
};

// Voltigore walks to something tasty and eats it.
Task_t tlVoltigoreEat[] = {
	{ TASK_STOP_MOVING, (float)0 },
	{ TASK_EAT, (float)10 },// this is in case the voltigore can't get to the food
	{ TASK_STORE_LASTPOSITION, (float)0 },
	{ TASK_GET_PATH_TO_BESTSCENT, (float)0 },
	{ TASK_WALK_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
	{ TASK_PLAY_SEQUENCE, (float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE, (float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE, (float)ACT_EAT },
	{ TASK_EAT, (float)50 },
	{ TASK_GET_PATH_TO_LASTPOSITION, (float)0 },
	{ TASK_WALK_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
	{ TASK_CLEAR_LASTPOSITION, (float)0 },
};

Schedule_t slVoltigoreEat[] = {
	{
		tlVoltigoreEat,
		HL_ARRAYSIZE(tlVoltigoreEat),
	bits_COND_LIGHT_DAMAGE |
	bits_COND_HEAVY_DAMAGE |
	bits_COND_NEW_ENEMY,

	// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
	// here or the monster won't detect these sounds at ALL while running this schedule.
	bits_SOUND_MEAT |
	bits_SOUND_CARCASS,
	"VoltigoreEat"
	}
};

// this is a bit different than just Eat. We use this schedule when the food is far away, occluded, or behind
// the voltigore. This schedule plays a sniff animation before going to the source of food.
Task_t tlVoltigoreSniffAndEat[] = {
	{ TASK_STOP_MOVING, (float)0 },
	{ TASK_EAT, (float)10 },// this is in case the voltigore can't get to the food
	{ TASK_PLAY_SEQUENCE, (float)ACT_DETECT_SCENT },
	{ TASK_STORE_LASTPOSITION, (float)0 },
	{ TASK_GET_PATH_TO_BESTSCENT, (float)0 },
	{ TASK_WALK_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
	{ TASK_PLAY_SEQUENCE, (float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE, (float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE, (float)ACT_EAT },
	{ TASK_EAT, (float)50 },
	{ TASK_GET_PATH_TO_LASTPOSITION, (float)0 },
	{ TASK_WALK_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
	{ TASK_CLEAR_LASTPOSITION, (float)0 },
};

Schedule_t slVoltigoreSniffAndEat[] = {
	{
		tlVoltigoreSniffAndEat,
		HL_ARRAYSIZE(tlVoltigoreSniffAndEat),
	bits_COND_LIGHT_DAMAGE |
	bits_COND_HEAVY_DAMAGE |
	bits_COND_NEW_ENEMY,

	// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
	// here or the monster won't detect these sounds at ALL while running this schedule.
	bits_SOUND_MEAT |
	bits_SOUND_CARCASS,
	"VoltigoreSniffAndEat"
	}
};

DEFINE_CUSTOM_SCHEDULES(CVoltigore) {
	slVoltigoreRangeAttack1,
		slVoltigoreChaseEnemy,
		slVoltigoreEat,
		slVoltigoreSniffAndEat,
};

IMPLEMENT_CUSTOM_SCHEDULES(CVoltigore, CBaseMonster);

//=========================================================
// GetSchedule 
//=========================================================
Schedule_t *CVoltigore::GetSchedule(void) {
	switch (m_MonsterState) {
	case MONSTERSTATE_ALERT: {
		if (HasConditions(bits_COND_SMELL_FOOD)) {
			CSound		*pSound;
			pSound = PBestScent();
			if (pSound && (!FInViewCone(&pSound->m_vecOrigin) || !FVisible(pSound->m_vecOrigin))) {
				// scent is behind or occluded
				return GetScheduleOfType(SCHED_VOLTIGORE_SNIFF_AND_EAT);
			}

			// food is right out in the open. Just go get it.
			return GetScheduleOfType(SCHED_VOLTIGORE_EAT);
		}
		break;
	}
	case MONSTERSTATE_COMBAT: {
		// dead enemy
		if (HasConditions(bits_COND_ENEMY_DEAD)) {
			// call base class, all code to handle dead enemies is centralized there.
			return CBaseMonster::GetSchedule();
		}

		if (HasConditions(bits_COND_NEW_ENEMY)) {
			return GetScheduleOfType(SCHED_WAKE_ANGRY);
		}

		if (HasConditions(bits_COND_CAN_RANGE_ATTACK1)) {
			return GetScheduleOfType(SCHED_RANGE_ATTACK1);
		}

		if (HasConditions(bits_COND_CAN_MELEE_ATTACK1)) {
			return GetScheduleOfType(SCHED_MELEE_ATTACK1);
		}

		if (HasConditions(bits_COND_CAN_MELEE_ATTACK2)) {
			return GetScheduleOfType(SCHED_MELEE_ATTACK2);
		}

		return GetScheduleOfType(SCHED_CHASE_ENEMY);
	}
							  break;
	}

	return CBaseMonster::GetSchedule();
}

//=========================================================
// GetScheduleOfType
//=========================================================
Schedule_t* CVoltigore::GetScheduleOfType(int Type) {
	switch (Type) {
	case SCHED_RANGE_ATTACK1:
		return &slVoltigoreRangeAttack1[0];
		break;
	case SCHED_VOLTIGORE_EAT:
		return &slVoltigoreEat[0];
		break;
	case SCHED_VOLTIGORE_SNIFF_AND_EAT:
		return &slVoltigoreSniffAndEat[0];
		break;
	case SCHED_CHASE_ENEMY:
		return &slVoltigoreChaseEnemy[0];
		break;
	}

	return CBaseMonster::GetScheduleOfType(Type);
}

//=========================================================
// Start task - selects the correct activity and performs
// any necessary calculations to start the next task on the
// schedule.  OVERRIDDEN for voltigore because it needs to
// know explicitly when the last attempt to chase the enemy
// failed, since that impacts its attack choices.
//=========================================================
void CVoltigore::StartTask(Task_t *pTask) {
	m_iTaskStatus = TASKSTATUS_RUNNING;
	switch (pTask->iTask) {
	case TASK_RANGE_ATTACK1: {
		CreateBeams();

		GlowOn(255);
		m_fShouldUpdateBeam = TRUE;

		// Play the beam 'glow' sound.
		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "debris/zap4.wav", VOL_NORM, ATTN_NORM, 0, PITCH_HIGH);
		EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "debris/beamstart1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_HIGH);
		CBaseMonster::StartTask(pTask);
	}
							 break;
	case TASK_GET_PATH_TO_ENEMY: {
		if (BuildRoute(m_hEnemy->pev->origin, bits_MF_TO_ENEMY, m_hEnemy)) {
			m_iTaskStatus = TASKSTATUS_COMPLETE;
		}
		else {
			ALERT(at_aiconsole, "GetPathToEnemy failed!!\n");
			TaskFail();
		}
	}
								 break;

	default:
		CBaseMonster::StartTask(pTask);
		break;
	}
}


//=========================================================
// GetIdealState - Overridden for Voltigore to deal with
// the feature that makes it lose interest in headcrabs for 
// a while if something injures it. 
//=========================================================
MONSTERSTATE CVoltigore::GetIdealState(void) {
	m_IdealMonsterState = CBaseMonster::GetIdealState();
	return m_IdealMonsterState;
}

void CVoltigore::Killed(entvars_t *pevAttacker, int iGib) {
	ClearBeams();
	DestroyGlow();
	DieBlast();

	Vector vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;

	// gibs
	vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
	WRITE_BYTE(TE_BREAKMODEL);
	WRITE_COORD(vecSpot.x);
	WRITE_COORD(vecSpot.y);
	WRITE_COORD(vecSpot.z);
	WRITE_COORD(100);
	WRITE_COORD(100);
	WRITE_COORD(32);
	WRITE_COORD(0);
	WRITE_COORD(0);
	WRITE_COORD(200);
	WRITE_BYTE(8);
	WRITE_SHORT(m_iVgib);	//model id#
	WRITE_BYTE(8);
	WRITE_BYTE(200);// 10.0 seconds
	WRITE_BYTE(BREAK_FLESH);
	MESSAGE_END();

	SetThink(&CVoltigore::SUB_Remove);
	SetNextThink(0.1);

	CBaseMonster::Killed(pevAttacker, iGib);
}

void CVoltigore::CreateBeams() {
	Vector vecStart, vecEnd, vecAngles;
	GetAttachment(3, vecStart, vecAngles);

	if (m_iBeams >= VOLTIGORE_MAX_BEAMS)
		return;

	for (int i = 0; i < VOLTIGORE_MAX_BEAMS; i++) {
		m_pBeam[i] = CBeam::BeamCreate("sprites/lgtning.spr", 30);
		if (!m_pBeam[i])
			return;

		GetAttachment(i, vecEnd, vecAngles);
		m_pBeam[i]->PointsInit(vecStart, vecEnd);
		m_pBeam[i]->SetColor(125, 61, 177);
		m_pBeam[i]->SetBrightness(255);
		m_pBeam[i]->SetNoise(80);
		m_pBeam[i]->SetFlags(SF_BEAM_SHADEIN);
		m_iBeams++;
	}
}

void CVoltigore::UpdateBeams() {
	Vector vecStart, vecEnd, vecAngles, vecDis;
	GetAttachment(3, vecStart, vecAngles);

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecStart);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(vecStart.x);	// X
	WRITE_COORD(vecStart.y);	// Y
	WRITE_COORD(vecStart.z);	// Z
	WRITE_BYTE(16);		// radius * 0.1
	WRITE_BYTE(225);     // R
	WRITE_BYTE(115);     // G
	WRITE_BYTE(255);     // B
	WRITE_BYTE(3);		// time * 10
	WRITE_BYTE(0);		// decay * 0.1
	MESSAGE_END();

	for (int i = 0; i < VOLTIGORE_MAX_BEAMS; i++) {
		GetAttachment(i, vecEnd, vecAngles);
		m_pBeam[i]->SetStartPos(vecStart);
		m_pBeam[i]->SetEndPos(vecEnd);
		m_pBeam[i]->RelinkBeam();
	}

	pBeam = CBeam::BeamCreate("sprites/lgtning.spr", 10);
	pBeam->PointEntInit(vecStart, entindex());
	pBeam->SetColor(125, 61, 177);
	pBeam->SetNoise(80);
	pBeam->SetBrightness(200);
	pBeam->SetWidth(30);
	pBeam->SetScrollRate(35);
	pBeam->LiveForTime(0.3);
}

void CVoltigore::CreateGlow() {
	m_pBeamGlow = CSprite::SpriteCreate("sprites/glow04.spr", pev->origin, FALSE);
	m_pBeamGlow->SetTransparency(kRenderGlow, 255, 255, 255, 0, kRenderFxNoDissipation);
	m_pBeamGlow->SetAttachment(edict(), 4);
	m_pBeamGlow->pev->scale = 0.2;

	m_pSprite = CSprite::SpriteCreate("sprites/blueflare1.spr", pev->origin, FALSE);
	m_pSprite->SetAttachment(edict(), 4);
	m_pSprite->pev->scale = 1;
	m_pSprite->SetTransparency(kRenderGlow, 125, 61, 177, 255, kRenderFxNoDissipation);
	m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;
	m_pSprite->pev->flags |= FL_SKIPLOCALHOST;
}

void CVoltigore::DestroyGlow() {
	if (m_pBeamGlow) {
		UTIL_Remove(m_pBeamGlow);
		m_pBeamGlow = NULL;
	}

	if (m_pSprite) {
		UTIL_Remove(m_pSprite);
		m_pSprite = NULL;
	}
}

void CVoltigore::GlowUpdate() {
	if (m_pBeamGlow) {
		m_pBeamGlow->pev->renderamt = UTIL_Approach(m_glowBrightness, m_pBeamGlow->pev->renderamt, 100);
		if (m_pBeamGlow->pev->renderamt == 0)
			m_pBeamGlow->pev->effects |= EF_NODRAW;
		else
			m_pBeamGlow->pev->effects &= ~EF_NODRAW;

		UTIL_SetOrigin(m_pBeamGlow, pev->origin);
	}

	if (m_pSprite) {
		m_pSprite->pev->renderamt = UTIL_Approach(m_glowBrightness, m_pSprite->pev->renderamt, 100);
		if (m_pSprite->pev->renderamt == 0)
			m_pSprite->pev->effects |= EF_NODRAW;
		else
			m_pSprite->pev->effects &= ~EF_NODRAW;

		UTIL_SetOrigin(m_pSprite, pev->origin);
	}
}

void CVoltigore::GlowOff(void) {
	m_glowBrightness = 0;
}

void CVoltigore::GlowOn(int level) {
	m_glowBrightness = level;
}

void CVoltigore::PrintBloodDecal(CBaseEntity* pHurt, const Vector& vecOrigin, const Vector& vecVelocity, float maxDist, int bloodColor) {
	if (!pHurt)
		return;

	if (bloodColor == DONT_BLEED) {
		CBaseMonster* pMonster = pHurt->MyMonsterPointer();
		if (pMonster)
			bloodColor = pMonster->m_bloodColor;
	}

	TraceResult tr;
	UTIL_TraceLine(vecOrigin, vecOrigin + vecVelocity * maxDist, ignore_monsters, ENT(pev), &tr);
	UTIL_BloodDecalTrace(&tr, bloodColor);
}

void CVoltigore::ClearBeams() {
	for (int i = 0; i < VOLTIGORE_MAX_BEAMS; i++) {
		if (m_pBeam[i]) {
			UTIL_Remove(m_pBeam[i]);
			m_pBeam[i] = NULL;
		}

		if (m_pSprite)
		{
			UTIL_Remove(m_pSprite);
			m_pSprite = NULL;
		}
	}

	m_iBeams = 0;
	pev->skin = 0;

	STOP_SOUND(ENT(pev), CHAN_WEAPON, "debris/zap4.wav");
}

void CVoltigore::DieBlast() {
	int iTimes = 0;
	int iDrawn = 0;
	TraceResult tr;
	Vector vecDest;
	CBeam *pBeam;
	pev->origin.z = pev->origin.z + 64;

	while (iDrawn < 12 && iTimes < (12 * 3)) {
		vecDest = 90 * (Vector(RANDOM_FLOAT(-1, 1), RANDOM_FLOAT(-1, 1), RANDOM_FLOAT(-1, 1)).Normalize());
		UTIL_TraceLine(pev->origin, pev->origin + vecDest, ignore_monsters, NULL, &tr);
		if (tr.flFraction != 1.0) {
			// we hit something.
			iDrawn++;
			pBeam = CBeam::BeamCreate("sprites/plasma.spr", 200);
			pBeam->PointsInit(tr.vecEndPos, pev->origin);
			pBeam->SetStartPos(tr.vecEndPos);
			pBeam->SetEndPos(pev->origin);
			pBeam->SetColor(255, 0, 255);
			pBeam->SetNoise(65);
			pBeam->SetBrightness(255);
			pBeam->SetWidth(30);
			pBeam->SetScrollRate(35);
			pBeam->LiveForTime(2.5);
		}
		iTimes++;
	}

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(pev->origin.x);	// X
	WRITE_COORD(pev->origin.y);	// Y
	WRITE_COORD(pev->origin.z);	// Z
	WRITE_BYTE(16);		// radius * 0.1
	WRITE_BYTE(225);     // R
	WRITE_BYTE(115);     // G
	WRITE_BYTE(255);     // B
	WRITE_BYTE(8);		// time * 10
	WRITE_BYTE(0);		// decay * 0.1
	MESSAGE_END();

	pev->origin.z = pev->origin.z - 64;
	SetNextThink(0.5);
}