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

//=========================================================
// Generic Monster - purely for scripted sequence work.
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"schedule.h"
#include	"monster_loader.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
LINK_ENTITY_TO_CLASS(monster_op4loader, CLoader);
LINK_ENTITY_TO_CLASS(monster_loader, CLoader);

//=========================================================
// Spawn
//=========================================================
void CLoader::Spawn()
{
	Precache();
	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/loader.mdl");

	if (FStrEq(STRING(pev->model), "models/player.mdl") || FStrEq(STRING(pev->model), "models/holo.mdl"))
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = DONT_BLEED;
	pev->health = 100;
	m_flFieldOfView = 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState = MONSTERSTATE_NONE;

	if (pev->spawnflags & SF_LOADER_NOTSOLID) {
		pev->solid = SOLID_NOT;
		pev->takedamage = DAMAGE_NO;
	}

	MonsterInit();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CLoader::Precache()
{
	if (pev->model)
		PRECACHE_MODEL(const_cast<char*>(STRING(pev->model))); //LRC
	else
		PRECACHE_MODEL("models/loader.mdl");

	PRECACHE_SOUND("ambience/loader_step1.wav");
	PRECACHE_SOUND("ambience/loader_hydra1.wav");
}

//=========================================================
// SetYawSpeed
//=========================================================
void CLoader::SetYawSpeed()
{
	pev->yaw_speed = 90;
}

//=========================================================
// TakeDamage
//=========================================================
int CLoader::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	//Don't take damage
	return true;
}

//=========================================================
// TraceAttack
//=========================================================
void CLoader::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	UTIL_Ricochet(ptr->vecEndPos, RANDOM_FLOAT(1.0, 2.0));
}

//=========================================================
// HandleAnimEvent
//=========================================================
void CLoader::HandleAnimEvent(MonsterEvent_t* pEvent)
{
	CBaseMonster::HandleAnimEvent(pEvent);
}

//=========================================================
// StartTask
//=========================================================
void CLoader::StartTask(Task_t* pTask)
{
	float newYawAngle;

	switch (pTask->iTask)
	{
	case TASK_TURN_LEFT:
		newYawAngle = UTIL_AngleMod(pev->angles.y) + pTask->flData;
		break;

	case TASK_TURN_RIGHT:
		newYawAngle = UTIL_AngleMod(pev->angles.y) - pTask->flData;
		break;

	default:
		CBaseMonster::StartTask(pTask);
		return;
	}

	pev->ideal_yaw = UTIL_AngleMod(newYawAngle);

	SetTurnActivity();
}

//=========================================================
// SetTurnActivity
//=========================================================
void CLoader::SetTurnActivity()
{
	const auto difference = FlYawDiff();

	if (difference <= -45 && LookupActivity(ACT_TURN_RIGHT) != -1)
	{
		m_IdealActivity = ACT_TURN_RIGHT;
	}
	else if (difference > 45.0 && LookupActivity(ACT_TURN_LEFT) != -1)
	{
		m_IdealActivity = ACT_TURN_LEFT;
	}

	CBaseMonster::SetTurnActivity();
}