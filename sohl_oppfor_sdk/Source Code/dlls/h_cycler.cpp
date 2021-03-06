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
/*

===== h_cycler.cpp ========================================================

  The Halflife Cycler Monsters

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "CCycler.h"
#include "CGenericCycler.h"
#include "CCyclerProbe.h"
#include "CWeaponCycler.h"
#include "player.h"
#include "CCyclerSprite.h"

#define TEMP_FOR_SCREEN_SHOTS
#ifdef TEMP_FOR_SCREEN_SHOTS //===================================================

TYPEDESCRIPTION	CCycler::m_SaveData[] =
{
	DEFINE_FIELD(CCycler, m_animate, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CCycler, CBaseMonster);

LINK_ENTITY_TO_CLASS(cycler, CGenericCycler);

LINK_ENTITY_TO_CLASS(cycler_prdroid, CCyclerProbe);

void CCyclerProbe::Spawn()
{
	pev->origin = pev->origin + Vector(0, 0, 16);
	GenericCyclerSpawn("models/prdroid.mdl", Vector(-16, -16, -16), Vector(16, 16, 16));
}



// Cycler member functions

void CCycler::GenericCyclerSpawn(char *szModel, Vector vecMin, Vector vecMax)
{
	if (!szModel || !*szModel)
	{
		ALERT(at_error, "cycler at %.0f %.0f %0.f missing modelname", pev->origin.x, pev->origin.y, pev->origin.z);
		REMOVE_ENTITY(ENT(pev));
		return;
	}

	pev->classname = MAKE_STRING("cycler");
	PRECACHE_MODEL(szModel);
	SET_MODEL(ENT(pev), szModel);

	CCycler::Spawn();

	UTIL_SetSize(pev, vecMin, vecMax);
}


void CCycler::Spawn()
{
	InitBoneControllers();
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_YES;
	pev->effects = 0;
	pev->health = 80000;// no cycler should die
	pev->yaw_speed = 5;
	pev->ideal_yaw = pev->angles.y;
	ChangeYaw(360);

	m_flFrameRate = 75;
	m_flGroundSpeed = 0;

	AbsoluteNextThink(m_fNextThink + 1.0);

	ResetSequenceInfo();

	if (pev->sequence != 0 || pev->frame != 0)
	{
		m_animate = 0;
		pev->framerate = 0;
	}
	else
	{
		m_animate = 1;
	}
}




//
// cycler think
//
void CCycler::Think()
{
	SetNextThink(0.1);

	if (m_animate)
	{
		StudioFrameAdvance();
	}
	if (m_fSequenceFinished && !m_fSequenceLoops)
	{
		// ResetSequenceInfo();
		// hack to avoid reloading model every frame
		pev->animtime = UTIL_GlobalTimeBase();
		pev->framerate = 1.0;
		m_fSequenceFinished = FALSE;
		m_flLastEventCheck = UTIL_GlobalTimeBase();
		pev->frame = 0;
		if (!m_animate)
			pev->framerate = 0.0;	// FIX: don't reset framerate
	}
}

//
// CyclerUse - starts a rotation trend
//
void CCycler::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	m_animate = !m_animate;
	if (m_animate)
		pev->framerate = 1.0;
	else
		pev->framerate = 0.0;
}

//
// CyclerPain , changes sequences when shot
//
//void CCycler :: Pain( float flDamage )
int CCycler::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	if (m_animate)
	{
		pev->sequence++;

		ResetSequenceInfo();

		if (m_flFrameRate == 0.0)
		{
			pev->sequence = 0;
			ResetSequenceInfo();
		}
		pev->frame = 0;
	}
	else
	{
		pev->framerate = 1.0;
		StudioFrameAdvance(0.1);
		pev->framerate = 0;
		ALERT(at_debug, "sequence: %d, frame %.0f\n", pev->sequence, pev->frame);
	}

	return 0;
}

#endif

LINK_ENTITY_TO_CLASS(cycler_sprite, CCyclerSprite);

TYPEDESCRIPTION	CCyclerSprite::m_SaveData[] =
{
	DEFINE_FIELD(CCyclerSprite, m_animate, FIELD_INTEGER),
	DEFINE_FIELD(CCyclerSprite, m_lastTime, FIELD_TIME),
	DEFINE_FIELD(CCyclerSprite, m_maxFrame, FIELD_FLOAT),
};

IMPLEMENT_SAVERESTORE(CCyclerSprite, CBaseEntity);


void CCyclerSprite::Spawn()
{
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_YES;
	pev->effects = 0;

	pev->frame = 0;
	SetNextThink(0.1);
	m_animate = 1;
	m_lastTime = UTIL_GlobalTimeBase();

	PRECACHE_MODEL((char *)STRING(pev->model));
	SET_MODEL(ENT(pev), STRING(pev->model));

	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex) - 1;
}


void CCyclerSprite::Think()
{
	if (ShouldAnimate())
		Animate(pev->framerate * (UTIL_GlobalTimeBase() - m_lastTime));

	SetNextThink(0.1);
	m_lastTime = UTIL_GlobalTimeBase();
}


void CCyclerSprite::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	m_animate = !m_animate;
	ALERT(at_debug, "Sprite: %s\n", STRING(pev->model));
}


int	CCyclerSprite::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	if (m_maxFrame > 1.0)
	{
		Animate(1.0);
	}
	return 1;
}

void CCyclerSprite::Animate(float frames)
{
	pev->frame += frames;
	if (m_maxFrame > 0)
		pev->frame = fmod(pev->frame, m_maxFrame);
}

LINK_ENTITY_TO_CLASS(cycler_weapon, CWeaponCycler);
LINK_ENTITY_TO_CLASS(weapon_question, CWeaponCycler);	// saverestore issues

TYPEDESCRIPTION	CWeaponCycler::m_SaveData[] =
{
	DEFINE_FIELD(CWeaponCycler, m_iPlayerModel, FIELD_MODELNAME),
	DEFINE_FIELD(CWeaponCycler, m_iViewModel, FIELD_MODELNAME),
	DEFINE_FIELD(CWeaponCycler, m_iWorldModel, FIELD_MODELNAME),
}; IMPLEMENT_SAVERESTORE(CWeaponCycler, CBasePlayerWeapon);

void CWeaponCycler::Spawn()
{
	// g-cont. this alias need for right slot switching because all selectable items must be preceed with "weapon_" or "item_"
	pev->classname = MAKE_STRING("weapon_question");
	m_iId = WEAPON_CYCLER;
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_NONE;

	char basemodel[80], v_path[80], p_path[80], w_path[80];

	strncpy(basemodel, (char *)STRING(pev->model), sizeof(basemodel) - 1);

	for (int i = 0; i < (int)strlen(basemodel); i++)
	{
		int c = basemodel[i];

		if ((c == 'v' || c == 'p' || c == 'w') && basemodel[i + 1] == '_')
		{
			basemodel[i] = 'v';
			strcpy(v_path, basemodel);
			basemodel[i] = 'p';
			strcpy(p_path, basemodel);
			basemodel[i] = 'w';
			strcpy(w_path, basemodel);

			// create wepon model pathes
			m_iPlayerModel = ALLOC_STRING(p_path);
			m_iWorldModel = ALLOC_STRING(w_path);
			m_iViewModel = ALLOC_STRING(v_path);
			break;
		}
	}

	if (m_iPlayerModel && m_iWorldModel && m_iViewModel)
	{
		PRECACHE_MODEL((char *)STRING(m_iPlayerModel));
		PRECACHE_MODEL((char *)STRING(m_iWorldModel));
		PRECACHE_MODEL((char *)STRING(m_iViewModel));

		// set right world model
		pev->model = m_iWorldModel;

		SET_MODEL(ENT(pev), STRING(pev->model));
	}
	else
	{
		// fallback to default relationship
		PRECACHE_MODEL((char *)STRING(pev->model));
		SET_MODEL(ENT(pev), STRING(pev->model));

		// setup viewmodel
		m_iViewModel = pev->model;
	}
	m_iClip = -1;
	FallInit();
}

void CWeaponCycler::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "deploy"))
	{
		pev->impulse = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "holster"))
	{
		pev->button = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "primary"))
	{
		pev->sequence = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "secondary"))
	{
		pev->team = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else CBasePlayerWeapon::KeyValue(pkvd);
}

int CWeaponCycler::GetItemInfo(ItemInfo *p)
{
	p->pszName = "weapon_question";	// need for right HUD displaying
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_CYCLER;
	p->iWeight = -1;

	return 1;
}

BOOL CWeaponCycler::Deploy()
{
	return DefaultDeploy(m_iViewModel, m_iPlayerModel, pev->impulse, "onehanded", 0.5);
}

void CWeaponCycler::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_GlobalTimeBase() + 1.0;
	SendWeaponAnim(pev->button);
}

void CWeaponCycler::PrimaryAttack()
{
	SendWeaponAnim(pev->sequence);

	m_flNextPrimaryAttack = UTIL_GlobalTimeBase() + 0.5;
}


void CWeaponCycler::SecondaryAttack()
{
	SendWeaponAnim(pev->team);

	m_flNextSecondaryAttack = UTIL_GlobalTimeBase() + 0.5;
}

// Flaming Wreakage
class CWreckage : public CBaseMonster
{
	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn();
	void Precache();
	void Think();

	int m_flStartTime;
};
TYPEDESCRIPTION	CWreckage::m_SaveData[] =
{
	DEFINE_FIELD(CWreckage, m_flStartTime, FIELD_TIME),
};
IMPLEMENT_SAVERESTORE(CWreckage, CBaseMonster);


LINK_ENTITY_TO_CLASS(cycler_wreckage, CWreckage);

void CWreckage::Spawn()
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = 0;
	pev->effects = 0;

	pev->frame = 0;
	SetNextThink(0.1);

	if (pev->model)
	{
		PRECACHE_MODEL((char *)STRING(pev->model));
		SET_MODEL(ENT(pev), STRING(pev->model));
	}
	// pev->scale = 5.0;

	m_flStartTime = UTIL_GlobalTimeBase();
}

void CWreckage::Precache()
{
	if (pev->model)
		PRECACHE_MODEL((char *)STRING(pev->model));
}

void CWreckage::Think()
{
	StudioFrameAdvance();
	SetNextThink(0.2);

	if (pev->dmgtime)
	{
		if (pev->dmgtime < UTIL_GlobalTimeBase())
		{
			UTIL_Remove(this);
			return;
		}
		else if (RANDOM_FLOAT(0, pev->dmgtime - m_flStartTime) > pev->dmgtime - UTIL_GlobalTimeBase())
		{
			return;
		}
	}

	Vector VecSrc;

	VecSrc.x = RANDOM_FLOAT(pev->absmin.x, pev->absmax.x);
	VecSrc.y = RANDOM_FLOAT(pev->absmin.y, pev->absmax.y);
	VecSrc.z = RANDOM_FLOAT(pev->absmin.z, pev->absmax.z);

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, VecSrc);
	WRITE_BYTE(TE_SMOKE);
	WRITE_COORD(VecSrc.x);
	WRITE_COORD(VecSrc.y);
	WRITE_COORD(VecSrc.z);
	WRITE_SHORT(g_sModelIndexSmoke);
	WRITE_BYTE(RANDOM_LONG(0, 49) + 50); // scale * 10
	WRITE_BYTE(RANDOM_LONG(0, 3) + 8); // framerate
	MESSAGE_END();
}
