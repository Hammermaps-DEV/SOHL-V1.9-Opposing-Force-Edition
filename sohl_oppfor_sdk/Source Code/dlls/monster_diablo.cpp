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
*	Base Source-Code written by Marc-Antoine Lortie (https://github.com/malortie).
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"soundent.h"
#include	"monster_diablo.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	DIABLO_AE_ATTACK_LEFT		0x01
#define	DIABLO_AE_ATTACK_BOTH		0x02
#define	DIABLO_AE_ATTACK_RIGHT		0x03

#define DIABLO_FLINCH_DELAY				2
#define	DIABLO_MELEE_ATTACK_RADIUS		70
#define	DIABLO_TOLERANCE_MELEE2_RANGE	85

//=========================================================
// CDiablo
//=========================================================
LINK_ENTITY_TO_CLASS(monster_diablo, CDiablo);

//=========================================================
// Monster Sounds
//=========================================================
const char *CDiablo::pAttackHitSoundsEXT[] = {
	"zombie/claw_strike1.wav",
	"zombie/claw_strike2.wav",
	"zombie/claw_strike3.wav",
};

const char *CDiablo::pAttackMissSoundsEXT[] = {
	"zombie/claw_miss1.wav",
	"zombie/claw_miss2.wav",
};

const char *CDiablo::pAttackHitSounds[] = {
	"diablo/diablo_claw1.wav",
	"diablo/diablo_claw2.wav",
	"diablo/diablo_claw3.wav"
};

const char *CDiablo::pAttackMissSounds[] = {
	"diablo/diablo_claw_miss1.wav",
	"diablo/diablo_claw_miss2.wav"
};

const char *CDiablo::pAttackSounds[] = {
	"diablo/diablo_attack1.wav",
	"diablo/diablo_attack2.wav"
};

const char *CDiablo::pIdleSounds[] = {
	"diablo/diablo_idle1.wav",
	"diablo/diablo_idle2.wav",
	"diablo/diablo_idle3.wav"
};

const char *CDiablo::pAlertSounds[] = {
	"diablo/diablo_alert10.wav",
	"diablo/diablo_alert20.wav",
	"diablo/diablo_alert30.wav"
};

const char *CDiablo::pPainSounds[] = {
	"diablo/diablo_pain1.wav",
	"diablo/diablo_pain2.wav"
};

//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards. In the base class implementation,
// monsters care about all sounds, but no scents.
//=========================================================
int CDiablo::ISoundMask(void) {
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
int	CDiablo::Classify(void) {
	return m_iClass ? m_iClass : CLASS_ALIEN_PREDATOR;
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CDiablo::SetYawSpeed(void) {
	switch (m_Activity) {
		default:					pev->yaw_speed = 140;   break;
	}
}

//=========================================================
// TakeDamage
//=========================================================
int CDiablo::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) {
	// Take 30% damage from bullets
	if (bitsDamageType == DMG_BULLET) {
		Vector vecDir = pev->origin - (pevInflictor->absmin + pevInflictor->absmax) * 0.5;
		vecDir = vecDir.Normalize();
		float flForce = DamageForce(flDamage);
		pev->velocity = pev->velocity + vecDir * flForce;
		flDamage *= 0.3;
	}

	return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

//=========================================================
// PainSound
//=========================================================
void CDiablo::PainSound(void) {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pPainSounds);
}

//=========================================================
// AlertSound
//=========================================================
void CDiablo::AlertSound(void) {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pAlertSounds);
}

//=========================================================
// IdleSound
//=========================================================
void CDiablo::IdleSound(void) {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pIdleSounds);
}

//=========================================================
// AttackSound 
//=========================================================
void CDiablo::AttackSound(void) {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pAttackSounds);
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CDiablo::HandleAnimEvent(MonsterEvent_t *pEvent) {
	switch (pEvent->event) {
		case DIABLO_AE_ATTACK_RIGHT: {
			CBaseEntity *pHurt = CheckTraceHullAttack(DIABLO_MELEE_ATTACK_RADIUS, /*gSkillData.diabloDmgOneSlash*/ 1, DMG_SLASH | DMG_NEVERGIB);
			if (pHurt) {
				if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT)) {
					pHurt->pev->punchangle.z = -20;
					pHurt->pev->punchangle.x = 15;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * -200;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 100;
				}

				if (RANDOM_LONG(0, 1))
					EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSounds);

				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSoundsEXT);
			} else {
				if (RANDOM_LONG(0, 1))
					EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSounds);

				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSoundsEXT);
			}

			if (RANDOM_LONG(0, 1)) { AttackSound(); }
		}
		break;
		case DIABLO_AE_ATTACK_LEFT: {
			CBaseEntity *pHurt = CheckTraceHullAttack(DIABLO_MELEE_ATTACK_RADIUS, /*gSkillData.diabloDmgOneSlash*/ 1, DMG_SLASH | DMG_NEVERGIB);
			if (pHurt) {
				if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT)) {
					pHurt->pev->punchangle.z = 20;
					pHurt->pev->punchangle.x = 15;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * 200;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 100;
				}
				if (RANDOM_LONG(0, 1))
					EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSounds);

				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSoundsEXT);
			} else {
				if (RANDOM_LONG(0, 1))
					EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSounds);

				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSoundsEXT);
			}
		}
		break;
		case DIABLO_AE_ATTACK_BOTH: {
			CBaseEntity *pHurt = CheckTraceHullAttack(DIABLO_TOLERANCE_MELEE2_RANGE, /*gSkillData.diabloDmgBothSlash*/ 1, DMG_SLASH | DMG_NEVERGIB);
			if (pHurt) {
				pHurt->pev->punchangle.z = RANDOM_LONG(-10, 10);
				pHurt->pev->punchangle.x = RANDOM_LONG(-20, -30);
				pHurt->pev->velocity = pHurt->pev->velocity - gpGlobals->v_forward * -100;
				pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 70;
				if (RANDOM_LONG(0, 1))
					EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSounds);

				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackHitSoundsEXT);
			} else {
				if (RANDOM_LONG(0, 1))
					EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSounds);

				EMIT_SOUND_ARRAY_DYN(CHAN_WEAPON, pAttackMissSoundsEXT);
			}

			if (RANDOM_LONG(0, 1)) { AttackSound(); }
		}
		break;
		default:
			CBaseMonster::HandleAnimEvent(pEvent);
		break;
	}
}

//=========================================================
// Spawn
//=========================================================
void CDiablo::Spawn(void) {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/diablo.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_YELLOW;

	if (pev->health == 0)
		pev->health = 50;//gSkillData.diabloHealth;

	pev->view_ofs = VEC_VIEW;// position of the eyes relative to monster's origin.
	m_flFieldOfView = 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )

	m_MonsterState = MONSTERSTATE_NONE;
	m_afCapability = bits_CAP_DOORS_GROUP;

	MonsterInit();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CDiablo::Precache(void) {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/diablo.mdl");

	PRECACHE_SOUND_ARRAY(pAttackHitSounds);
	PRECACHE_SOUND_ARRAY(pAttackMissSounds);
	PRECACHE_SOUND_ARRAY(pAttackHitSoundsEXT);
	PRECACHE_SOUND_ARRAY(pAttackMissSoundsEXT);
	PRECACHE_SOUND_ARRAY(pAttackSounds);
	PRECACHE_SOUND_ARRAY(pIdleSounds);
	PRECACHE_SOUND_ARRAY(pAlertSounds);
	PRECACHE_SOUND_ARRAY(pPainSounds);
}

//=========================================================
// SetActivity
//=========================================================
void CDiablo::SetActivity(Activity newActivity) {
	if (m_MonsterState == MONSTERSTATE_COMBAT) {
		if (newActivity == ACT_WALK)
			newActivity = ACT_RUN;
	}

	CBaseMonster::SetActivity(newActivity);
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
int CDiablo::IgnoreConditions(void) {
	int iIgnore = CBaseMonster::IgnoreConditions();

	if ((m_Activity == ACT_MELEE_ATTACK1) || (m_Activity == ACT_MELEE_ATTACK1)) {
		if (m_flNextFlinch >= UTIL_GlobalTimeBase())
			iIgnore |= (bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE);
	}

	if ((m_Activity == ACT_SMALL_FLINCH) || (m_Activity == ACT_BIG_FLINCH)) {
		if (m_flNextFlinch < UTIL_GlobalTimeBase())
			m_flNextFlinch = UTIL_GlobalTimeBase() + DIABLO_FLINCH_DELAY;
	}

	return iIgnore;

}