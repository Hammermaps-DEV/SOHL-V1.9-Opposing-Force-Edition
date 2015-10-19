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
#include    "weapons.h"
#include    "skill.h"
#include	"proj_pitdronespit.h"

//=========================================================
// CPitDrone's spit projectile
//=========================================================
LINK_ENTITY_TO_CLASS(pitdronespit, CPitDroneSpit);

CPitDroneSpit *CPitDroneSpit::SpitCreate(void) {
	// Create a new entity with CShock private data
	CPitDroneSpit *pSpit = GetClassPtr((CPitDroneSpit *)NULL);
	pSpit->pev->classname = MAKE_STRING("pitdronespit");
	pSpit->Spawn();
	return pSpit;
}

void CPitDroneSpit::Spawn(void) {
	Precache();
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/pit_drone_spike.mdl");

	UTIL_SetOrigin(this, pev->origin);
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	SetTouch(&CPitDroneSpit::SpitTouch);
	SetThink(&CPitDroneSpit::BubbleThink);
	SetNextThink(0.2);
}

void CPitDroneSpit::Precache(void) {
	PRECACHE_MODEL("models/pit_drone_spike.mdl");
	PRECACHE_SOUND("weapons/xbow_hitbod1.wav");
	PRECACHE_SOUND("weapons/xbow_hitbod2.wav");
	PRECACHE_SOUND("weapons/xbow_fly1.wav");
	PRECACHE_SOUND("weapons/xbow_hit1.wav");
	PRECACHE_SOUND("weapons/xbow_hit2.wav");
}

void CPitDroneSpit::SpitTouch(CBaseEntity *pOther) {
	SetTouch(NULL);
	SetThink(NULL);
	if (pOther->pev->takedamage) {
		TraceResult tr = UTIL_GetGlobalTrace();
		entvars_t	*pevOwner;

		pevOwner = VARS(pev->owner);

		// UNDONE: this needs to call TraceAttack instead
		ClearMultiDamage();
		pOther->TraceAttack(pevOwner, gSkillData.pitdroneDmgSpit, pev->velocity.Normalize(), &tr, DMG_GENERIC | DMG_NEVERGIB);
		ApplyMultiDamage(pev, pevOwner);

		pev->velocity = Vector(0, 0, 0);
		switch (RANDOM_LONG(0, 1)) {
			case 0:
				EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/xbow_hitbod1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, RANDOM_FLOAT(90, 110));
			break;
			case 1:
				EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/xbow_hitbod2.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, RANDOM_FLOAT(90, 110));
			break;
		}

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
		SetNextThink(10);// this will get changed below if the bolt is allowed to stick in what it hit.

		// if what we hit is static architecture, can stay around for a while.
		Vector vecDir = pev->velocity.Normalize();
		UTIL_SetOrigin(this, pev->origin - vecDir * 12);
		pev->angles = UTIL_VecToAngles(vecDir);
		pev->solid = SOLID_NOT;
		pev->movetype = MOVETYPE_FLY;
		pev->velocity = Vector(0, 0, 0);
		pev->avelocity.z = 0;
		pev->angles.z = RANDOM_LONG(0, 360);

		if (pOther->IsBSPModel()) {
			SetParent(pOther);//glue bolt with parent system
		}
	}
}

void CPitDroneSpit::BubbleThink(void) {
	SetNextThink(0.1);
	if (pev->waterlevel == 0 || pev->watertype <= CONTENT_FLYFIELD)
		return;

	UTIL_BubbleTrail(pev->origin - pev->velocity * 0.1, pev->origin, 1);
}