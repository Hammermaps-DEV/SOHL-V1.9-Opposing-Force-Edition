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
// NPC: Otis * http://half-life.wikia.com/wiki/Otis_Laurey
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "talkmonster.h"
#include "schedule.h"
#include "defaultai.h"
#include "scripted.h"
#include "weapons.h"
#include "soundent.h"
#include "monster_otis.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	OTIS_AE_DRAW		( 2 )
#define	OTIS_AE_SHOOT		( 3 )
#define	OTIS_AE_HOLSTER		( 4 )

#define	NUM_OTIS_HEADS		4

#define	GUN_GROUP			1
#define	HEAD_GROUP			2

#define	HEAD_HAIR			0
#define	HEAD_BALD			1
#define	HEAD_HELMET			2
#define	HEAD_JOE			3

#define	GUN_NONE			0
#define	GUN_EAGLE			1
#define	GUN_NO_GUN			2
#define	GUN_DONUT			3

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS(monster_otis, COtis);

TYPEDESCRIPTION	COtis::m_SaveData[] = {
	DEFINE_FIELD(COtis, head, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(COtis, CBarney);

//=========================================================
// Monster Sounds
//=========================================================
const char *COtis::pAttackSounds[] = {
	"barney/ba_attack2.wav",
	"weapons/357_shot1.wav",
	"weapons/357_shot2.wav"
};

//=========================================================
// KeyValue
//=========================================================
void COtis::KeyValue(KeyValueData *pkvd) {
	if (FStrEq(pkvd->szKeyName, "head")) {
		head = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	} else
		CBarney::KeyValue(pkvd);
}

//=========================================================
// Spawn Barney
//=========================================================
void COtis::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/otis.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_RED;

	if (pev->health == 0) //LRC
		pev->health = gSkillData.otisHealth;

	pev->view_ofs = Vector(0, 0, 50);// position of the eyes relative to monster's origin.
	m_flFieldOfView = VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_MonsterState = MONSTERSTATE_NONE;

	m_iBaseBody = pev->body; //LRC
	SetBodygroup(GUN_GROUP, GUN_NONE);

	m_fGunDrawn = FALSE;

	m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;

	// Make sure hands are white.
	if (m_iBaseBody) {
		SetBodygroup(HEAD_GROUP, m_iBaseBody);
	} else {
		SetBodygroup(HEAD_GROUP, RANDOM_LONG(0, NUM_OTIS_HEADS - 1));
	}

	if (head != -1 && !m_iBaseBody) {
		SetBodygroup(HEAD_GROUP, head);
	}

	m_flDebug = false; //Debug Massages

	m_flHitgroupHead    = gSkillData.otisHead;
	m_flHitgroupChest   = gSkillData.otisChest;
	m_flHitgroupStomach = gSkillData.otisStomach;
	m_flHitgroupArm     = gSkillData.otisArm;
	m_flHitgroupLeg     = gSkillData.otisLeg;

	MonsterInit();
	SetUse(&COtis::FollowerUse);
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void COtis::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/otis.mdl");

	PRECACHE_SOUND("weapons/desert_eagle_fire.wav");

	TalkInit();
	CBarney::Precache();
}

//=========================================================
// TakeDamage
//=========================================================
int COtis::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) {
	if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_64) {
		if (m_flDebug)
			ALERT(at_console, "%s:TakeDamage:SF_MONSTER_SPAWNFLAG_64\n", STRING(pev->classname));

		CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
		if (pEnt->IsPlayer()) {
			pev->health = pev->max_health / 2;
			if (flDamage > 0) //Override all damage
				SetConditions(bits_COND_LIGHT_DAMAGE);

			if (flDamage >= 20) //Override all damage
				SetConditions(bits_COND_HEAVY_DAMAGE);

			return CTalkMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
		}

		if (pevAttacker->owner) {
			pEnt = CBaseEntity::Instance(pevAttacker->owner);
			if (pEnt->IsPlayer()) {
				pev->health = pev->max_health / 2;
				if (flDamage > 0) //Override all damage
					SetConditions(bits_COND_LIGHT_DAMAGE);

				if (flDamage >= 20) //Override all damage
					SetConditions(bits_COND_HEAVY_DAMAGE);

				return CTalkMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
			}
		}
	}

	if (!IsAlive() || pev->deadflag == DEAD_DYING || m_iPlayerReact) {
		return CTalkMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	}

	if (m_MonsterState != MONSTERSTATE_PRONE && (pevAttacker->flags & FL_CLIENT)) {
		m_flPlayerDamage += flDamage;
		if (m_hEnemy == NULL) {
			if ((m_afMemory & bits_MEMORY_SUSPICIOUS) || UTIL_IsFacing(pevAttacker, pev->origin)) {
				if (m_iszSpeakAs) {
					char szBuf[32];
					strcpy(szBuf, STRING(m_iszSpeakAs));
					strcat(szBuf, "_MAD");
					PlaySentence(szBuf, 4, VOL_NORM, ATTN_NORM);
				} else {
					PlaySentence("OT_MAD", 4, VOL_NORM, ATTN_NORM);
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
					PlaySentence("OT_SHOT", 4, VOL_NORM, ATTN_NORM);
				}
				Remember(bits_MEMORY_SUSPICIOUS);
			}
		}
		else if (!(m_hEnemy->IsPlayer()) && pev->deadflag == DEAD_NO) {
			if (m_iszSpeakAs) {
				char szBuf[32];
				strcpy(szBuf, STRING(m_iszSpeakAs));
				strcat(szBuf, "_SHOT");
				PlaySentence(szBuf, 4, VOL_NORM, ATTN_NORM);
			} else {
				PlaySentence("OT_SHOT", 4, VOL_NORM, ATTN_NORM);
			}
		}
	}

	return CTalkMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

//=========================================================
// TraceAttack - Damage based on Hitgroups
//=========================================================
void COtis::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CTalkMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
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
			case HITGROUP_HEAD_HELMET_BN:
			case HITGROUP_HEAD:
				if (m_flDebug)
					ALERT(at_console, "%s:TraceAttack:HITGROUP_HEAD\n", STRING(pev->classname));
				if ((GetBodygroup(HEAD_GROUP) == HEAD_HELMET) && bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_CLUB)) {
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
		}
	}

	SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);// a little surface blood.
	TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
	CTalkMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}

//=========================================================
// AlertSound
//=========================================================
void COtis::AlertSound(void) {
	if (m_hEnemy != NULL) {
		if (FOkToSpeak()) {
			if (m_iszSpeakAs) {
				char szBuf[32];
				strcpy(szBuf, STRING(m_iszSpeakAs));
				strcat(szBuf, "_ATTACK");
				PlaySentence(szBuf, RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE);
			} else {
				PlaySentence("OT_ATTACK", RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE);
			}
		}
	}
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void COtis::HandleAnimEvent(MonsterEvent_t *pEvent) {
	switch (pEvent->event) {
		case OTIS_AE_SHOOT:
			FirePistol();
		break;
		case OTIS_AE_DRAW:
			SetBodygroup(GUN_GROUP, GUN_EAGLE);
			m_fGunDrawn = TRUE;
		break;
		case OTIS_AE_HOLSTER:
			SetBodygroup(GUN_GROUP, GUN_NONE);
			m_fGunDrawn = FALSE;
		break;
		default:
			CTalkMonster::HandleAnimEvent(pEvent);
		break;
	}
}

//=========================================================
// BarneyFirePistol - shoots one round from the pistol at
// the enemy otis is facing.
//=========================================================
void COtis::FirePistol(void) {
	UTIL_MakeVectors(pev->angles);
	Vector vecShootOrigin = pev->origin + Vector(0, 0, 55);
	Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

	Vector angDir = UTIL_VecToAngles(vecShootDir);
	SetBlending(0, angDir.x);
	pev->effects = EF_MUZZLEFLASH;

	int pitchShift = RANDOM_LONG(0, 20);
	if (pitchShift > 10)
		pitchShift = 0;
	else
		pitchShift -= 5;

	if (pev->frags) {
		FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_2DEGREES, 1024, BULLET_MONSTER_9MM);
		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "barney/ba_attack2.wav", VOL_NORM, ATTN_NORM, 0, 100 + pitchShift);
	} else {
		FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_2DEGREES, 1024, BULLET_MONSTER_357);
		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/desert_eagle_fire.wav", VOL_NORM, ATTN_NORM, 0, 100 + pitchShift);
	}

	Vector vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40, 90) + gpGlobals->v_up * RANDOM_FLOAT(75, 100) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
	EjectBrass(vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL);
	WeaponFlash(vecShootOrigin);

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);
	m_cAmmoLoaded--;
}

//=========================================================
// Init talk data
//=========================================================
void COtis::TalkInit() {
	CTalkMonster::TalkInit();

	if (!m_iszSpeakAs) {
		m_szGrp[TLK_ANSWER] = "OT_ANSWER";
		m_szGrp[TLK_QUESTION] = "OT_QUESTION";
		m_szGrp[TLK_IDLE] = "OT_IDLE";
		m_szGrp[TLK_STARE] = "OT_STARE";

		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_256) //LRC
			m_szGrp[TLK_USE] = "OT_PFOLLOW";
		else
			m_szGrp[TLK_USE] = "OT_OK";

		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_256)
			m_szGrp[TLK_UNUSE] = "OT_PWAIT";
		else
			m_szGrp[TLK_UNUSE] = "OT_WAIT";

		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_256)
			m_szGrp[TLK_DECLINE] = "OT_POK";
		else
			m_szGrp[TLK_DECLINE] = "OT_NOTOK";

		m_szGrp[TLK_STOP] = "OT_STOP";

		m_szGrp[TLK_NOSHOOT] = "OT_SCARED";
		m_szGrp[TLK_HELLO] = "OT_HELLO";

		m_szGrp[TLK_PLHURT1] = "!OT_CUREA";
		m_szGrp[TLK_PLHURT2] = "!OT_CUREB";
		m_szGrp[TLK_PLHURT3] = "!OT_CUREC";

		m_szGrp[TLK_PHELLO] = NULL;
		m_szGrp[TLK_PIDLE] = NULL;
		m_szGrp[TLK_PQUESTION] = NULL;

		m_szGrp[TLK_SMELL] = "OT_SMELL";
		m_szGrp[TLK_WOUND] = "OT_WOUND";
		m_szGrp[TLK_MORTAL] = "OT_MORTAL";
	}

	m_voicePitch = (95 + RANDOM_LONG(0, 10));
}

//=========================================================
// Monster is Killed, change body and drop weapon
//=========================================================
void COtis::Killed(entvars_t *pevAttacker, int iGib) {
	if (GetBodygroup(GUN_GROUP) == GUN_EAGLE && !(pev->spawnflags & SF_MONSTER_SPAWNFLAG_1024)) {
		Vector vecGunPos, vecGunAngles;
		SetBodygroup(GUN_GROUP, GUN_NO_GUN);
		GetAttachment(0, vecGunPos, vecGunAngles);
		CBaseEntity *pGun = DropItem((pev->frags ? "weapon_9mmhandgun" : "weapon_eagle"), vecGunPos, vecGunAngles);
	}

	SetUse(NULL);
	CBarney::Killed(pevAttacker, iGib);
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *COtis::GetSchedule(void) {
	if (HasConditions(bits_COND_HEAR_SOUND)) {
		CSound *pSound;
		pSound = PBestSound();
		ASSERT(pSound != NULL);
		if (pSound && (pSound->m_iType & bits_SOUND_DANGER))
			return GetScheduleOfType(SCHED_TAKE_COVER_FROM_BEST_SOUND);
	}

	if (HasConditions(bits_COND_ENEMY_DEAD) && FOkToSpeak()) {
		if (m_iszSpeakAs) {
			char szBuf[32];
			strcpy(szBuf, STRING(m_iszSpeakAs));
			strcat(szBuf, "_KILL");
			PlaySentence(szBuf, 4, VOL_NORM, ATTN_NORM);
		}
		else {
			PlaySentence("OT_KILL", 4, VOL_NORM, ATTN_NORM);
		}
	}

	switch (m_MonsterState) {
	case MONSTERSTATE_COMBAT:
		if (HasConditions(bits_COND_ENEMY_DEAD)) {
			return CBaseMonster::GetSchedule();
		}

		if (HasConditions(bits_COND_NEW_ENEMY) && HasConditions(bits_COND_LIGHT_DAMAGE))
			return GetScheduleOfType(SCHED_SMALL_FLINCH);

		if (!m_fGunDrawn)
			return GetScheduleOfType(SCHED_ARM_WEAPON);

		if (HasConditions(bits_COND_HEAVY_DAMAGE))
			return GetScheduleOfType(SCHED_TAKE_COVER_FROM_ENEMY);
		break;
	case MONSTERSTATE_ALERT:
	case MONSTERSTATE_IDLE:
		if (HasConditions(bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE)) {
			return GetScheduleOfType(SCHED_SMALL_FLINCH);
		}

		if (m_hEnemy == NULL && IsFollowing()) {
			if (!m_hTargetEnt->IsAlive()) {
				StopFollowing(FALSE);
				break;
			}
			else {
				if (HasConditions(bits_COND_CLIENT_PUSH)) {
					return GetScheduleOfType(SCHED_MOVE_AWAY_FOLLOW);
				}

				return GetScheduleOfType(SCHED_TARGET_FACE);
			}
		}

		if (HasConditions(bits_COND_CLIENT_PUSH)) {
			return GetScheduleOfType(SCHED_MOVE_AWAY);
		}

		TrySmellTalk();
		break;
	}

	return CTalkMonster::GetSchedule();
}

//=========================================================
// Decline Following from Monster
//=========================================================
void COtis::DeclineFollowing(void)
{
	PlaySentence(m_szGrp[TLK_DECLINE], 2, VOL_NORM, ATTN_NORM); //LRC
	//PlaySentence("OT_POK", 2, VOL_NORM, ATTN_NORM);
}