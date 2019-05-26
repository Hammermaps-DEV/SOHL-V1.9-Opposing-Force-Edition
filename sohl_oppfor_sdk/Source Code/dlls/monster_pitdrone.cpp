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
// NPC: Pit Drone * http://half-life.wikia.com/wiki/Pit_Drone
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"soundent.h"
#include	"game.h"
#include	"weapons.h"
#include	"proj_pitdronespit.h"
#include	"monster_pitdrone.h"
#include "CGib.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define PIT_DRONE_AE_SPIT_SOUND		( 1 ) //Shooting Sound
#define PIT_DRONE_AE_SPIT			( 2 ) //Shooting
#define PIT_DRONE_AE_RELOAD			( 3 ) //Reload
#define PIT_DRONE_AE_BITE			( 4 ) //Double Slash
#define PIT_DRONE_AE_SLASH_LEFT		( 5 ) //Slash Left
#define PIT_DRONE_AE_SLASH_RIGHT	( 6 ) //Slash Right

#define HORNGROUP					1
#define PITDRONE_HORNS0				0
#define PITDRONE_HORNS6				1
#define PITDRONE_HORNS5				2
#define PITDRONE_HORNS4				3
#define PITDRONE_HORNS3				4
#define PITDRONE_HORNS2				5
#define PITDRONE_HORNS1				6

#define	PITDRONE_SPRINT_DIST		255
#define PITDRONE_FLINCH_DELAY		1

//=========================================================
// monster-specific schedule types
//=========================================================
enum {
	SCHED_PDRONE_SMELLFOOD = LAST_COMMON_SCHEDULE + 1,
	SCHED_PDRONE_EAT,
	SCHED_PDRONE_SNIFF_AND_EAT,
	SCHED_PDRONE_WALLOW,
};

//=========================================================
// monster-specific tasks
//=========================================================
enum {
	TASK_PDRONE_SMELLFOOD = LAST_COMMON_SCHEDULE + 1,
};

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS(monster_pitdrone, CPitDrone);

TYPEDESCRIPTION	CPitDrone::m_SaveData[] = {
	DEFINE_FIELD(CPitDrone, m_flammo, FIELD_FLOAT),
	DEFINE_FIELD(CPitDrone, m_flhorns, FIELD_FLOAT),
	DEFINE_FIELD(CPitDrone, m_fCanThreatDisplay, FIELD_BOOLEAN),
	DEFINE_FIELD(CPitDrone, m_flNextSpitTime, FIELD_TIME),
	DEFINE_FIELD(CPitDrone, m_flNextSpeakTime, FIELD_TIME),
	DEFINE_FIELD(CPitDrone, m_flNextWordTime, FIELD_TIME),
	DEFINE_FIELD(CPitDrone, m_iLastWord, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CPitDrone, CBaseMonster);

//=========================================================
// Read options from bsp * put in .fgd file as option *
// [initammo] = 1-6 
//=========================================================
void CPitDrone::KeyValue(KeyValueData *pkvd) {
	if (FStrEq(pkvd->szKeyName, "initammo")) {
		m_flammo = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue(pkvd);
}

//=========================================================
// Monster Sounds
//=========================================================
const char *CPitDrone::pAttackHitStrikeSounds[] = {
	"zombie/claw_strike1.wav",
	"zombie/claw_strike2.wav",
	"zombie/claw_strike3.wav",
};

const char *CPitDrone::pAttackHitSounds[] = {
	"pitdrone/pit_drone_melee_attack1.wav",
	"pitdrone/pit_drone_melee_attack2.wav",
};

const char *CPitDrone::pAttackMissSounds[] = {
	"zombie/claw_miss1.wav",
	"zombie/claw_miss2.wav",
};

const char *CPitDrone::pAttackSoundsSpike[] = {
	"pitdrone/pit_drone_attack_spike1.wav",
	"pitdrone/pit_drone_attack_spike2.wav",
};

const char *CPitDrone::pIdleSounds[] = {
	"pitdrone/pit_drone_idle1.wav",
	"pitdrone/pit_drone_idle2.wav",
	"pitdrone/pit_drone_idle3.wav",

};

const char *CPitDrone::pAlertSounds[] = {
	"pitdrone/pit_drone_alert1.wav",
	"pitdrone/pit_drone_alert2.wav",
	"pitdrone/pit_drone_alert3.wav",
};

const char *CPitDrone::pPainSounds[] = {
	"pitdrone/pit_drone_pain1.wav",
	"pitdrone/pit_drone_pain2.wav",
	"pitdrone/pit_drone_pain3.wav",
	"pitdrone/pit_drone_pain4.wav",
};

const char *CPitDrone::pDieSounds[] = {
	"pitdrone/pit_drone_die1.wav",
	"pitdrone/pit_drone_die2.wav",
	"pitdrone/pit_drone_die3.wav",
};

//=========================================================
// Spawn
//=========================================================
void CPitDrone::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/pit_drone.mdl");

	UTIL_SetSize(pev, Vector(-24, -24, 0), Vector(24, 24, 64));

	SetSolidType(SOLID_SLIDEBOX);
	SetMoveType(MOVETYPE_STEP);
	m_bloodColor = BLOOD_COLOR_GREEN;
	pev->effects = 0;

	if (pev->health == 0)
		pev->health = gSkillData.pitdroneHealth;

	m_flFieldOfView = 0.2;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState = MONSTERSTATE_NONE;
	m_flDebug = false; //Debug Massages

	m_fCanThreatDisplay = TRUE;
	m_flNextSpitTime = UTIL_GlobalTimeBase();

	m_flhorns = m_flammo;
	MonsterInit();
	UpdateHorns();
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CPitDrone::Classify() {
	return m_iClass ? m_iClass : CLASS_ALIEN_MONSTER;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CPitDrone::Precache() {
	PRECACHE_MODEL("models/pit_drone.mdl");
	PRECACHE_MODEL("models/pit_drone_gibs.mdl");

	m_iSpitSprite = PRECACHE_MODEL("sprites/tinyspit.spr");

	PRECACHE_SOUND_ARRAY(pAttackHitSounds);
	PRECACHE_SOUND_ARRAY(pAttackHitStrikeSounds);
	PRECACHE_SOUND_ARRAY(pAttackMissSounds);
	PRECACHE_SOUND_ARRAY(pAttackSoundsSpike);
	PRECACHE_SOUND_ARRAY(pIdleSounds);
	PRECACHE_SOUND_ARRAY(pDieSounds);
	PRECACHE_SOUND_ARRAY(pPainSounds);
	PRECACHE_SOUND_ARRAY(pAlertSounds);

	PRECACHE_SOUND("pitdrone/pit_drone_communicate1.wav");
	PRECACHE_SOUND("pitdrone/pit_drone_communicate2.wav");
	PRECACHE_SOUND("pitdrone/pit_drone_communicate3.wav");
	PRECACHE_SOUND("pitdrone/pit_drone_communicate4.wav");

	PRECACHE_SOUND("pitdrone/pit_drone_eat.wav");
	PRECACHE_SOUND("pitdrone/pit_drone_hunt1.wav");
	PRECACHE_SOUND("pitdrone/pit_drone_hunt2.wav");
	PRECACHE_SOUND("pitdrone/pit_drone_hunt3.wav");
	PRECACHE_SOUND("pitdrone/pit_drone_reload.wav");

	PRECACHE_SOUND("zombie/claw_miss1.wav");
	PRECACHE_SOUND("zombie/claw_miss2.wav");

	UTIL_PrecacheOther("pitdronespit");
}

//=========================================================
// TakeDamage - overridden for Pit Drone so we can keep track
// of how much time has passed since it was last injured
//=========================================================
int CPitDrone::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) {
	// if the gonome is running, has an enemy, was hurt by the enemy, hasn't been hurt in the last 3 seconds, and isn't too close to the enemy,
	// it will swerve. (whew).
	if (m_hEnemy != NULL && IsMoving() && pevAttacker == m_hEnemy->pev && UTIL_GlobalTimeBase() - m_flLastHurtTime > 3) {
		float flDist = (pev->origin - m_hEnemy->pev->origin).Length2D();
		if (flDist > PITDRONE_SPRINT_DIST) {
			flDist = Vector(pev->origin - m_Route[m_iRouteIndex].vecLocation).Length2D();// reusing flDist. 

			Vector vecApex;
			if (FTriangulate(pev->origin, m_Route[m_iRouteIndex].vecLocation, flDist * 0.5, m_hEnemy, &vecApex)) {
				InsertWaypoint(vecApex, bits_MF_TO_DETOUR | bits_MF_DONT_SIMPLIFY);
			}
		}
	}

	if (!FClassnameIs(pevAttacker, "monster_babycrab")) {
		// don't forget about headcrabs if it was a headcrab that hurt the squid.
		m_flLastHurtTime = UTIL_GlobalTimeBase();
	}

	return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

//=========================================================
// TraceAttack - Damage based on Hitgroups
//=========================================================
void CPitDrone::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
		return;
	}

	if (pev->takedamage) {
		if (IsAlive() && RANDOM_LONG(0, 4) <= 2) { PainSound(); }
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
			flDamage = gSkillData.pitdroneHead*flDamage;
			break;
		case HITGROUP_CHEST:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_CHEST\n", GetClassname());
			flDamage = gSkillData.pitdroneChest*flDamage;
			break;
		case HITGROUP_STOMACH:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_STOMACH\n", GetClassname());
			flDamage = gSkillData.pitdroneStomach*flDamage;
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_ARM\n", GetClassname());
			flDamage = gSkillData.pitdroneArm*flDamage;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_LEG\n", GetClassname());
			flDamage = gSkillData.pitdroneLeg*flDamage;
			break;
		}
	}

	SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);// a little surface blood.
	TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
	CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}

//=========================================================
// IdleSound
//=========================================================
void CPitDrone::IdleSound() {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pIdleSounds);
}

//=========================================================
// PainSound
//=========================================================
void CPitDrone::PainSound() {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pPainSounds);
}

//=========================================================
// AlertSound
//=========================================================
void CPitDrone::AlertSound() {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pAlertSounds);
}
//=========================================================
// DeathSound
//=========================================================
void CPitDrone::DeathSound() {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pDieSounds);
}

//=========================================================
// AttackSound Hit
//=========================================================
void CPitDrone::AttackSound() {
	EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackSoundsSpike);
}

//=========================================================
// AttackSound
//=========================================================
void CPitDrone::AttackSoundSpike() {
	EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackSoundsSpike);
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CPitDrone::HandleAnimEvent(MonsterEvent_t *pEvent) {
	switch (pEvent->event) {
	case PIT_DRONE_AE_SLASH_LEFT: {
		if (m_flDebug)
			ALERT(at_console, "%s:HandleAnimEvent:Slash left!\n", GetClassname());

		CBaseEntity *pHurt = CheckTraceHullAttack(70, gSkillData.pitdroneDmgWhip, DMG_SLASH);
		if (pHurt) {
			if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT)) {
				pHurt->pev->punchangle.z = 20;
				pHurt->pev->punchangle.x = -20;
				pHurt->pev->velocity = pHurt->pev->velocity - gpGlobals->v_right * 100;
			}

			EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSounds);
		}
		else {
			if (RANDOM_LONG(0, 1)) { AttackSoundSpike(); }
			EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSounds);
		}
	}
								  break;
	case PIT_DRONE_AE_SLASH_RIGHT: {
		if (m_flDebug)
			ALERT(at_console, "%s:HandleAnimEvent:Slash right!\n", GetClassname());

		CBaseEntity *pHurt = CheckTraceHullAttack(70, gSkillData.pitdroneDmgWhip, DMG_SLASH);
		if (pHurt) {
			if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT)) {
				pHurt->pev->punchangle.z = -20;
				pHurt->pev->punchangle.x = 20;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * 100;
			}

			EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSounds);
		}
		else
			EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSounds);
	}
								   break;
	case PIT_DRONE_AE_BITE: {
		if (m_flDebug)
			ALERT(at_console, "%s:HandleAnimEvent:Slash both!\n", GetClassname());

		CBaseEntity *pHurt = CheckTraceHullAttack(70, gSkillData.pitdroneDmgBite, DMG_CLUB);
		if (pHurt) {
			if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT)) {
				pHurt->pev->punchangle.z = RANDOM_FLOAT(-25, 25);
				pHurt->pev->punchangle.x = 30;
				UTIL_ScreenShake(pHurt->pev->origin, 8.0, 1.5, 0.7, 2);
				if (pHurt->IsPlayer()) {
					UTIL_MakeVectors(pev->angles);
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * 50 + gpGlobals->v_up * 300;
					EMIT_SOUND_ARRAY_DYN(CHAN_BODY, pAttackHitStrikeSounds);
				}
			}

			EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pAttackHitSounds);
		}
		else {
			if (RANDOM_LONG(0, 1)) { AttackSoundSpike(); }
			EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSounds);
		}
	}
							break;
	case PIT_DRONE_AE_RELOAD: {
		if (m_flDebug)
			ALERT(at_console, "%s:HandleAnimEvent:Horns reload!\n", GetClassname());

		if (bits_COND_NO_AMMO_LOADED) {
			m_flhorns = m_flammo;
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "pitdrone/pit_drone_reload.wav", VOL_NORM, ATTN_IDLE, 0, 100);
			UpdateHorns();
			ClearConditions(bits_COND_NO_AMMO_LOADED);
		}
	}
							  break;
	case PIT_DRONE_AE_SPIT_SOUND: {
		if (m_flhorns) {
			AttackSoundSpike();
		}
	}
								  break;
	case PIT_DRONE_AE_SPIT: {
		if (m_flhorns) {
			Vector	vecSpitOffset;
			Vector  vecSpitDir;

			UTIL_MakeVectors(pev->angles);

			Vector vecArmPos, vecArmAng;
			GetAttachment(0, vecArmPos, vecArmAng);

			vecSpitOffset = (pev->origin + vecSpitOffset);
			vecSpitOffset = vecArmPos;
			vecSpitDir = ((m_hEnemy->pev->origin + m_hEnemy->pev->view_ofs) - vecSpitOffset).Normalize();

			vecSpitDir.x += RANDOM_FLOAT(-0.02, 0.02);
			vecSpitDir.y += RANDOM_FLOAT(-0.02, 0.02);
			vecSpitDir.z += RANDOM_FLOAT(-0.12, -0.08);
			vecSpitDir.z *= -1;

			// do stuff for this event.
			AttackSound();
			AttackSoundSpike();

			// spew the spittle temporary ents.
			vecArmPos = vecArmPos + vecSpitDir * 16;
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecArmPos);
			WRITE_BYTE(TE_SPRITE_SPRAY);
			WRITE_COORD(vecArmPos.x);	// pos
			WRITE_COORD(vecArmPos.y);
			WRITE_COORD(vecArmPos.z);
			WRITE_COORD(vecSpitDir.x);	// dir
			WRITE_COORD(vecSpitDir.y);
			WRITE_COORD(vecSpitDir.z + 0.30);
			WRITE_SHORT(m_iSpitSprite);	// model
			WRITE_BYTE(15);			// count
			WRITE_BYTE(180);			// speed
			WRITE_BYTE(25);			// noise ( client will divide by 100 )
			MESSAGE_END();

			CBaseEntity *pSpit = CBaseEntity::Create("pitdronespit", vecSpitOffset, UTIL_VecToAngles(vecSpitDir), edict());
			UTIL_MakeVectors(pSpit->pev->angles);
			pSpit->pev->velocity = gpGlobals->v_forward * 700;
			m_flhorns = (m_flhorns - 1);
			UpdateHorns();
		}
	}
							break;
	default:
		CBaseMonster::HandleAnimEvent(pEvent);
	}
}

//=========================================================
// IgnoreConditions 
//=========================================================
int CPitDrone::IgnoreConditions() {
	int iIgnore = CBaseMonster::IgnoreConditions();
	if (UTIL_GlobalTimeBase() - m_flLastHurtTime <= 20) {
		// haven't been hurt in 20 seconds, so let the squid care about stink. 
		// Er, more like, we HAVE been hurt in the last 20 seconds, so DON'T let it care about food. --LRC
		iIgnore = bits_COND_SMELL | bits_COND_SMELL_FOOD;
	}

	if (m_hEnemy != NULL) {
		if (FClassnameIs(m_hEnemy->pev, "monster_babycrab") || FClassnameIs(m_hEnemy->pev, "monster_rat")) {
			// (Unless after a tasty headcrab)
			// i.e. when chasing a headcrab, don't worry about other food. --LRC
			iIgnore = bits_COND_SMELL | bits_COND_SMELL_FOOD;
		}
	}

	if ((m_Activity == ACT_MELEE_ATTACK1)) {
		if (m_flNextFlinch >= UTIL_GlobalTimeBase())
			iIgnore |= (bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE);
	}

	if ((m_Activity == ACT_SMALL_FLINCH) || (m_Activity == ACT_BIG_FLINCH)) {
		if (m_flNextFlinch < UTIL_GlobalTimeBase())
			m_flNextFlinch = UTIL_GlobalTimeBase() + PITDRONE_FLINCH_DELAY;
	}

	return iIgnore;
}

//=========================================================
// StopTalking - won't speak again for 10-20 seconds.
//=========================================================
void CPitDrone::StopTalking() {
	m_flNextWordTime = m_flNextSpeakTime = UTIL_GlobalTimeBase() + 10 + RANDOM_LONG(0, 10);
}

//=========================================================
// IRelationship - overridden for gonome so that it can
// be made to ignore its love of headcrabs for a while.
//=========================================================
int CPitDrone::IRelationship(CBaseEntity *pTarget) {
	if (UTIL_GlobalTimeBase() - m_flLastHurtTime < 5 && FClassnameIs(pTarget->pev, "monster_babycrab")) {
		// if squid has been hurt in the last 5 seconds, and is getting relationship for a headcrab, 
		// tell squid to disregard crab. 
		return R_NO;
	}

	if (UTIL_GlobalTimeBase() - m_flLastHurtTime < 5 && FClassnameIs(pTarget->pev, "monster_rat")) {
		// if squid has been hurt in the last 5 seconds, and is getting relationship for a headcrab, 
		// tell squid to disregard crab. 
		return R_NO;
	}

	return CBaseMonster::IRelationship(pTarget);
}

//=========================================================
// CheckRangeAttack1
//=========================================================
bool CPitDrone::CheckRangeAttack1(float flDot, float flDist) {
	if (!m_flhorns || IsMoving() && flDist >= 512) {
		return false;
	}

	if (flDist > 64 && flDist <= 784 && flDot >= 0.5 && UTIL_GlobalTimeBase() >= m_flNextSpitTime) 
	{
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
// Change Bodygroup of horns when they are consumed
//=========================================================
void CPitDrone::UpdateHorns() {
	if (m_flDebug)
		ALERT(at_console, "BodyChange %f horns\n", m_flhorns);

	if (!m_flhorns) {
		SetConditions(bits_COND_NO_AMMO_LOADED);
		SetBodygroup(HORNGROUP, PITDRONE_HORNS0);
	}
	else if (m_flhorns == 1) {
		SetBodygroup(HORNGROUP, PITDRONE_HORNS1);
	}
	else if (m_flhorns == 2) {
		SetBodygroup(HORNGROUP, PITDRONE_HORNS2);
	}
	else if (m_flhorns == 3) {
		SetBodygroup(HORNGROUP, PITDRONE_HORNS3);
	}
	else if (m_flhorns == 4) {
		SetBodygroup(HORNGROUP, PITDRONE_HORNS4);
	}
	else if (m_flhorns == 5) {
		SetBodygroup(HORNGROUP, PITDRONE_HORNS5);
	}
	else if (m_flhorns == 6) {
		SetBodygroup(HORNGROUP, PITDRONE_HORNS6);
	}
}

//=========================================================
// ShouldSpeak - Should this PDrone be talking?
//=========================================================
bool CPitDrone::ShouldSpeak() {
	if (m_flNextSpeakTime > UTIL_GlobalTimeBase()) {
		// my time to talk is still in the future.
		return false;
	}

	if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_2) {
		if (m_MonsterState != MONSTERSTATE_COMBAT) {
			// if gagged, don't talk outside of combat.
			// if not going to talk because of this, put the talk time 
			// into the future a bit, so we don't talk immediately after 
			// going into combat
			m_flNextSpeakTime = UTIL_GlobalTimeBase() + 3;
			return false;
		}
	}

	return true;
}

//=========================================================
// RunAI
//=========================================================
void CPitDrone::RunAI() {
	// first, do base class stuff
	CBaseMonster::RunAI();

	if (m_hEnemy != NULL && m_Activity == ACT_RUN) {
		// chasing enemy. Sprint for last bit
		if ((pev->origin - m_hEnemy->pev->origin).Length2D() < PITDRONE_SPRINT_DIST) {
			pev->framerate = 1.25;
		}
	}
}

//========================================================
// AI Schedules Specific to this monster
//=========================================================

// primary range attack
Task_t	tlPDroneRangeAttack1[] = {
	{ TASK_STOP_MOVING, 0 },
	{ TASK_FACE_IDEAL, (float)0 },
	{ TASK_RANGE_ATTACK1, (float)0 },
	{ TASK_SET_ACTIVITY, (float)ACT_IDLE },
};

Schedule_t	slPDroneRangeAttack1[] = {
	{
		tlPDroneRangeAttack1,
		HL_ARRAYSIZE(tlPDroneRangeAttack1),
		bits_COND_NEW_ENEMY |
		bits_COND_ENEMY_DEAD |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_ENEMY_OCCLUDED |
		bits_COND_NO_AMMO_LOADED,
		0,
		"PDrone Range Attack1"
	}
};

// Chase enemy schedule
Task_t tlPDroneChaseEnemy1[] = {
	{ TASK_SET_FAIL_SCHEDULE, (float)SCHED_RANGE_ATTACK1 },// !!!OEM - this will stop nasty PitDrone oscillation.
	{ TASK_GET_PATH_TO_ENEMY, (float)0 },
	{ TASK_RUN_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
};

Schedule_t slPDroneChaseEnemy[] = {
	{
		tlPDroneChaseEnemy1,
		HL_ARRAYSIZE(tlPDroneChaseEnemy1),
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
		"PDrone Chase Enemy"
	}
};


// PitDrone walks to something tasty and eats it.
Task_t tlPDroneEat[] = {
	{ TASK_STOP_MOVING, (float)0 },
	{ TASK_EAT, (float)10 },// this is in case the PitDrone can't get to the food
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

Schedule_t slPDroneEat[] = {
	{
		tlPDroneEat,
		HL_ARRAYSIZE(tlPDroneEat),
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_NEW_ENEMY,
	// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
	// here or the monster won't detect these sounds at ALL while running this schedule.
	bits_SOUND_MEAT |
	bits_SOUND_CARCASS,
	"PDroneEat"
}
};

// this is a bit different than just Eat. We use this schedule when the food is far away, occluded, or behind
// the PitDrone. This schedule plays a sniff animation before going to the source of food.
Task_t tlPDroneSniffAndEat[] = {
	{ TASK_STOP_MOVING, (float)0 },
	{ TASK_EAT, (float)10 },// this is in case the PitDrone can't get to the food
	{ TASK_PLAY_SEQUENCE, (float)ACT_DETECT_SCENT },
	{ TASK_STORE_LASTPOSITION, (float)0 },
	{ TASK_GET_PATH_TO_BESTSCENT, (float)0 },
	{ TASK_RUN_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
	{ TASK_PLAY_SEQUENCE, (float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE, (float)ACT_EAT },
	{ TASK_PLAY_SEQUENCE, (float)ACT_EAT },
	{ TASK_EAT, (float)50 },
	{ TASK_GET_PATH_TO_LASTPOSITION, (float)0 },
	{ TASK_RUN_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
	{ TASK_CLEAR_LASTPOSITION, (float)0 },
};

Schedule_t slPDroneSniffAndEat[] = {
	{
		tlPDroneSniffAndEat,
		HL_ARRAYSIZE(tlPDroneSniffAndEat),
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_NEW_ENEMY,
	// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
	// here or the monster won't detect these sounds at ALL while running this schedule.
	bits_SOUND_MEAT |
	bits_SOUND_CARCASS,
	"PDroneSniffAndEat"
}
};

// PitDrone does this to stinky things. 
Task_t tlPDroneWallow[] = {
	{ TASK_STOP_MOVING, (float)0 },
	{ TASK_EAT, (float)10 },// this is in case the PitDrone can't get to the stinkiness
	{ TASK_STORE_LASTPOSITION, (float)0 },
	{ TASK_GET_PATH_TO_BESTSCENT, (float)0 },
	{ TASK_RUN_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
	{ TASK_PLAY_SEQUENCE, (float)ACT_INSPECT_FLOOR },
	{ TASK_EAT, (float)50 },// keeps PitDrone from eating or sniffing anything else for a while.
	{ TASK_GET_PATH_TO_LASTPOSITION, (float)0 },
	{ TASK_RUN_PATH, (float)0 },
	{ TASK_WAIT_FOR_MOVEMENT, (float)0 },
	{ TASK_CLEAR_LASTPOSITION, (float)0 },
};

Schedule_t slPDroneWallow[] = {
	{
		tlPDroneWallow,
		HL_ARRAYSIZE(tlPDroneWallow),
		bits_COND_LIGHT_DAMAGE |
		bits_COND_HEAVY_DAMAGE |
		bits_COND_NEW_ENEMY,
	// even though HEAR_SOUND/SMELL FOOD doesn't break this schedule, we need this mask
	// here or the monster won't detect these sounds at ALL while running this schedule.
	bits_SOUND_GARBAGE,
	"PDroneWallow"
}
};

DEFINE_CUSTOM_SCHEDULES(CPitDrone) {
	slPDroneRangeAttack1,
		slPDroneChaseEnemy,
		slPDroneEat,
		slPDroneSniffAndEat,
		slPDroneWallow
};

IMPLEMENT_CUSTOM_SCHEDULES(CPitDrone, CBaseMonster);

//=========================================================
// GetSchedule 
//=========================================================
Schedule_t *CPitDrone::GetSchedule() {
	switch (m_MonsterState) {
	case MONSTERSTATE_ALERT: {
		if (HasConditions(bits_COND_SMELL_FOOD)) {
			CSound *pSound;
			pSound = PBestScent();
			ASSERT(pSound != NULL);
			if (pSound && (!FInViewCone(&pSound->m_vecOrigin) || !FVisible(pSound->m_vecOrigin))) {
				// scent is behind or occluded
				return GetScheduleOfType(SCHED_PDRONE_SNIFF_AND_EAT);
			}

			// food is right out in the open. Just go get it.
			return GetScheduleOfType(SCHED_PDRONE_EAT);
		}

		if (HasConditions(bits_COND_SMELL)) {
			// there's something stinky. 
			CSound *pSound;
			pSound = PBestScent();
			ASSERT(pSound != NULL);
			if (pSound) {
				return GetScheduleOfType(SCHED_PDRONE_WALLOW);
			}
		}
	}
							 break;
	case MONSTERSTATE_COMBAT: {
		// dead enemy
		if (HasConditions(bits_COND_ENEMY_DEAD)) {
			// call base class, all code to handle dead enemies is centralized there.
			return CBaseMonster::GetSchedule();
		}

		if (HasConditions(bits_COND_NEW_ENEMY)) {
			if (m_fCanThreatDisplay && IRelationship(m_hEnemy) == R_HT) {
				return GetScheduleOfType(SCHED_WAKE_ANGRY);
			}
		}

		if (HasConditions(bits_COND_SMELL_FOOD)) {
			CSound *pSound;
			pSound = PBestScent();
			if (pSound && (!FInViewCone(&pSound->m_vecOrigin) || !FVisible(pSound->m_vecOrigin))) {
				// scent is behind or occluded
				return GetScheduleOfType(SCHED_PDRONE_SNIFF_AND_EAT);
			}

			// food is right out in the open. Just go get it.
			return GetScheduleOfType(SCHED_PDRONE_EAT);
		}

		if (m_flammo >= 1 && HasConditions(bits_COND_NO_AMMO_LOADED)) {
			return GetScheduleOfType(SCHED_RELOAD);
		}

		if (HasConditions(bits_COND_CAN_RANGE_ATTACK1)) {
			return GetScheduleOfType(SCHED_RANGE_ATTACK1);
		}

		if (HasConditions(bits_COND_CAN_MELEE_ATTACK1)) {
			return GetScheduleOfType(SCHED_MELEE_ATTACK1);
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
Schedule_t* CPitDrone::GetScheduleOfType(int Type) {
	switch (Type) {
	case SCHED_RANGE_ATTACK1:
		return &slPDroneRangeAttack1[0];
		break;
	case SCHED_PDRONE_EAT:
		return &slPDroneEat[0];
		break;
	case SCHED_PDRONE_SNIFF_AND_EAT:
		return &slPDroneSniffAndEat[0];
		break;
	case SCHED_PDRONE_WALLOW:
		return &slPDroneWallow[0];
		break;
	case SCHED_CHASE_ENEMY:
		return &slPDroneChaseEnemy[0];
		break;
	}

	return CBaseMonster::GetScheduleOfType(Type);
}

//=========================================================
// Start task - selects the correct activity and performs
// any necessary calculations to start the next task on the
// schedule.  OVERRIDDEN for PitDrone because it needs to
// know explicitly when the last attempt to chase the enemy
// failed, since that impacts its attack choices.
//=========================================================
void CPitDrone::StartTask(Task_t *pTask) {
	m_iTaskStatus = TASKSTATUS_RUNNING;
	switch (pTask->iTask) {
	case TASK_MELEE_ATTACK2: {
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
	default: {
		CBaseMonster::StartTask(pTask);
	}
			 break;
	}
}

//=========================================================
// RunTask
//=========================================================
void CPitDrone::RunTask(Task_t *pTask) {
	CBaseMonster::RunTask(pTask);
}

//=========================================================
// GetIdealState - Overridden for PDrone to deal with
// the feature that makes it lose interest in headcrabs for 
// a while if something injures it. 
//=========================================================
MONSTERSTATE CPitDrone::GetIdealState() {
	return CBaseMonster::GetIdealState();
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CPitDrone::SetYawSpeed() {
	int ys;
	switch (m_Activity) {
	case	ACT_WALK:			ys = 120;	break;
	case	ACT_RUN:			ys = 120;	break;
	case	ACT_IDLE:			ys = 120;	break;
	case	ACT_RANGE_ATTACK1:	ys = 120;	break;
	default:					ys = 120;   break;
	}

	pev->yaw_speed = ys;
}

//=========================================================
// GibMonster
//=========================================================
const GibData PitDroneGibs = { "models/pit_drone_gibs.mdl", 0, 7 };

void CPitDrone::GibMonster()
{
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/bodysplat.wav", 1, ATTN_NORM);
	CGib::SpawnRandomGibs(pev, 6, PitDroneGibs);	// Throw alien gibs

	// don't remove players!
	SetThink(&CBaseMonster::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}