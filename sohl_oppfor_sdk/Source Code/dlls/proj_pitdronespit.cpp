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
// Projectile: Pit Drone Spit * http://half-life.wikia.com/wiki/Pit_Drone
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"proj_pitdronespit.h"

//=========================================================
// Projectile's link to Class
//=========================================================
LINK_ENTITY_TO_CLASS(pitdronespit, CPitDroneSpit);

//=========================================================
// Projectile Sounds
//=========================================================
const char *CPitDroneSpit::pHitSounds[] = {
	"weapons/xbow_hitbod1.wav",
	"weapons/xbow_hitbod2.wav",
};

const char *CPitDroneSpit::pMissSounds[] = {
	"weapons/xbow_hit1.wav",
	"weapons/xbow_hit2.wav",
};

//=========================================================
// Spawn
//=========================================================
void CPitDroneSpit::Spawn() {
	Precache();

	pev->classname = MAKE_STRING("pitdronespit");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_NO;
	pev->flags |= FL_MONSTER;
	pev->health = 1;

	m_flFieldOfView = 0.9;

	SET_MODEL(ENT(pev), "models/pit_drone_spike.mdl");
	UTIL_SetSize(pev, Vector(-4, -4, -4), Vector(4, 4, 4));

	SetTouch(&CPitDroneSpit::DieTouch);
	SetThink(&CPitDroneSpit::StartTrack);

	SetNextThink(0.1);
	ResetSequenceInfo();
}

//=========================================================
// Precache - precaches all resources
//=========================================================
void CPitDroneSpit::Precache() {
	PRECACHE_MODEL("models/pit_drone_spike.mdl");
	iDroneSpitTrail = PRECACHE_MODEL("sprites/spike_trail.spr");
	PRECACHE_SOUND_ARRAY(pHitSounds);
	PRECACHE_SOUND_ARRAY(pMissSounds);
}

//=========================================================
// spits will never get mad at each other, no matter who the owner is.
//=========================================================
int CPitDroneSpit::IRelationship(CBaseEntity *pTarget) {
	if (pTarget->pev->modelindex == pev->modelindex) {
		return R_NO;
	}

	return CBaseMonster::IRelationship(pTarget);
}

//=========================================================
// ID's Spit as their owner
//=========================================================
int CPitDroneSpit::Classify() {
	if (m_iClass) return m_iClass;
	return CLASS_ALIEN_BIOWEAPON;
}

//=========================================================
// StartTrack - starts a spit out tracking its target
//=========================================================
void CPitDroneSpit::StartTrack() {
	IgniteTrail();
	SetTouch(&CPitDroneSpit::TrackTouch);
	SetThink(&CPitDroneSpit::TrackTarget);
	SetNextThink(0.1);
}

//=========================================================
// StartDart - starts a spit out just flying straight.
//=========================================================
void CPitDroneSpit::StartDart() {
	IgniteTrail();
	SetTouch(&CPitDroneSpit::DieTouch);
	SetThink(&CPitDroneSpit::SUB_Remove);
	SetNextThink(4);
}

void CPitDroneSpit::IgniteTrail() {
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(entindex());	// entity
	WRITE_SHORT(iDroneSpitTrail);	// model
	WRITE_BYTE(2); // life
	WRITE_BYTE(1);  // width
	WRITE_BYTE(170);   // r, g, b
	WRITE_BYTE(170);   // r, g, b
	WRITE_BYTE(0);   // r, g, b
	WRITE_BYTE(200);	// brightness
	MESSAGE_END();
}

//=========================================================
// Spit is flying, gently tracking target
//=========================================================
void CPitDroneSpit::TrackTarget() {
	Vector	vecFlightDir;
	Vector	vecDirToEnemy;
	float	flDelta;

	StudioFrameAdvance();

	// UNDONE: The player pointer should come back after returning from another level
	if (m_hEnemy == NULL) {
		Look(512);
		m_hEnemy = BestVisibleEnemy();
	}

	if (m_hEnemy != NULL && FVisible(m_hEnemy)) {
		m_vecEnemyLKP = m_hEnemy->BodyTarget(pev->origin);
	}
	else {
		m_vecEnemyLKP = m_vecEnemyLKP + pev->velocity * 700 * 0.1;
	}

	vecDirToEnemy = (m_vecEnemyLKP - pev->origin).Normalize();
	if (pev->velocity.Length() < 0.1)
		vecFlightDir = vecDirToEnemy;
	else
		vecFlightDir = pev->velocity.Normalize();

	// measure how far the turn is, the wider the turn, the slow we'll go this time.
	flDelta = DotProduct(vecFlightDir, vecDirToEnemy);
	pev->velocity = (vecFlightDir + vecDirToEnemy).Normalize();

	pev->velocity = pev->velocity * 700;// do not have to slow down to turn.
	SetNextThink(0.1);// fixed think time

	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->solid = SOLID_BBOX;

	// if hornet is close to the enemy, jet in a straight line for a half second.
	// (only in the single player game)
	if (m_hEnemy != NULL && !g_pGameRules->IsMultiplayer()) {
		if (flDelta >= 0.4 && (pev->origin - m_vecEnemyLKP).Length() <= 300) {
			pev->velocity = pev->velocity * 2;
			SetNextThink(1.0);
		}
	}
}

//=========================================================
// Tracking Spit hit something
//=========================================================
void CPitDroneSpit::TrackTouch(CBaseEntity *pOther) {
	if (pOther->edict() == pev->owner || pOther->pev->modelindex == pev->modelindex) {
		pev->solid = SOLID_NOT;
		return;
	}

	DieTouch(pOther);
}

void CPitDroneSpit::DieTouch(CBaseEntity *pOther) {
	if (pOther->pev->takedamage) {
		TraceResult tr = UTIL_GetGlobalTrace();
		entvars_t	*pevOwner;
		pevOwner = VARS(pev->owner);
		pev->velocity = Vector(0, 0, 0);

		ClearMultiDamage();
		pOther->TraceAttack(pevOwner, gSkillData.pitdroneDmgSpit, pev->velocity.Normalize(), &tr, DMG_GENERIC | DMG_NEVERGIB);
		ApplyMultiDamage(pev, pevOwner);
		EMIT_SOUND_ARRAY_DYN(CHAN_BODY, pHitSounds);
		SetThink(&CPitDroneSpit::SUB_Remove);
		SetNextThink(0);

		if (!IsMultiplayer()) {
			Killed(pev, GIB_NEVER);
		}
	}
	else {
		EMIT_SOUND_ARRAY_DYN(CHAN_BODY, pMissSounds);
		SetThink(&CPitDroneSpit::SUB_Remove);
		SetNextThink(0);

		if (FClassnameIs(pOther->pev, "worldspawn")) {
			Vector vecDir = pev->velocity.Normalize();
			UTIL_SetOrigin(this, pev->origin - vecDir * 4);
			pev->angles = UTIL_VecToAngles(vecDir);
			pev->solid = SOLID_NOT;
			pev->movetype = MOVETYPE_FLY;
			pev->velocity = Vector(0, 0, 0);
			pev->avelocity.z = 0;
			pev->angles.z = RANDOM_LONG(0, 360);
			SetNextThink(10);
		}
	}
}

