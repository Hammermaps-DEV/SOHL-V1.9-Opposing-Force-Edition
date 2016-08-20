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
// NPC: AH-64 Apache * http://half-life.wikia.com/wiki/AH-64_Apache
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "particle_defs.h"
#include "monster_apache.h"

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS(monster_apache, CApache);

TYPEDESCRIPTION	CApache::m_SaveData[] = {
	DEFINE_FIELD( CApache, m_iRockets, FIELD_INTEGER ),
	DEFINE_FIELD( CApache, m_flForce, FIELD_FLOAT ),
	DEFINE_FIELD( CApache, m_flNextRocket, FIELD_TIME ),
	DEFINE_FIELD( CApache, m_vecTarget, FIELD_VECTOR ),
	DEFINE_FIELD( CApache, m_posTarget, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CApache, m_vecDesired, FIELD_VECTOR ),
	DEFINE_FIELD( CApache, m_posDesired, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CApache, m_vecGoal, FIELD_VECTOR ),
	DEFINE_FIELD( CApache, m_angGun, FIELD_VECTOR ),
	DEFINE_FIELD( CApache, m_flLastSeen, FIELD_TIME ),
	DEFINE_FIELD( CApache, m_flPrevSeen, FIELD_TIME ),
	DEFINE_FIELD( CApache, m_flGoalSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CApache, m_iDoSmokePuff, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CApache, CBaseMonster );

//=========================================================
// Monster Sounds
//=========================================================
const char *CApache::pRotorSounds[] = {
	"apache/ap_rotor1.wav",
	"apache/ap_rotor2.wav",
	"apache/ap_rotor3.wav",
	"apache/ap_rotor4.wav"
};

const char *CApache::pDebrisSounds[] = {
	"weapons/debris1.wav",
	"weapons/debris2.wav",
	"weapons/debris3.wav"
};

const char *CApache::pExplodeSounds[] = {
	"explosions/explode1.wav",
	"explosions/explode2.wav",
	"explosions/explode3.wav",
	"explosions/explode4.wav",
	"explosions/explode5.wav"
};

//=========================================================
// Spawn Apache
//=========================================================
void CApache::Spawn(void) {
	Precache( );

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/apache.mdl");

	UTIL_SetSize( pev, Vector( -32, -32, -64 ), Vector( 32, 32, 0 ) );
	UTIL_SetOrigin( this, pev->origin );

	// Motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	pev->flags |= FL_MONSTER | FL_FLY;
	pev->takedamage	= DAMAGE_AIM;

	if (pev->health == 0)
		pev->health	= gSkillData.apacheHealth;

	m_flFieldOfView = -0.707; // 270 degrees

	// Change Rotor Sound
	m_iRotorSound = 1;
	if (RANDOM_LONG(0, 100) >= 60) {
		m_iRotorSound = 3;
	}

	pev->sequence = 0;
	ResetSequenceInfo( );
	pev->frame = RANDOM_LONG(0, 0xFF);

	InitBoneControllers();

	if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_32) {
		SetUse(&CApache :: StartupUse );
	} else {
		SetThink(&CApache :: HuntThink );
		SetTouch(&CApache :: FlyTouch );
		SetNextThink( 1.0 );
	}

	m_flDebug = false;
	m_iRockets = 10;
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CApache::Classify(void) {
	return m_iClass ? m_iClass : CLASS_HUMAN_MILITARY;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CApache::Precache(void) {
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/apache.mdl");

	PRECACHE_MODEL("sprites/lgtning.spr");
	PRECACHE_MODEL("sprites/exp_end.spr");

	m_iSpriteTexture = PRECACHE_MODEL("sprites/white.spr");
	m_iExplode	= PRECACHE_MODEL( "sprites/fexplo.spr" );
	m_iBodyGibs = PRECACHE_MODEL( "models/metalplategibs_green.mdl" );

	PRECACHE_SOUND_ARRAY(pRotorSounds);
	PRECACHE_SOUND_ARRAY(pDebrisSounds);
	PRECACHE_SOUND_ARRAY(pExplodeSounds);

	PRECACHE_SOUND("apache/ap_whine1.wav");
	PRECACHE_SOUND("weapons/mortarhit.wav");
	PRECACHE_SOUND("apache/fire1.wav");
	PRECACHE_SOUND("weapons/rocketfire1.wav");

	UTIL_PrecacheOther( "hvr_rocket" );
}

//=========================================================
// NullThink
//=========================================================
void CApache::NullThink(void) {
	StudioFrameAdvance();
	SetNextThink(0.5);
}

//=========================================================
// StartupUse
//=========================================================
void CApache::StartupUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) {
	SetThink(&CApache::HuntThink);
	SetTouch(&CApache::FlyTouch);
	SetNextThink(0.1);
	SetUse(NULL);
}

//=========================================================
// Killed
//=========================================================
void CApache::Killed(entvars_t *pevAttacker, int iGib) {
	ALERT(at_aiconsole, "AI Trigger Fire Target\n");
	FireTargets(STRING(m_iszTriggerTarget), this, this, USE_TOGGLE, 0);

	pev->movetype = MOVETYPE_TOSS;
	pev->gravity = 0.3;

	STOP_SOUND(ENT(pev), CHAN_STATIC, pRotorSounds[m_iRotorSound]);
	STOP_SOUND(ENT(pev), CHAN_BODY, "apache/ap_whine1.wav");

	UTIL_SetSize(pev, Vector(-32, -32, -64), Vector(32, 32, 0));
	SetThink(&CApache::DyingThink);
	SetTouch(&CApache::CrashTouch);

	SetNextThink(0.1);
	pev->health = 0;
	pev->takedamage = DAMAGE_NO;

	if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_8) {
		m_flNextRocket = UTIL_GlobalTimeBase() + 4.0;
	} else {
		m_flNextRocket = UTIL_GlobalTimeBase() + 15.0;
	}
}

//=========================================================
// DyingThink
//=========================================================
void CApache::DyingThink(void) {
	StudioFrameAdvance();
	SetNextThink(0.1);

	pev->avelocity = pev->avelocity * 1.02;

	// still falling?
	if (m_flNextRocket > UTIL_GlobalTimeBase()) {
		PLAYBACK_EVENT_FULL(FEV_RELIABLE | FEV_GLOBAL, edict(), m_usEfx, 0.0, (float *)&pev->origin, (float *)&g_vecZero, pev->dmg, 0.0, 0, 0, 0, 0);

		if (CVAR_GET_FLOAT("cl_expdetail") != 0) {
			MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexFireball_0);
			WRITE_BYTE(1); // scale * 10 -50
			WRITE_BYTE(35); // framerate
			WRITE_BYTE(TE_EXPLFLAG_NONE);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexFireball_1);
			WRITE_BYTE(1); // scale * 10 -50
			WRITE_BYTE(35); // framerate 15
			WRITE_BYTE(TE_EXPLFLAG_NONE);
			MESSAGE_END();

			// create explosion particle system
			if (CVAR_GET_FLOAT("r_particles") != 0) {
				MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
					WRITE_SHORT(0);
					WRITE_BYTE(0);
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z);
					WRITE_COORD(0);
					WRITE_COORD(0);
					WRITE_COORD(0);
					WRITE_SHORT(iExplosionDefault);
				MESSAGE_END();

				MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
					WRITE_SHORT(0);
					WRITE_BYTE(0);
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z);
					WRITE_COORD(0);
					WRITE_COORD(0);
					WRITE_COORD(0);
					WRITE_SHORT(iDefaultFinalFire);
				MESSAGE_END();

				MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
					WRITE_SHORT(0);
					WRITE_BYTE(0);
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z);
					WRITE_COORD(0);
					WRITE_COORD(0);
					WRITE_COORD(0);
					WRITE_SHORT(iDefaultFinalSmoke);
				MESSAGE_END();
			}

			// Big Explosion
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_GLOWSPRITE);//Big Flare Effect
			WRITE_COORD(pev->origin.x); //where to make the sprite appear on x axis
			WRITE_COORD(pev->origin.y);//where to make the sprite appear on y axis
			WRITE_COORD(pev->origin.z);//where to make the sprite appear on zaxis
			WRITE_SHORT(g_sModelIndexFireballFlash); //Name of the sprite to use, as defined at begining of tut
			WRITE_BYTE(1); // scale
			WRITE_BYTE(30); // framerate 15
			WRITE_BYTE(80); // brightness
			MESSAGE_END();
		}

		// lots of smoke
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-150, 150));
		WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-150, 150));
		WRITE_COORD(pev->origin.z + RANDOM_FLOAT(-150, -50));
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(100); // scale * 10
		WRITE_BYTE(10); // framerate
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
			WRITE_SHORT(0);
			WRITE_BYTE(0);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( 0 );
			WRITE_COORD( 0 );
			WRITE_COORD( 0 );
			WRITE_SHORT(iDefaultSmoke);
		MESSAGE_END();

		Vector vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
		WRITE_BYTE(TE_BREAKMODEL);

		// position
		WRITE_COORD(vecSpot.x);
		WRITE_COORD(vecSpot.y);
		WRITE_COORD(vecSpot.z);

		// size
		WRITE_COORD(400);
		WRITE_COORD(400);
		WRITE_COORD(132);

		// velocity
		WRITE_COORD(pev->velocity.x);
		WRITE_COORD(pev->velocity.y);
		WRITE_COORD(pev->velocity.z);

		// randomization
		WRITE_BYTE(50);

		// Model
		WRITE_SHORT(m_iBodyGibs);	//model id#

									// # of shards
		WRITE_BYTE(4);	// let client decide

						// duration
		WRITE_BYTE(30);// 3.0 seconds

					   // flags
		WRITE_BYTE(BREAK_METAL);
		MESSAGE_END();

		EMIT_SOUND_ARRAY_DYN(CHAN_ITEM, pExplodeSounds);
		EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pDebrisSounds);

		UTIL_ScreenShake(pev->origin, 12.0, 100.0, 2.0, 1000);

		// don't stop it we touch a entity
		pev->flags &= ~FL_ONGROUND;
		SetNextThink(0.2);
		return;
	} else {
		Vector vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;

		// fireball
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
			WRITE_BYTE(TE_SPRITE);
			WRITE_COORD(vecSpot.x);
			WRITE_COORD(vecSpot.y);
			WRITE_COORD(vecSpot.z + 256);
			WRITE_SHORT(m_iExplode);
			WRITE_BYTE(120); // scale * 10
			WRITE_BYTE(255); // brightness
		MESSAGE_END();

		// big smoke
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(vecSpot.x);
			WRITE_COORD(vecSpot.y);
			WRITE_COORD(vecSpot.z + 512);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(250); // scale * 10
			WRITE_BYTE(5); // framerate
		MESSAGE_END();

		// blast circle
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_BEAMCYLINDER);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z + 2000); // reach damage radius over .2 seconds
			WRITE_SHORT(m_iSpriteTexture);
			WRITE_BYTE(0); // startframe
			WRITE_BYTE(0); // framerate
			WRITE_BYTE(4); // life
			WRITE_BYTE(32);  // width
			WRITE_BYTE(0);   // noise
			WRITE_BYTE(255);   // r, g, b
			WRITE_BYTE(255);   // r, g, b
			WRITE_BYTE(192);   // r, g, b
			WRITE_BYTE(128); // brightness
			WRITE_BYTE(0);		// speed
		MESSAGE_END();

		if (pev->flags & FL_ONGROUND) {
			CBaseEntity *pWreckage = Create("cycler_wreckage", pev->origin, pev->angles);
			UTIL_SetSize(pWreckage->pev, Vector(-200, -200, -128), Vector(200, 200, -32));
			pWreckage->pev->frame = pev->frame;
			pWreckage->pev->sequence = pev->sequence;
			pWreckage->pev->framerate = 0;
			pWreckage->pev->dmgtime = UTIL_GlobalTimeBase() + 5;

			EMIT_SOUND(ENT(pev), CHAN_STATIC, "weapons/mortarhit.wav", 1.0, 0.3);
			RadiusDamage(pev->origin, pev, pev, 400, CLASS_NONE, DMG_BLAST);
			UTIL_ScreenShake(pev->origin, 12.0, 100.0, 3.0, 5000);
		}

		// gibs
		vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
		WRITE_BYTE(TE_BREAKMODEL);

		// position
		WRITE_COORD(vecSpot.x);
		WRITE_COORD(vecSpot.y);
		WRITE_COORD(vecSpot.z + 64);

		// size
		WRITE_COORD(400);
		WRITE_COORD(400);
		WRITE_COORD(128);

		// velocity
		WRITE_COORD(0);
		WRITE_COORD(0);
		WRITE_COORD(200);

		// randomization
		WRITE_BYTE(30);

		// Model
		WRITE_SHORT(m_iBodyGibs);
		WRITE_BYTE(200);
		WRITE_BYTE(200);
		WRITE_BYTE(BREAK_METAL);
		MESSAGE_END();

		pev->deadflag = DEAD_DEAD;
		FCheckAITrigger();

		SetThink(&CApache::SUB_Remove);
		SetNextThink(0.1);
	}
}

//=========================================================
// FlyTouch
//=========================================================
void CApache::FlyTouch(CBaseEntity *pOther) {
	// bounce if we hit something solid
	if (pOther->pev->solid == SOLID_BSP) {
		TraceResult tr = UTIL_GetGlobalTrace();

		// UNDONE, do a real bounce
		pev->velocity = pev->velocity + tr.vecPlaneNormal * (pev->velocity.Length() + 200);
	}
}

//=========================================================
// CrashTouch
//=========================================================
void CApache::CrashTouch(CBaseEntity *pOther) {
	// only crash if we hit something solid
	if (pOther->pev->solid == SOLID_BSP) {
		SetTouch(NULL);
		m_flNextRocket = UTIL_GlobalTimeBase();
		SetNextThink(0);
	}
}

//=========================================================
// SetObjectCollisionBox
//=========================================================
void CApache::SetObjectCollisionBox(void) {
	pev->absmin = pev->origin + Vector(-300, -300, -172);
	pev->absmax = pev->origin + Vector(300, 300, 8);
}

//=========================================================
// HuntThink
//=========================================================
void CApache::HuntThink(void) {
	StudioFrameAdvance();
	SetNextThink(0.1);
	ShowDamage();

	CBaseEntity *pEntidad = NULL;
	Vector VecSrc = pev->origin + Vector(0, 0, 100);
	while ((pEntidad = UTIL_FindEntityInSphere(pEntidad, VecSrc, 256)) != NULL) {
		//UNDONE: Make a decent check? (I need to override the actual entity)
		if (!FClassnameIs(pEntidad->pev, "monster_apache")) {
			pEntidad->TakeDamage(pev, pev, 50, DMG_CRUSH);
			if (pEntidad->pev->movetype == MOVETYPE_STEP)
				pEntidad->pev->velocity.x = RANDOM_FLOAT(-10, 10);
		}
	}

	if (m_pGoalEnt == NULL && !FStringNull(pev->target)) {// this monster has a target
		m_pGoalEnt = UTIL_FindEntityByTargetname(NULL, STRING(pev->target));
		if (m_pGoalEnt) {
			m_posDesired = m_pGoalEnt->pev->origin;
			UTIL_MakeAimVectors(m_pGoalEnt->pev->angles);
			m_vecGoal = gpGlobals->v_forward;
		}
	}

	if (m_hEnemy == NULL) {
		Look(4092);
		m_hEnemy = BestVisibleEnemy();
	}

	// generic speed up
	if (m_flGoalSpeed < 800)
		m_flGoalSpeed += 5;

	if (m_hEnemy != NULL) {
		// ALERT( at_console, "%s\n", STRING( m_hEnemy->pev->classname ) );
		if (FVisible(m_hEnemy))	{
			if (m_flLastSeen < UTIL_GlobalTimeBase() - 5)
				m_flPrevSeen = UTIL_GlobalTimeBase();
			m_flLastSeen = UTIL_GlobalTimeBase();
			m_posTarget = m_hEnemy->Center();
		} else {
			m_hEnemy = NULL;
		}
	}

	m_vecTarget = (m_posTarget - pev->origin).Normalize();
	float flLength = (pev->origin - m_posDesired).Length();

	if (m_pGoalEnt) {
		// ALERT( at_console, "%.0f\n", flLength );
		if (flLength < 128) {
			m_pGoalEnt = UTIL_FindEntityByTargetname(NULL, STRING(m_pGoalEnt->pev->target));
			if (m_pGoalEnt)	{
				m_posDesired = m_pGoalEnt->pev->origin;
				UTIL_MakeAimVectors(m_pGoalEnt->pev->angles);
				m_vecGoal = gpGlobals->v_forward;
				flLength = (pev->origin - m_posDesired).Length();
			}
		}
	} else {
		m_posDesired = pev->origin;
	}

	if (flLength > 250) {
		if (m_flLastSeen + 90 > UTIL_GlobalTimeBase() && DotProduct((m_posTarget - pev->origin).Normalize(), (m_posDesired - pev->origin).Normalize()) > 0.25) {
			m_vecDesired = (m_posTarget - pev->origin).Normalize();
		} else {
			m_vecDesired = (m_posDesired - pev->origin).Normalize();
		}
	} else {
		m_vecDesired = m_vecGoal;
	}

	Flight();

	// ALERT( at_console, "%.0f %.0f %.0f\n", UTIL_GlobalTimeBase(), m_flLastSeen, m_flPrevSeen );
	if ((m_flLastSeen + 1 > UTIL_GlobalTimeBase()) && (m_flPrevSeen + 2 < UTIL_GlobalTimeBase())) {
		if (FireGun()) {
			// slow down if we're fireing
			if (m_flGoalSpeed > 400)
				m_flGoalSpeed = 400;
		}
	}

	UTIL_MakeAimVectors(pev->angles);
	Vector vecEst = (gpGlobals->v_forward * 800 + pev->velocity).Normalize();
	// ALERT( at_console, "%d %d %d %4.2f\n", pev->angles.x < 0, DotProduct( pev->velocity, gpGlobals->v_forward ) > -100, m_flNextRocket < UTIL_GlobalTimeBase(), DotProduct( m_vecTarget, vecEst ) );

	if ((m_iRockets % 2) == 1 && !(pev->spawnflags & SF_MONSTER_SPAWNFLAG_256)) {
		FireRocket();
		m_flNextRocket = UTIL_GlobalTimeBase() + 0.5;
		if (m_iRockets <= 0) {
			m_flNextRocket = UTIL_GlobalTimeBase() + 10;
			m_iRockets = 10;
		}
	} else if (pev->angles.x < 0 && DotProduct(pev->velocity, gpGlobals->v_forward) > -100 && m_flNextRocket < UTIL_GlobalTimeBase()) {
		if (m_flLastSeen + 60 > UTIL_GlobalTimeBase()) {
			if (m_hEnemy != NULL) {
				// make sure it's a good shot
				if (DotProduct(m_vecTarget, vecEst) > .965) {
					TraceResult tr;

					UTIL_TraceLine(pev->origin, pev->origin + vecEst * 4096, ignore_monsters, edict(), &tr);
					if ((tr.vecEndPos - m_posTarget).Length() < 512 && !(pev->spawnflags & SF_MONSTER_SPAWNFLAG_256))
						FireRocket();
				}
			} else {
				TraceResult tr;

				UTIL_TraceLine(pev->origin, pev->origin + vecEst * 4096, dont_ignore_monsters, edict(), &tr);
				// just fire when close
				if ((tr.vecEndPos - m_posTarget).Length() < 512 && !(pev->spawnflags & SF_MONSTER_SPAWNFLAG_256))
					FireRocket();
			}
		}
	}
}

//=========================================================
// Flight
//=========================================================
void CApache::Flight(void) {
	// tilt model 5 degrees
	Vector vecAdj = Vector(5.0, 0, 0);

	// estimate where I'll be facing in one seconds
	UTIL_MakeAimVectors(pev->angles + pev->avelocity * 2 + vecAdj);
	// Vector vecEst1 = pev->origin + pev->velocity + gpGlobals->v_up * m_flForce - Vector( 0, 0, 384 );
	// float flSide = DotProduct( m_posDesired - vecEst1, gpGlobals->v_right );

	float flSide = DotProduct(m_vecDesired, gpGlobals->v_right);
	if (flSide < 0) {
		if (pev->avelocity.y < 60) {
			pev->avelocity.y += 8; // 9 * (3.0/2.0);
		}
	} else {
		if (pev->avelocity.y > -60) {
			pev->avelocity.y -= 8; // 9 * (3.0/2.0);
		}
	}
	pev->avelocity.y *= 0.98;

	// estimate where I'll be in two seconds
	UTIL_MakeAimVectors(pev->angles + pev->avelocity * 1 + vecAdj);
	Vector vecEst = pev->origin + pev->velocity * 2.0 + gpGlobals->v_up * m_flForce * 20 - Vector(0, 0, 384 * 2);

	// add immediate force
	UTIL_MakeAimVectors(pev->angles + vecAdj);
	pev->velocity.x += gpGlobals->v_up.x * m_flForce;
	pev->velocity.y += gpGlobals->v_up.y * m_flForce;
	pev->velocity.z += gpGlobals->v_up.z * m_flForce;

	// add gravity
	pev->velocity.z -= 38.4; // 32ft/sec

	float flSpeed = pev->velocity.Length();
	float flDir = DotProduct(Vector(gpGlobals->v_forward.x, gpGlobals->v_forward.y, 0), Vector(pev->velocity.x, pev->velocity.y, 0));
	if (flDir < 0)
		flSpeed = -flSpeed;

	float flDist = DotProduct(m_posDesired - vecEst, gpGlobals->v_forward);

	// float flSlip = DotProduct( pev->velocity, gpGlobals->v_right );
	float flSlip = -DotProduct(m_posDesired - vecEst, gpGlobals->v_right);

	// fly sideways
	if (flSlip > 0) {
		if (pev->angles.z > -30 && pev->avelocity.z > -15)
			pev->avelocity.z -= 4;
		else
			pev->avelocity.z += 2;
	} else {
		if (pev->angles.z < 30 && pev->avelocity.z < 15)
			pev->avelocity.z += 4;
		else
			pev->avelocity.z -= 2;
	}

	// sideways drag
	pev->velocity.x = pev->velocity.x * (1.0 - V_fabs(gpGlobals->v_right.x) * 0.05);
	pev->velocity.y = pev->velocity.y * (1.0 - V_fabs(gpGlobals->v_right.y) * 0.05);
	pev->velocity.z = pev->velocity.z * (1.0 - V_fabs(gpGlobals->v_right.z) * 0.05);

	// general drag
	pev->velocity = pev->velocity * 0.995;

	// apply power to stay correct height
	if (m_flForce < 80 && vecEst.z < m_posDesired.z) {
		m_flForce += 12;
	} else if (m_flForce > 30) {
		if (vecEst.z > m_posDesired.z)
			m_flForce -= 8;
	}

	// pitch forward or back to get to target
	if (flDist > 0 && flSpeed < m_flGoalSpeed /* && flSpeed < flDist */ && pev->angles.x + pev->avelocity.x > -40)
	{
		// ALERT( at_console, "F " );
		// lean forward
		pev->avelocity.x -= 12.0;
	} else if (flDist < 0 && flSpeed > -50 && pev->angles.x + pev->avelocity.x  < 20) {
		// ALERT( at_console, "B " );
		// lean backward
		pev->avelocity.x += 12.0;
	} else if (pev->angles.x + pev->avelocity.x > 0) {
		// ALERT( at_console, "f " );
		pev->avelocity.x -= 4.0;
	} else if (pev->angles.x + pev->avelocity.x < 0) {
		// ALERT( at_console, "b " );
		pev->avelocity.x += 4.0;
	}

	// make rotor, engine sounds
	if (m_iSoundState == 0) {
		EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, pRotorSounds[m_iRotorSound], 1.0, 0.3, 0, 110);
		EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "apache/ap_whine1.wav", 0.1, 0.2, 0, 110);
		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions
	} else {
		CBaseEntity *pPlayer = NULL;
		pPlayer = UTIL_FindEntityByClassname(NULL, "player");
		float pitch = DotProduct(pev->velocity - pPlayer->pev->velocity, (pPlayer->pev->origin - pev->origin).Normalize());
		pitch = (int)(100 + pitch / 50.0);
		if (pitch > 250)
			pitch = 250;
		else if (pitch < 50)
			pitch = 50;
		else if (pitch == 100)
			pitch = 101;

		float flVol = (m_flForce / 100.0) + .1;
		if (flVol > 1.0) 
			flVol = 1.0;

		EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, pRotorSounds[m_iRotorSound], flVol, 0.3, m_iSoundState | SND_CHANGE_VOL, pitch);
		EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "apache/ap_whine1.wav", 0.1, 0.2, m_iSoundState, pitch);
	}
}

//=========================================================
// FireRocket
//=========================================================
void CApache::FireRocket(void) {
	static float side = 1.0;
	static int count;

	if (m_iRockets <= 0)
		return;

	UTIL_MakeAimVectors(pev->angles);
	Vector vecSrc = pev->origin + 1.5 * (gpGlobals->v_forward * 21 + gpGlobals->v_right * 70 * side + gpGlobals->v_up * -79);

	switch (m_iRockets % 5) {
		case 0:	vecSrc = vecSrc + gpGlobals->v_right * 10; break;
		case 1: vecSrc = vecSrc - gpGlobals->v_right * 10; break;
		case 2: vecSrc = vecSrc + gpGlobals->v_up * 10; break;
		case 3: vecSrc = vecSrc - gpGlobals->v_up * 10; break;
		case 4: break;
	}

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(20); // scale * 10
		WRITE_BYTE(12); // framerate
	MESSAGE_END();

	CSprite *pSprite = CSprite::SpriteCreate("sprites/exp_end.spr", vecSrc, TRUE);
	pSprite->AnimateAndDie(RANDOM_FLOAT(8.0, 12.0));//framerate
	pSprite->SetTransparency(kRenderTransAlpha, 50, 50, 50, 255, kRenderFxNone);
	pSprite->SetScale(5.0);
	pSprite->Expand(RANDOM_FLOAT(5, 10), RANDOM_FLOAT(50.0, 20.0));//expand
	pSprite->pev->frame = 0;

	pSprite->pev->avelocity.x = RANDOM_FLOAT(-50, 50);//rotar?
	pSprite->pev->avelocity.y = RANDOM_FLOAT(-50, 50);
	pSprite->pev->avelocity.z = RANDOM_FLOAT(-50, 50);

	EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/rocketfire1.wav", 1.0, 0.3, 0, 100);
	CBaseEntity *pRocket = CBaseEntity::Create("hvr_rocket", vecSrc, pev->angles, edict());
	if (pRocket)
		pRocket->pev->velocity = pev->velocity + gpGlobals->v_forward * 150;

	m_iRockets--;
	side = -side;
}

//=========================================================
// FireGun
//=========================================================
BOOL CApache::FireGun() {
	UTIL_MakeAimVectors(pev->angles);

	Vector posGun, angGun;
	GetAttachment(1, posGun, angGun);

	Vector vecTarget = (m_posTarget - posGun).Normalize();

	Vector vecOut;

	vecOut.x = DotProduct(gpGlobals->v_forward, vecTarget);
	vecOut.y = -DotProduct(gpGlobals->v_right, vecTarget);
	vecOut.z = DotProduct(gpGlobals->v_up, vecTarget);

	Vector angles = UTIL_VecToAngles(vecOut);

	angles.x = -angles.x;
	if (angles.y > 180)
		angles.y = angles.y - 360;
	if (angles.y < -180)
		angles.y = angles.y + 360;
	if (angles.x > 180)
		angles.x = angles.x - 360;
	if (angles.x < -180)
		angles.x = angles.x + 360;

	if (angles.x > m_angGun.x)
		m_angGun.x = V_min(angles.x, m_angGun.x + 12);
	if (angles.x < m_angGun.x)
		m_angGun.x = V_max(angles.x, m_angGun.x - 12);
	if (angles.y > m_angGun.y)
		m_angGun.y = V_min(angles.y, m_angGun.y + 12);
	if (angles.y < m_angGun.y)
		m_angGun.y = V_max(angles.y, m_angGun.y - 12);

	m_angGun.y = SetBoneController(0, m_angGun.y);
	m_angGun.x = SetBoneController(1, m_angGun.x);

	Vector posBarrel, angBarrel;
	GetAttachment(0, posBarrel, angBarrel);
	Vector vecGun = (posBarrel - posGun).Normalize();

	if (DotProduct(vecGun, vecTarget) > 0.98) {
		FireBullets(1, posGun, vecGun, VECTOR_CONE_4DEGREES, 8192, BULLET_MONSTER_12MM, 1);
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "apache/fire1.wav", 1, 0.3);
		return TRUE;
	}

	return FALSE;
}

//=========================================================
// ShowDamage
//=========================================================
void CApache::ShowDamage(void) {
	if (m_iDoSmokePuff > 0 || RANDOM_LONG(0, 99) > pev->health) {
		if (CVAR_GET_FLOAT("r_particles") != 0) {
			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
				WRITE_SHORT(0);
				WRITE_BYTE(0);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_SHORT(iDefaultFinalFire);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
				WRITE_SHORT(0);
				WRITE_BYTE(0);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_SHORT(iDefaultFinalSmoke);
			MESSAGE_END();
		}

		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z - 32);
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(RANDOM_LONG(0, 9) + 20); // scale * 10
		WRITE_BYTE(12); // framerate
		MESSAGE_END();
	}

	if (m_iDoSmokePuff > 0)
		m_iDoSmokePuff--;
}

//=========================================================
// TakeDamage
//=========================================================
int CApache::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) {
	if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_64) {
		if (m_flDebug)
			ALERT(at_console, "%s:TakeDamage:SF_MONSTER_SPAWNFLAG_64\n", STRING(pev->classname));

		CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
		if (pEnt->IsPlayer()) {
			pev->health = pev->max_health / 2;
			if (flDamage > 0) //Override all damage
				SetConditions(bits_COND_LIGHT_DAMAGE);

			if (flDamage >= 20) //Override all damage
				SetConditions(bits_COND_HEAVY_DAMAGE);

			return CBaseEntity::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
		}

		if (pevAttacker->owner) {
			pEnt = CBaseEntity::Instance(pevAttacker->owner);
			if (pEnt->IsPlayer()) {
				pev->health = pev->max_health / 2;
				if (flDamage > 0) //Override all damage
					SetConditions(bits_COND_LIGHT_DAMAGE);

				if (flDamage >= 20) //Override all damage
					SetConditions(bits_COND_HEAVY_DAMAGE);

				return CBaseEntity::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
			}
		}
	}

	if (!IsAlive() || pev->deadflag == DEAD_DYING || m_iPlayerReact) {
		return CBaseEntity::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	}

	if (pevInflictor->owner == edict())
		return 0;

	if (bitsDamageType & DMG_BLAST) {
		flDamage *= 2;
	}

	// ALERT( at_console, "%.0f\n", flDamage );
	return CBaseEntity::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

//=========================================================
// TraceAttack
//=========================================================
void CApache::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CBaseEntity::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
		return;
	}

	if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_64) {
		CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
		if (pEnt->IsPlayer()) { return; }
		if (pevAttacker->owner) {
			pEnt = CBaseEntity::Instance(pevAttacker->owner);
			if (pEnt->IsPlayer()) { return; }
		}
	}

	// ignore blades
	if (ptr->iHitgroup == 6 && (bitsDamageType & (DMG_ENERGYBEAM | DMG_BULLET | DMG_CLUB)))
		return;

	// hit hard, hits cockpit, hits engines
	if (flDamage > 50 || ptr->iHitgroup == HITGROUP_HEAD 
		|| ptr->iHitgroup == HITGROUP_CHEST) {
		AddMultiDamage(pevAttacker, this, flDamage, bitsDamageType);
		m_iDoSmokePuff = 3 + (flDamage / 5.0);
	} else {
		UTIL_Ricochet(ptr->vecEndPos, 2.0);
	}
}