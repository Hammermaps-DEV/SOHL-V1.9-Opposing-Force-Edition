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
// NPC: Cleansuit - Scientist
// http://half-life.wikia.com/wiki/Black_Mesa_Science_Team
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
#include	"monster_scientist.h"
#include	"monster_cleansuit_scientist.h"

//=========================================================
// Cleansuit scientist
//=========================================================
#define	NUM_SCIENTIST_HEADS	4 // four heads available for scientist model
enum { HEAD_GLASSES = 0, HEAD_EINSTEIN = 1, HEAD_LUTHER = 2, HEAD_SLICK = 3 };

LINK_ENTITY_TO_CLASS(monster_cleansuit_scientist, CCleansuitScientist);

//=======================================================
// Custom Schedules
//=======================================================
extern Schedule_t *slFollow, *slFaceTarget, *slIdleSciStand, *slFear, *slScientistCover, *slScientistHide,
*slScientistStartle, *slStopFollowing, *slSciPanic, *slFollowScared, *slFaceTargetScared;
DEFINE_CUSTOM_SCHEDULES(CCleansuitScientist) {
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

IMPLEMENT_CUSTOM_SCHEDULES(CCleansuitScientist, CTalkMonster);

//=========================================================
// Spawn
//=========================================================
void CCleansuitScientist::Spawn(void) {
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/cleansuit_scientist.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_RED;

	if (pev->health == 0)
		pev->health = gSkillData.scientistHealth;

	pev->view_ofs = Vector(0, 0, 50);// position of the eyes relative to monster's origin.
	m_flFieldOfView = VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so scientists will notice player and say hello
	m_MonsterState = MONSTERSTATE_NONE;

	//	m_flDistTooFar		= 256.0;

	m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_OPEN_DOORS | bits_CAP_AUTO_DOORS | bits_CAP_USE;

	// White hands
	pev->skin = 0;

	if (pev->body == -1)
	{// -1 chooses a random head
		pev->body = RANDOM_LONG(0, NUM_SCIENTIST_HEADS - 1);// pick a head, any head
	}

	// Luther is black, make his hands black
	if (pev->body == HEAD_LUTHER)
		pev->skin = 1;

	MonsterInit();
	SetUse(&CScientist::FollowerUse);
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CCleansuitScientist::Precache(void) {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/cleansuit_scientist.mdl");

	CScientist::Precache();
}

void CCleansuitScientist::StartTask(Task_t *pTask) {
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
		else {
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

void CCleansuitScientist::RunTask(Task_t *pTask) {
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
// Dead Cleansuit Scientist PROP
//=========================================================
class CDeadCleansuitScientist : public CDeadScientist
{
public:
	virtual void Spawn(void);

	static char *m_szPoses[9];
};

LINK_ENTITY_TO_CLASS(monster_cleansuit_scientist_dead, CDeadCleansuitScientist);

char *CDeadCleansuitScientist::m_szPoses[] = { "lying_on_back", "lying_on_stomach", "dead_sitting", "dead_hang", "dead_table1", "dead_table2", "dead_table3", "scientist_deadpose1", "dead_against_wall" };

void CDeadCleansuitScientist::Spawn(void)
{
	PRECACHE_MODEL("models/cleansuit_scientist.mdl");
	SET_MODEL(ENT(pev), "models/cleansuit_scientist.mdl");

	pev->effects = 0;
	pev->sequence = 0;
	// Corpses have less health
	pev->health = 8;//gSkillData.scientistHealth;

	m_bloodColor = BLOOD_COLOR_RED;

	if (pev->body == -1)
	{// -1 chooses a random head
		pev->body = RANDOM_LONG(0, NUM_SCIENTIST_HEADS - 1);// pick a head, any head
	}
	// Luther is black, make his hands black
	if (pev->body == HEAD_LUTHER)
		pev->skin = 1;
	else
		pev->skin = 0;

	pev->sequence = LookupSequence(m_szPoses[m_iPose]);
	if (pev->sequence == -1)
	{
		ALERT(at_console, "Dead cleansuit scientist with bad pose\n");
	}

	//	pev->skin += 2; // use bloody skin -- UNDONE: Turn this back on when we have a bloody skin again!
	MonsterInitDead();
}

//=========================================================
// Sitting Cleansuit Scientist PROP
//=========================================================

class CSittingCleansuitScientist : public CSittingScientist
{
public:

	void Spawn(void);
};

LINK_ENTITY_TO_CLASS(monster_sitting_cleansuit_scientist, CSittingCleansuitScientist);


//
// ********** Cleansuit Scientist SPAWN **********
//
void CSittingCleansuitScientist::Spawn(void)
{
	PRECACHE_MODEL("models/cleansuit_scientist.mdl");
	SET_MODEL(ENT(pev), "models/cleansuit_scientist.mdl");
	Precache();
	InitBoneControllers();

	UTIL_SetSize(pev, Vector(-14, -14, 0), Vector(14, 14, 36));

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	pev->effects = 0;
	pev->health = 50;

	m_bloodColor = BLOOD_COLOR_RED;
	m_flFieldOfView = VIEW_FIELD_WIDE; // indicates the width of this monster's forward view cone ( as a dotproduct result )

	m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD;

	SetBits(pev->spawnflags, SF_MONSTER_SPAWNFLAG_256); // predisaster only!

	if (pev->body == -1)
	{// -1 chooses a random head
		pev->body = RANDOM_LONG(0, NUM_SCIENTIST_HEADS - 1);// pick a head, any head
	}
	// Luther is black, make his hands black
	if (pev->body == HEAD_LUTHER)
		pev->skin = 1;

	m_baseSequence = LookupSequence("sitlookleft");
	pev->sequence = m_baseSequence + RANDOM_LONG(0, 4);
	ResetSequenceInfo();

	SetThink(&CSittingScientist::SittingThink);
	SetNextThink(0.1);

	DROP_TO_FLOOR(ENT(pev));
}