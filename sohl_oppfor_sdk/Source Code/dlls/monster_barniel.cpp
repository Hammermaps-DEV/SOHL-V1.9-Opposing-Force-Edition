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
// NPC: Female Barney * Barniel * http://half-life.wikia.com/wiki/Barney_Calhoun
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00002 / Date: 01.02.2016
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"weapons.h"
#include	"soundent.h"
#include	"monster_barniel.h"

//=========================================================
// Monster's link to Class
//=========================================================
LINK_ENTITY_TO_CLASS(monster_barniel, CBarniel);

//=========================================================
// Monster Sounds
//=========================================================
const char *CBarniel::pPainSounds[] = {
	"barniel/bn_pain1.wav",
};

const char *CBarniel::pDeathSounds[] = {
	"barniel/bn_die1.wav",
};

const char *CBarniel::pAttackSounds[] = {
	"barniel/bn_attack1.wav",
	"barniel/bn_attack2.wav",
};

//=========================================================
// Spawn
//=========================================================
void CBarniel::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/barniel.mdl");

	if (pev->health == 0) //LRC
		pev->health = gSkillData.barnielHealth;

	m_flHitgroupHead = gSkillData.barnielHead;
	m_flHitgroupChest = gSkillData.barnielChest;
	m_flHitgroupStomach = gSkillData.barnielStomach;
	m_flHitgroupArm = gSkillData.barnielArm;
	m_flHitgroupLeg = gSkillData.barnielLeg;

	m_flDebug = false; //Debug Massages

	CBarney::Spawn();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CBarniel::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/barniel.mdl");

	m_iBrassShell = PRECACHE_MODEL("models/shell.mdl");// brass shell

	PRECACHE_SOUND_ARRAY(pAttackSounds);
	PRECACHE_SOUND_ARRAY(pPainSounds);
	PRECACHE_SOUND_ARRAY(pDeathSounds);

	CBarney::Precache();
}

//=========================================================
// TakeDamage
//=========================================================
int CBarniel::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) {
	int takedamage = CTalkMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
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

			return takedamage;
		}

		if (pevAttacker->owner) {
			pEnt = CBaseEntity::Instance(pevAttacker->owner);
			if (pEnt->IsPlayer()) {
				pev->health = pev->max_health / 2;
				if (flDamage > 0) //Override all damage
					SetConditions(bits_COND_LIGHT_DAMAGE);

				if (flDamage >= 20) //Override all damage
					SetConditions(bits_COND_HEAVY_DAMAGE);

				return takedamage;
			}
		}
	}

	if (!IsAlive() || pev->deadflag == DEAD_DYING || m_iPlayerReact) {
		return takedamage;
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
				}
				else {
					PlaySentence("BN_MAD", 4, VOL_NORM, ATTN_NORM);
				}

				Remember(bits_MEMORY_PROVOKED);
				StopFollowing(TRUE);
			}
			else {
				if (m_iszSpeakAs) {
					char szBuf[32];
					strcpy(szBuf, STRING(m_iszSpeakAs));
					strcat(szBuf, "_SHOT");
					PlaySentence(szBuf, 4, VOL_NORM, ATTN_NORM);
				}
				else {
					PlaySentence("BN_SHOT", 4, VOL_NORM, ATTN_NORM);
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
			}
			else {
				PlaySentence("BN_SHOT", 4, VOL_NORM, ATTN_NORM);
			}
		}
	}

	return takedamage;
}

//=========================================================
// TraceAttack - Damage based on Hitgroups
//=========================================================
void CBarniel::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CBarney::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
		return;
	}

	if (pev->takedamage) {
		if (IsAlive() && RANDOM_LONG(0, 4) <= 2) { PainSound(); }
		if (pev->spawnflags & SF_MONSTER_INVINCIBLE) {
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
					ALERT(at_console, "%s:TraceAttack:HITGROUP_HEAD\n", STRING(pev->classname));
				flDamage = m_flHitgroupHead*flDamage;
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
void CBarniel::AlertSound(void) {
	if (m_hEnemy != NULL) {
		if (FOkToSpeak()) {
			if (m_iszSpeakAs) {
				char szBuf[32];
				strcpy(szBuf, STRING(m_iszSpeakAs));
				strcat(szBuf, "_ATTACK");
				PlaySentence(szBuf, RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE);
			}
			else {
				PlaySentence("BN_ATTACK", RANDOM_FLOAT(2.8, 3.2), VOL_NORM, ATTN_IDLE);
			}
		}
	}
}

//=========================================================
// PainSound
//=========================================================
void CBarniel::PainSound(void) {
	if (gpGlobals->time < m_painTime)
		return;

	m_painTime = gpGlobals->time + RANDOM_FLOAT(0.5, 0.75);
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pPainSounds);
}

//=========================================================
// DeathSound 
//=========================================================
void CBarniel::DeathSound(void) {
	EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pDeathSounds);
}

//=========================================================
// Barney shoots one round from the pistol at 9mm
//=========================================================
void CBarniel::Fire9mmPistol(void) {
	Vector vecShootOrigin;

	UTIL_MakeVectors(pev->angles);
	vecShootOrigin = pev->origin + Vector(0, 0, 55);
	Vector vecShootDir = ShootAtEnemy(vecShootOrigin);

	Vector angDir = UTIL_VecToAngles(vecShootDir);
	SetBlending(0, angDir.x);
	pev->effects = EF_MUZZLEFLASH;

	int pitchShift = RANDOM_LONG(0, 20);
	if (pitchShift > 10)
		pitchShift = 0;
	else
		pitchShift -= 5;

	Vector vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40, 90) + gpGlobals->v_up * RANDOM_FLOAT(75, 100) + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40);
	EjectBrass(vecShootOrigin - vecShootDir * 24, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL);
	FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_2DEGREES, 1024, BULLET_MONSTER_9MM);
	EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "barniel/bn_attack2.wav", 1, ATTN_NORM, 0, 100 + pitchShift);
	WeaponFlash(vecShootOrigin);

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 384, 0.3);
	m_cAmmoLoaded--;
}

//=========================================================
// Init talk data
//=========================================================
void CBarniel::TalkInit() {
	CBarney::TalkInit();

	if (!m_iszSpeakAs) {
		m_szGrp[TLK_ANSWER] = "BN_ANSWER";
		m_szGrp[TLK_QUESTION] = "BN_QUESTION";
		m_szGrp[TLK_IDLE] = "BN_IDLE";
		m_szGrp[TLK_STARE] = "BN_STARE";

		if (pev->spawnflags & SF_MONSTER_PREDISASTER) //LRC
			m_szGrp[TLK_USE] = "BN_PFOLLOW";
		else
			m_szGrp[TLK_USE] = "BN_OK";

		if (pev->spawnflags & SF_MONSTER_PREDISASTER)
			m_szGrp[TLK_UNUSE] = "BN_PWAIT";
		else
			m_szGrp[TLK_UNUSE] = "BN_WAIT";

		if (pev->spawnflags & SF_MONSTER_PREDISASTER)
			m_szGrp[TLK_DECLINE] = "BN_POK";
		else
			m_szGrp[TLK_DECLINE] = "BN_NOTOK";

		m_szGrp[TLK_STOP] = "BN_STOP";
		m_szGrp[TLK_NOSHOOT] = "BN_SCARED";
		m_szGrp[TLK_HELLO] = "BN_HELLO";
		m_szGrp[TLK_PLHURT1] = "!BN_CUREA";
		m_szGrp[TLK_PLHURT2] = "!BN_CUREB";
		m_szGrp[TLK_PLHURT3] = "!BN_CUREC";
		m_szGrp[TLK_PHELLO] = NULL;
		m_szGrp[TLK_PIDLE] = NULL;
		m_szGrp[TLK_PQUESTION] = "BN_PQUEST";
		m_szGrp[TLK_SMELL] = "BN_SMELL";
		m_szGrp[TLK_WOUND] = "BN_WOUND";
		m_szGrp[TLK_MORTAL] = "BN_MORTAL";
	}

	m_voicePitch = (95 + RANDOM_LONG(0, 10));
}

//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *CBarniel::GetSchedule(void) {
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
			PlaySentence("BN_KILL", 4, VOL_NORM, ATTN_NORM);
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

	return CBarney::GetSchedule();
}