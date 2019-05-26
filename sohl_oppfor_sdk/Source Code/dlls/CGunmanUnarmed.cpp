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
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"CGunmanUnarmed.h"
#include	"soundent.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
LINK_ENTITY_TO_CLASS(monster_human_unarmed, CGunmanUnarmed);

TYPEDESCRIPTION	CGunmanUnarmed::m_SaveData[] = {
	DEFINE_FIELD(CGunmanUnarmed, m_useTime, FIELD_TIME),
};

IMPLEMENT_SAVERESTORE(CGunmanUnarmed, CBaseMonster);

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CGunmanUnarmed::Classify()
{
	return m_iClass ? m_iClass : CLASS_PLAYER_ALLY;
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CGunmanUnarmed::SetYawSpeed()
{
	switch (m_Activity)
	{
	case ACT_IDLE:
	default:
		pev->yaw_speed = 90;
	}
}

void CGunmanUnarmed::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "gn_idle"))
	{
		m_szIdleSentences = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "gn_stare"))
	{
		m_szStareSentences = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "gn_noshoot"))
	{
		m_szPainSentences = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "gn_use"))
	{
		m_szUseSentences = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "gn_unuse"))
	{
		m_szUnuseSentences = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "gunstate"))
	{
		m_iBodygroup = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue(pkvd);
}

//=========================================================
// Spawn
//=========================================================
void CGunmanUnarmed::Spawn()
{
	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/gunmanchronicles/gunmantrooper_ng.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	SetBodygroup(GROUP_WEAPON, m_iBodygroup);

	SetSolidType(SOLID_SLIDEBOX);
	SetMoveType(MOVETYPE_STEP);
	m_bloodColor = BLOOD_COLOR_RED;
	pev->health = 100;
	pev->view_ofs = Vector(0, 0, 50);// position of the eyes relative to monster's origin.
	m_flFieldOfView = VIEW_FIELD_FULL;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState = MONSTERSTATE_NONE;

	m_afCapability = bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;

	MonsterInit();

	if (pev->spawnflags & SF_HUMANUNARMED_NOTSOLID != 0)
	{
		SetSolidType(SOLID_NOT);
		pev->takedamage = DAMAGE_NO;
	}
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CGunmanUnarmed::Precache()
{
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/gunmanchronicles/gunmantrooper_ng.mdl");
}

//=========================================================
//=========================================================
Schedule_t *CGunmanUnarmed::GetSchedule()
{
	// so we don't keep calling through the EHANDLE stuff
	CBaseEntity *pEnemy = m_hEnemy;

	if (HasConditions(bits_COND_HEAR_SOUND))
	{
		CSound* pSound = PBestSound();

		ASSERT(pSound != NULL);
		if (pSound && (pSound->m_iType & bits_SOUND_DANGER))
		{
			// dangerous sound nearby!
			return GetScheduleOfType(SCHED_TAKE_COVER_FROM_BEST_SOUND);
		}
	}

	switch (m_MonsterState)
	{
		case MONSTERSTATE_ALERT:
		case MONSTERSTATE_IDLE:
		{
			if (pEnemy != NULL)
			{
				m_hEnemy = NULL;
				pEnemy = NULL;
			}

			if (HasConditions(bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE))
			{
				return GetScheduleOfType(SCHED_SMALL_FLINCH);
			}

			// Cower when you hear something scary
			if (HasConditions(bits_COND_HEAR_SOUND))
			{
				CSound* pSound = PBestSound();

				ASSERT(pSound != NULL);
				if (pSound)
				{
					if (pSound->m_iType & (bits_SOUND_DANGER | bits_SOUND_COMBAT))
					{
						return GetScheduleOfType(SCHED_IDLE_STAND);
					}
				}
			}
		}
		break;
		case MONSTERSTATE_COMBAT:
		{
			return GetScheduleOfType(SCHED_IDLE_STAND);
		}
	}

	return CBaseMonster::GetSchedule();
}

//=========================================================
// IdleSound
//=========================================================
void CGunmanUnarmed::IdleSound() 
{
	char szBuf[32];
	strcpy(szBuf, STRING(m_szStareSentences));
	PlaySentence(szBuf, RANDOM_FLOAT(5.0, 7.5), VOL_NORM, ATTN_IDLE);
}

//=========================================================
// AlertSound 
//=========================================================
void CGunmanUnarmed::PainSound() 
{
	char szBuf[32];
	strcpy(szBuf, STRING(m_szPainSentences));
	PlaySentence(szBuf, 2, VOL_NORM, ATTN_IDLE);
}

void CGunmanUnarmed::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Don't allow use during a scripted_sentence
	if (m_useTime > UTIL_GlobalTimeBase())
		return;

	if (pCaller != NULL && pCaller->IsPlayer() && CanPlaySentence(false))
	{
		char szBuf[32];
		strcpy(szBuf, STRING(m_szUseSentences));
		PlaySentence(szBuf, 10.0, VOL_NORM, ATTN_IDLE);
	}
}

//=========================================================
// PlayScriptedSentence
//=========================================================
void CGunmanUnarmed::PlayScriptedSentence(const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener) {

	m_useTime = UTIL_GlobalTimeBase() + duration;
	CBaseMonster::PlayScriptedSentence(pszSentence, duration, volume, attenuation, bConcurrent, pListener);
}