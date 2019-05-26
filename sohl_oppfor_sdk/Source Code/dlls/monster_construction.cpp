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
// NPC: Construction * Gus
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"talkmonster.h"
#include	"schedule.h"
#include	"defaultai.h"
#include	"scripted.h"
#include	"animation.h"
#include	"soundent.h"
#include	"weapons.h"
#include	"monster_scientist.h"
#include	"monster_construction.h"

//=======================================================
// Construction
//=======================================================
LINK_ENTITY_TO_CLASS(monster_construction, CConstruction);
LINK_ENTITY_TO_CLASS(monster_gus, CConstruction);

//=======================================================
// Custom Schedules
//=======================================================
extern Schedule_t *slFollow, *slFaceTarget, *slIdleSciStand, *slFear, *slScientistCover, *slScientistHide,
*slScientistStartle, *slStopFollowing, *slSciPanic, *slFollowScared, *slFaceTargetScared;
DEFINE_CUSTOM_SCHEDULES(CConstruction) {
	slFollow,
		slFaceTarget,
		slIdleSciStand,
		slFear,
		slScientistCover,
		slScientistHide,
		slScientistStartle,
		slStopFollowing,
		slSciPanic,
		slFollowScared,
		slFaceTargetScared,
};

IMPLEMENT_CUSTOM_SCHEDULES(CConstruction, CTalkMonster);

//=========================================================
// Spawn
//=========================================================
void CConstruction::Spawn() {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/construction.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	SetSolidType(SOLID_SLIDEBOX);
	SetMoveType(MOVETYPE_STEP);
	m_bloodColor = BLOOD_COLOR_RED;

	if (pev->health == 0)
		pev->health = gSkillData.constructionHealth;

	m_flFieldOfView = VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so scientists will notice player and say hello
	m_MonsterState = MONSTERSTATE_NONE;

	m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_OPEN_DOORS | bits_CAP_AUTO_DOORS | bits_CAP_USE;

	m_flDebug = false; //Debug Massages

	m_flHitgroupHead = gSkillData.constructionHead;
	m_flHitgroupChest = gSkillData.constructionChest;
	m_flHitgroupStomach = gSkillData.constructionStomach;
	m_flHitgroupArm = gSkillData.constructionArm;
	m_flHitgroupLeg = gSkillData.constructionLeg;

	MonsterInit();
	SetUse(&CConstruction::FollowerUse);
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CConstruction::Precache() {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/construction.mdl");

	PRECACHE_SOUND("scientist/sci_pain1.wav");
	PRECACHE_SOUND("scientist/sci_pain2.wav");
	PRECACHE_SOUND("scientist/sci_pain3.wav");
	PRECACHE_SOUND("scientist/sci_pain4.wav");
	PRECACHE_SOUND("scientist/sci_pain5.wav");

	TalkInit();
	CScientist::Precache();
}

//=========================================================
// StartTask
//=========================================================
void CConstruction::StartTask(Task_t *pTask) {
	switch (pTask->iTask) {
	case TASK_SCREAM:
		Scream();
		TaskComplete();
		break;
	case TASK_RANDOM_SCREAM:
		if (RANDOM_FLOAT(0, 1) < pTask->flData)
			Scream();

		TaskComplete();
		break;
	case TASK_SAY_FEAR:
		if (FOkToSpeak()) {
			Talk(2);
			m_hTalkTarget = m_hEnemy;
			if (m_hEnemy->IsPlayer())
				PlaySentence("SC_PLFEAR", 5, VOL_NORM, ATTN_NORM);
			else
				PlaySentence("SC_FEAR", 5, VOL_NORM, ATTN_NORM);
		}
		TaskComplete();
		break;
	case TASK_RUN_PATH_SCARED:
		m_movementActivity = ACT_RUN_SCARED;
		break;
	case TASK_MOVE_TO_TARGET_RANGE_SCARED:
		if ((m_hTargetEnt->pev->origin - pev->origin).Length() < 1)
			TaskComplete();
		else
		{
			m_vecMoveGoal = m_hTargetEnt->pev->origin;
			if (!MoveToTarget(ACT_WALK_SCARED, 0.5))
				TaskFail();
		}
		break;
	default:
		CTalkMonster::StartTask(pTask);
		break;
	}
}

//=========================================================
// RunTask
//=========================================================
void CConstruction::RunTask(Task_t *pTask) {
	switch (pTask->iTask) {
	case TASK_RUN_PATH_SCARED:
		if (MovementIsComplete())
			TaskComplete();

		if (RANDOM_LONG(0, 31) < 8)
			Scream();
		break;
	case TASK_MOVE_TO_TARGET_RANGE_SCARED:
		if (RANDOM_LONG(0, 63) < 8)
			Scream();

		if (m_hEnemy == NULL) {
			TaskFail();
		}
		else {
			float distance;

			distance = (m_vecMoveGoal - pev->origin).Length2D();
			// Re-evaluate when you think your finished, or the target has moved too far
			if ((distance < pTask->flData) || (m_vecMoveGoal - m_hTargetEnt->pev->origin).Length() > pTask->flData * 0.5) {
				m_vecMoveGoal = m_hTargetEnt->pev->origin;
				distance = (m_vecMoveGoal - pev->origin).Length2D();
				FRefreshRoute();
			}

			// Set the appropriate activity based on an overlapping range
			// overlap the range to prevent oscillation
			if (distance < pTask->flData) {
				TaskComplete();
				RouteClear();		// Stop moving
			}
			else if (distance < 190 && m_movementActivity != ACT_WALK_SCARED)
				m_movementActivity = ACT_WALK_SCARED;
			else if (distance >= 270 && m_movementActivity != ACT_RUN_SCARED)
				m_movementActivity = ACT_RUN_SCARED;
		}
		break;
	default:
		CTalkMonster::RunTask(pTask);
		break;
	}
}

//=========================================================
// Init talk data
//=========================================================
void CConstruction::TalkInit() {
	CScientist::TalkInit();

	if (!m_iszSpeakAs) {
		m_szGrp[TLK_ANSWER] = "GUS_ANSWER";
		m_szGrp[TLK_QUESTION] = "GUS_QUESTION";
		m_szGrp[TLK_IDLE] = "GUS_IDLE";
		m_szGrp[TLK_STARE] = "GUS_STARE";

		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_256)
			m_szGrp[TLK_USE] = "GUS_PFOLLOW";
		else
			m_szGrp[TLK_USE] = "GUS_OK";

		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_256)
			m_szGrp[TLK_UNUSE] = "GUS_PWAIT";
		else
			m_szGrp[TLK_UNUSE] = "GUS_WAIT";

		if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_256)
			m_szGrp[TLK_DECLINE] = "GUS_POK";
		else
			m_szGrp[TLK_DECLINE] = "GUS_NOTOK";

		m_szGrp[TLK_STOP] = "GUS_STOP";
		m_szGrp[TLK_NOSHOOT] = "GUS_SCARED";
		m_szGrp[TLK_HELLO] = "GUS_HELLO";

		m_szGrp[TLK_PLHURT1] = "!GUS_CUREA";
		m_szGrp[TLK_PLHURT2] = "!GUS_CUREB";
		m_szGrp[TLK_PLHURT3] = "!GUS_CUREC";

		m_szGrp[TLK_PHELLO] = "GUS_PHELLO";
		m_szGrp[TLK_PIDLE] = "GUS_PIDLE";
		m_szGrp[TLK_PQUESTION] = "GUS_PQUEST";
		m_szGrp[TLK_SMELL] = "GUS_SMELL";

		m_szGrp[TLK_WOUND] = "GUS_WOUND";
		m_szGrp[TLK_MORTAL] = "GUS_MORTAL";
	}
}

//=========================================================
// TraceAttack - Damage based on Hitgroups
//=========================================================
void CConstruction::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CScientist::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
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

			if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_CLUB)) {
				flDamage -= 20;
				if (flDamage <= 0) {
					UTIL_Ricochet(ptr->vecEndPos, 1.0);
					flDamage = 0.01;
				}
			}
			else {
				flDamage = m_flHitgroupHead * flDamage;
			}
			ptr->iHitgroup = HITGROUP_HEAD;
			break;
		case HITGROUP_CHEST:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_CHEST\n", GetClassname());
			flDamage = m_flHitgroupChest * flDamage;
			break;
		case HITGROUP_STOMACH:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_STOMACH\n", GetClassname());
			flDamage = m_flHitgroupStomach * flDamage;
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_ARM\n", GetClassname());
			flDamage = m_flHitgroupArm * flDamage;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			if (m_flDebug)
				ALERT(at_console, "%s:TraceAttack:HITGROUP_LEG\n", GetClassname());
			flDamage = m_flHitgroupLeg * flDamage;
			break;
		}
	}

	SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);// a little surface blood.
	TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
	CScientist::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}

//=========================================================
// MoveExecute - Hack for Construction Model
//=========================================================
void CConstruction::MoveExecute(CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval) {
	if (m_IdealActivity != m_movementActivity)
		m_IdealActivity = m_movementActivity;

	switch (m_Activity) {
	case ACT_WALK:
		m_flGroundSpeed = 55;
		break;
	case ACT_RUN:
		m_flGroundSpeed = 190;
		break;
	}

	float flTotal = m_flGroundSpeed * pev->framerate * flInterval;
	float flStep;
	while (flTotal > 0.001) {
		// don't walk more than 16 units or stairs stop working
		flStep = min(16.0, flTotal);
		UTIL_MoveToOrigin(ENT(pev), m_Route[m_iRouteIndex].vecLocation, flStep, MOVE_NORMAL);
		flTotal -= flStep;
	}
}