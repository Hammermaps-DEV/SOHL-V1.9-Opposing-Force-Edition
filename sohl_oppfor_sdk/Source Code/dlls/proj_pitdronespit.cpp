/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"proj_pitdronespit.h"

//=========================================================
// CPitDrone's spit projectile
//=========================================================
LINK_ENTITY_TO_CLASS(pitdronespit, CPitDroneSpit);

//=========================================================
//=========================================================
void CPitDroneSpit::Spawn(void) {
	Precache();

	pev->classname = MAKE_STRING("pitdronespit");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_NO;
	pev->flags |= FL_MONSTER;
	pev->health = 1;// weak!

	m_flFieldOfView = 0.9; // +- 25 degrees

	SET_MODEL(ENT(pev), "models/pit_drone_spike.mdl");
	UTIL_SetSize(pev, Vector(-4, -4, -4), Vector(4, 4, 4));

	SetTouch(&CPitDroneSpit::DieTouch);
	SetThink(&CPitDroneSpit::StartTrack);

	SetNextThink(0.1);
	ResetSequenceInfo();
}

void CPitDroneSpit::Precache() {
	PRECACHE_MODEL("models/pit_drone_spike.mdl");

	PRECACHE_SOUND("weapons/xbow_hitbod1.wav");
	PRECACHE_SOUND("weapons/xbow_hitbod2.wav");

	PRECACHE_SOUND("weapons/xbow_fly1.wav");
	PRECACHE_SOUND("weapons/xbow_hit1.wav");
	PRECACHE_SOUND("weapons/xbow_hit2.wav");

	iDroneSpitTrail = PRECACHE_MODEL("sprites/spike_trail.spr");
}

//=========================================================
// hornets will never get mad at each other, no matter who the owner is.
//=========================================================
int CPitDroneSpit::IRelationship(CBaseEntity *pTarget) {
	if (pTarget->pev->modelindex == pev->modelindex) {
		return R_NO;
	}

	return CBaseMonster::IRelationship(pTarget);
}

//=========================================================
// ID's Hornet as their owner
//=========================================================
int CPitDroneSpit::Classify(void) {
	if (m_iClass) return m_iClass;
	return	CLASS_ALIEN_BIOWEAPON;
}

//=========================================================
// StartTrack - starts a hornet out tracking its target
//=========================================================
void CPitDroneSpit::StartTrack(void) {
	IgniteTrail();
	SetTouch(&CPitDroneSpit::TrackTouch);
	SetThink(&CPitDroneSpit::TrackTarget);
	SetNextThink(0.1);
}

//=========================================================
// StartDart - starts a hornet out just flying straight.
//=========================================================
void CPitDroneSpit::StartDart(void) {
	IgniteTrail();
	SetTouch(&CPitDroneSpit::DartTouch);
	SetThink(&CPitDroneSpit::SUB_Remove);
	SetNextThink(4);
}

void CPitDroneSpit::IgniteTrail(void) {
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
// Hornet is flying, gently tracking target
//=========================================================
void CPitDroneSpit::TrackTarget(void) {
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
	} else {
		m_vecEnemyLKP = m_vecEnemyLKP + pev->velocity * 600 * 0.1;
	}

	vecDirToEnemy = (m_vecEnemyLKP - pev->origin).Normalize();
	if (pev->velocity.Length() < 0.1)
		vecFlightDir = vecDirToEnemy;
	else
		vecFlightDir = pev->velocity.Normalize();

	// measure how far the turn is, the wider the turn, the slow we'll go this time.
	flDelta = DotProduct(vecFlightDir, vecDirToEnemy);
	pev->velocity = (vecFlightDir + vecDirToEnemy).Normalize();

	if (pev->owner && (pev->owner->v.flags & FL_MONSTER)) {
		// random pattern only applies to hornets fired by monsters, not players. 
		pev->velocity.x += RANDOM_FLOAT(-0.10, 0.10);// scramble the flight dir a bit.
		pev->velocity.y += RANDOM_FLOAT(-0.10, 0.10);
		pev->velocity.z += RANDOM_FLOAT(-0.10, 0.10);
	}

	pev->velocity = pev->velocity * 600;// do not have to slow down to turn.
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
// Tracking Hornet hit something
//=========================================================
void CPitDroneSpit::TrackTouch(CBaseEntity *pOther) {
	if (pOther->edict() == pev->owner || pOther->pev->modelindex == pev->modelindex) {
		pev->solid = SOLID_NOT;
		return;
	}

	DieTouch(pOther);
}

void CPitDroneSpit::DartTouch(CBaseEntity *pOther) {
	DieTouch(pOther);
}

void CPitDroneSpit::DieTouch(CBaseEntity *pOther) {
	if(pOther->pev->takedamage) {
		TraceResult tr = UTIL_GetGlobalTrace();
		entvars_t	*pevOwner;
		pevOwner = VARS(pev->owner);
		pev->velocity = Vector(0, 0, 0);

		switch (RANDOM_LONG(0, 1)) {
			case 0:
				EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/xbow_hitbod1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, RANDOM_FLOAT(90, 110));
			break;
			case 1:
				EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/xbow_hitbod2.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, RANDOM_FLOAT(90, 110));
			break;
		}

		// UNDONE: this needs to call TraceAttack instead
		ClearMultiDamage();
		pOther->TraceAttack(pevOwner, gSkillData.pitdroneDmgSpit, pev->velocity.Normalize(), &tr, DMG_GENERIC | DMG_NEVERGIB);
		ApplyMultiDamage(pev, pevOwner);
		SetThink(&CPitDroneSpit::SUB_Remove);
		SetNextThink(0);

		if (!IsMultiplayer()) {
			Killed(pev, GIB_NEVER);
		}
	} else {
		switch (RANDOM_LONG(0, 1)) {
		case 0:
			EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/xbow_hit1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, RANDOM_FLOAT(90, 110));
			break;
		case 1:
			EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/xbow_hit2.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, RANDOM_FLOAT(90, 110));
			break;
		}

		SetThink(&CPitDroneSpit::SUB_Remove);
		SetNextThink(0);

		if (FClassnameIs(pOther->pev, "worldspawn")) {
			Vector vecDir = pev->velocity.Normalize();
			UTIL_SetOrigin(this, pev->origin - vecDir * 12);
			pev->angles = UTIL_VecToAngles(vecDir);
			pev->solid = SOLID_NOT;
			pev->movetype = MOVETYPE_FLY;
			pev->velocity = Vector(0, 0, 0);
			pev->avelocity.z = 0;
			pev->angles.z = RANDOM_LONG(0, 360);
			SetNextThink(10);
		}

		if (pOther->IsBSPModel()) {
			SetParent(pOther);//glue bolt with parent system
		}
	}
}

