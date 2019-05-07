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
// NPC: V-22 Osprey * http://half-life.wikia.com/wiki/V-22_Osprey
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
/*
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "effects.h"
#include "particle_defs.h"
#include "customentity.h"
#include "monster_osprey.h"

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS(monster_osprey, COsprey);

TYPEDESCRIPTION	COsprey::m_SaveData[] = {
	DEFINE_FIELD(COsprey, m_pGoalEnt, FIELD_CLASSPTR),
	DEFINE_FIELD(COsprey, m_vel1, FIELD_VECTOR),
	DEFINE_FIELD(COsprey, m_vel2, FIELD_VECTOR),
	DEFINE_FIELD(COsprey, m_pos1, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(COsprey, m_pos2, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(COsprey, m_ang1, FIELD_VECTOR),
	DEFINE_FIELD(COsprey, m_ang2, FIELD_VECTOR),

	DEFINE_FIELD(COsprey, m_startTime, FIELD_TIME),
	DEFINE_FIELD(COsprey, m_dTime, FIELD_FLOAT),
	DEFINE_FIELD(COsprey, m_velocity, FIELD_VECTOR),

	DEFINE_FIELD(COsprey, m_flIdealtilt, FIELD_FLOAT),
	DEFINE_FIELD(COsprey, m_flRotortilt, FIELD_FLOAT),

	DEFINE_FIELD(COsprey, m_flRightHealth, FIELD_FLOAT),
	DEFINE_FIELD(COsprey, m_flLeftHealth, FIELD_FLOAT),

	DEFINE_FIELD(COsprey, m_iUnits, FIELD_INTEGER),
	DEFINE_FIELD(COsprey, m_monster_spawn, FIELD_INTEGER),
	DEFINE_ARRAY(COsprey, m_hGrunt, FIELD_EHANDLE, MAX_CARRY),
	DEFINE_ARRAY(COsprey, m_vecOrigin, FIELD_POSITION_VECTOR, MAX_CARRY),
	DEFINE_ARRAY(COsprey, m_hRepel, FIELD_EHANDLE, 4),

	DEFINE_FIELD(COsprey, m_iDoLeftSmokePuff, FIELD_INTEGER),
	DEFINE_FIELD(COsprey, m_iDoRightSmokePuff, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(COsprey, CBaseMonster);

//=========================================================
// Monster Sounds
//=========================================================
const char *COsprey::pDebrisSounds[] = {
	"weapons/debris1.wav",
	"weapons/debris2.wav",
	"weapons/debris3.wav"
};

const char *COsprey::pExplodeSounds[] = {
	"explosions/explode1.wav",
	"explosions/explode2.wav",
	"explosions/explode3.wav",
	"explosions/explode4.wav",
	"explosions/explode5.wav"
};

//=========================================================
// KeyValue
//=========================================================
void COsprey::KeyValue(KeyValueData *pkvd) {
	if (FStrEq(pkvd->szKeyName, "m_iMonster")) {
		m_iMonster = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else {
		CBaseMonster::KeyValue(pkvd);
	}
}

//=========================================================
// Spawn Osprey
//=========================================================
void COsprey::Spawn(void) {
	if (!m_iMonster) {
		m_iMonster = 5;
	}

	switch (m_iMonster) {
		case 1: m_monster_spawn = "monster_human_grunt_ally"; break; //Human Grunt (Ally)
		case 2:	m_monster_spawn = "monster_human_medic_ally"; break; //Human Grunt (Ally Medic)
		case 3: m_monster_spawn = "monster_human_torch_ally"; break; //Human Grunt (Ally Torch)
		case 5: m_monster_spawn = "monster_male_assassin"; break; //Black Ops
		default: m_monster_spawn = "monster_human_grunt"; break; //Human Grunt
	}

	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/osprey.mdl");

	UTIL_SetSize(pev, Vector(-400, -400, -100), Vector(400, 400, 32));
	UTIL_SetOrigin(this, pev->origin);

	// Motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	pev->flags |= FL_MONSTER | FL_FLY;
	pev->takedamage = DAMAGE_YES;
	pev->speed = 80; //LRC - default speed, in case path corners don't give a speed.

	if (pev->health == 0)
		pev->health = gSkillData.ospreyHealth;

	m_flFieldOfView = 0; // 180 degrees
	m_flRightHealth = (pev->health / 2);
	m_flLeftHealth = (pev->health / 2);

	pev->sequence = 0;
	ResetSequenceInfo();
	pev->frame = RANDOM_LONG(0, 0xFF);

	InitBoneControllers();

	SetThink(&COsprey::FindAllThink);
	SetUse(&COsprey::CommandUse);

	if (!(pev->spawnflags & SF_MONSTER_SPAWNFLAG_32)) {
		SetNextThink(1.0);
	}

	m_pos2 = pev->origin;
	m_ang2 = pev->angles;
	m_vel2 = pev->velocity;
}

//=========================================================
// Classify - indicates this monster's place in the
// relationship table.
//=========================================================
int	COsprey::Classify(void) {
	return m_iClass ? m_iClass : CLASS_MACHINE;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void COsprey::Precache(void) {
	if (m_iMonster == 4) {
		UTIL_PrecacheOther("monster_human_grunt_ally");
		UTIL_PrecacheOther("monster_human_medic_ally");
		UTIL_PrecacheOther("monster_human_torch_ally");
	}
	else {
		UTIL_PrecacheOther(m_monster_spawn);
	}

	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/osprey.mdl");

	PRECACHE_MODEL("models/HVR.mdl");

	m_iSpriteTexture = PRECACHE_MODEL("sprites/rope.spr");

	m_iExplode = PRECACHE_MODEL("sprites/fexplo.spr");
	m_iTailGibs = PRECACHE_MODEL("models/osprey_tailgibs.mdl");
	m_iBodyGibs = PRECACHE_MODEL("models/osprey_bodygibs.mdl");
	m_iEngineGibs = PRECACHE_MODEL("models/osprey_enginegibs.mdl");

	PRECACHE_SOUND_ARRAY(pDebrisSounds);
	PRECACHE_SOUND_ARRAY(pExplodeSounds);

	PRECACHE_SOUND("apache/ap_whine1.wav");
	PRECACHE_SOUND("apache/ap_rotor4.wav");
	PRECACHE_SOUND("weapons/mortarhit.wav");
}
*/