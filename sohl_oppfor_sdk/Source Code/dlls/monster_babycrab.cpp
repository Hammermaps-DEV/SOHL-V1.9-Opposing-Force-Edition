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
// babyheadcrab.cpp - Baby, jumpy alien parasite
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"game.h"
#include	"monster_babycrab.h"

LINK_ENTITY_TO_CLASS(monster_babycrab, CBabyCrab);

Task_t	tlBHCRangeAttack1Fast[] =
{
	{ TASK_STOP_MOVING,			(float)0 },
	{ TASK_FACE_IDEAL,			(float)0 },
	{ TASK_RANGE_ATTACK1,		(float)0 },
	{ TASK_SET_ACTIVITY,		(float)ACT_IDLE },
};

Schedule_t	slBHCRangeAttack1Fast[] =
{
	{
		tlBHCRangeAttack1Fast,
		HL_ARRAYSIZE(tlBHCRangeAttack1Fast),
	bits_COND_ENEMY_OCCLUDED |
	bits_COND_NO_AMMO_LOADED,
	0,
	"BHCRAFast"
	},
};

void CBabyCrab::Spawn(void)
{
	CHeadCrab::Spawn();
	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/baby_headcrab.mdl");
	pev->rendermode = kRenderTransTexture;
	pev->renderamt = 192;
	UTIL_SetSize(pev, Vector(-12, -12, 0), Vector(12, 12, 24));

	pev->health = gSkillData.headcrabHealth * 0.25;	// less health than full grown
}

void CBabyCrab::Precache(void)
{
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/baby_headcrab.mdl");
	CHeadCrab::Precache();
}


void CBabyCrab::SetYawSpeed(void)
{
	pev->yaw_speed = 120;
}


BOOL CBabyCrab::CheckRangeAttack1(float flDot, float flDist)
{
	if (pev->flags & FL_ONGROUND)
	{
		if (pev->groundentity && (pev->groundentity->v.flags & (FL_CLIENT | FL_MONSTER)))
			return TRUE;

		// A little less accurate, but jump from closer
		if (flDist <= 180 && flDot >= 0.55)
			return TRUE;
	}

	return FALSE;
}


Schedule_t* CBabyCrab::GetScheduleOfType(int Type)
{
	switch (Type)
	{
	case SCHED_FAIL:	// If you fail, try to jump!
		if (m_hEnemy != NULL)
			return slBHCRangeAttack1Fast;
		break;

	case SCHED_RANGE_ATTACK1:
	{
		return slBHCRangeAttack1Fast;
	}
	break;
	}

	return CHeadCrab::GetScheduleOfType(Type);
}
