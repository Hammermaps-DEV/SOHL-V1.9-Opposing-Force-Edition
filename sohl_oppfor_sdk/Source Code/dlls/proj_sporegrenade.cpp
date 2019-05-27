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
// Projectile: Spore Grenade for Shocktrooper & Spore Launcher
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "decals.h"
#include "weapons.h"
#include "proj_sporegrenade.h"
#include "particle_defs.h"

extern int gmsgParticles;

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(spore, CSporeGrenade);

//=========================================================
// ShootTimed
//=========================================================
CSporeGrenade *CSporeGrenade::ShootTimed(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time) {
	CSporeGrenade *pSpore = GetClassPtr((CSporeGrenade *)NULL);
	UTIL_SetOrigin(pSpore, vecStart);
	pSpore->pev->movetype = MOVETYPE_BOUNCE;
	pSpore->pev->owner = ENT(pevOwner);
	pSpore->pev->classname = MAKE_STRING("spore");
	pSpore->pev->velocity = vecVelocity;
	pSpore->pev->velocity = pSpore->pev->velocity + gpGlobals->v_forward * 700;
	pSpore->pev->angles = UTIL_VecToAngles(pSpore->pev->velocity);
	pSpore->pev->dmgtime = (time <= 0 ? 2.5 : time);
	pSpore->pev->sequence = RANDOM_LONG(3, 6);
	pSpore->pev->framerate = 1.0;
	pSpore->Spawn();
	return pSpore;
}

//=========================================================
// ShootContact
//=========================================================
CSporeGrenade *CSporeGrenade::ShootContact(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity) {
	CSporeGrenade *pSpore = GetClassPtr((CSporeGrenade *)NULL);
	UTIL_SetOrigin(pSpore, vecStart);
	pSpore->pev->movetype = MOVETYPE_FLY;
	pSpore->pev->owner = ENT(pevOwner);
	pSpore->pev->classname = MAKE_STRING("spore");
	pSpore->pev->velocity = vecVelocity;
	pSpore->pev->velocity = pSpore->pev->velocity + gpGlobals->v_forward * 1500;
	pSpore->pev->angles = UTIL_VecToAngles(pSpore->pev->velocity);
	pSpore->Spawn();
	return pSpore;
}

//=========================================================
// Spawn
//=========================================================
void CSporeGrenade::Spawn() {
	Precache();

	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/spore.mdl");
	UTIL_SetSize(pev, Vector(-4, -4, -4), Vector(4, 4, 4));

	UTIL_SetOrigin(this, pev->origin);
	UTIL_MakeVectors(pev->angles);

	pev->gravity = 0.5;
	pev->friction = 0.2;
	Glow();

	if (pev->movetype == MOVETYPE_FLY) {
		SetThink(&CSporeGrenade::FlyThink);
		SetTouch(&CSporeGrenade::ExplodeThink);
	}
	else {
		SetThink(&CSporeGrenade::FlyThink);
		SetTouch(&CSporeGrenade::BounceThink);
	}

	pev->dmgtime = (UTIL_GlobalTimeBase() + pev->dmgtime);
	pev->dmg = gSkillData.plrDmgSpore;
	SetNextThink(0.1);
}

//=========================================================
// Precache
//=========================================================
void CSporeGrenade::Precache() {
	PRECACHE_MODEL("models/spore.mdl");

	PRECACHE_MODEL("sprites/glow02.spr");

	PRECACHE_SOUND("weapons/splauncher_impact.wav");

	PRECACHE_SOUND("weapons/spore_hit1.wav");
	PRECACHE_SOUND("weapons/spore_hit2.wav");
	PRECACHE_SOUND("weapons/spore_hit3.wav");
}

//=========================================================
// Glow
//=========================================================
void CSporeGrenade::Glow() {
	if (!m_pSprite) {
		m_pSprite = CSprite::SpriteCreate("sprites/glow02.spr", pev->origin, FALSE);
		m_pSprite->SetAttachment(edict(), 0);
		m_pSprite->pev->scale = 0.75;
		m_pSprite->SetTransparency(kRenderTransAdd, 150, 158, 19, 155, kRenderFxNoDissipation);
		m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;
		m_pSprite->pev->flags |= FL_SKIPLOCALHOST;
	}
}

//=========================================================
// FlyThink
//=========================================================
void CSporeGrenade::FlyThink() {
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + pev->velocity * 10, dont_ignore_monsters, ENT(pev), &tr);
	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_SPRITE_SPRAY);
	WRITE_COORD(pev->origin.x + RANDOM_LONG(-5, 5));
	WRITE_COORD(pev->origin.y + RANDOM_LONG(-5, 5));
	WRITE_COORD(pev->origin.z + RANDOM_LONG(-5, 5));
	WRITE_COORD(tr.vecPlaneNormal.Normalize().x);
	WRITE_COORD(tr.vecPlaneNormal.Normalize().y);
	WRITE_COORD(tr.vecPlaneNormal.Normalize().z);
	WRITE_SHORT(g_sModelIndexTinySpit);
	WRITE_BYTE(RANDOM_LONG(3, 8)); // count
	WRITE_BYTE(RANDOM_FLOAT(10, 15)); // speed
	WRITE_BYTE(RANDOM_FLOAT(4, 9) * 100);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(pev->origin.x);	// X
	WRITE_COORD(pev->origin.y);	// Y
	WRITE_COORD(pev->origin.z);	// Z
	WRITE_BYTE(15);     // radius
	WRITE_BYTE(0);		// r
	WRITE_BYTE(180);	// g
	WRITE_BYTE(0);	// b
	WRITE_BYTE(1);     // life * 10
	WRITE_BYTE(0); // decay
	MESSAGE_END();

	if (pev->movetype == MOVETYPE_BOUNCE) {
		if (pev->dmgtime <= UTIL_GlobalTimeBase())
			Explode();
	}

	SetNextThink(0.001);
}

//=========================================================
// BounceThink
//=========================================================
void CSporeGrenade::BounceThink(CBaseEntity *pOther) {
	if (pOther->pev->flags & FL_MONSTER || pOther->IsPlayer()) {
		Explode();
	}

	if (UTIL_PointContents(pev->origin) == CONTENT_SKY) {
		if (m_pSprite) {
			UTIL_Remove(m_pSprite);
			m_pSprite = NULL;
		}
		UTIL_Remove(this);
		return;
	}

	// only do damage if we're moving fairly fast
	if (m_flNextAttack < UTIL_GlobalTimeBase() && pev->velocity.Length() > 100) {
		entvars_t *pevOwner = VARS(pev->owner);
		if (pevOwner) {
			TraceResult tr = UTIL_GetGlobalTrace();
			ClearMultiDamage();
			pOther->TraceAttack(pevOwner, 1, gpGlobals->v_forward, &tr, DMG_GENERIC);
			ApplyMultiDamage(pev, pevOwner);
		}
		m_flNextAttack = UTIL_GlobalTimeBase() + 1.0;
	}

	Vector vecTestVelocity;
	vecTestVelocity = pev->velocity;
	vecTestVelocity.z *= 0.45;

	if (pev->flags & FL_ONGROUND) {
		pev->velocity = pev->velocity * 0.8;
		pev->sequence = RANDOM_LONG(1, 1);
	}
	else {
		// play bounce sound
		switch (RANDOM_LONG(0, 2)) {
		case 0: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/spore_hit1.wav", 0.25, ATTN_NORM); break;
		case 1: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/spore_hit2.wav", 0.25, ATTN_NORM); break;
		case 2: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/spore_hit3.wav", 0.25, ATTN_NORM); break;
		}
	}

	pev->framerate = pev->velocity.Length() / 200.0;
	if (pev->framerate > 1.0)
		pev->framerate = 1;
	else if (pev->framerate < 0.5)
		pev->framerate = 0;

}

//=========================================================
// ExplodeThink
//=========================================================
void CSporeGrenade::ExplodeThink(CBaseEntity *pOther) {
	if (UTIL_PointContents(pev->origin) == CONTENT_SKY) {
		if (m_pSprite) {
			UTIL_Remove(m_pSprite);
			m_pSprite = NULL;
		}
		UTIL_Remove(this);
		return;
	}

	Explode();
}

//=========================================================
// Explode
//=========================================================
void CSporeGrenade::Explode() {
	SetTouch(NULL);
	SetThink(NULL);
	EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/splauncher_impact.wav", VOL_NORM, ATTN_NORM);

	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_SPRITE);		// This makes a dynamic light and the explosion sprites/sound
	WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	switch (RANDOM_LONG(0, 1)) {
	case 0:
		WRITE_SHORT(g_sModelIndexSpore1);
		break;
	default:
	case 1:
		WRITE_SHORT(g_sModelIndexSpore3);
		break;
	}
	WRITE_BYTE(25); // scale * 10
	WRITE_BYTE(155); // framerate
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_EXPLOSION);		// This makes a dynamic light and the explosion sprites/sound
	WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(g_sModelIndexSpore1);
	WRITE_BYTE((pev->dmg - 50) * .60); // scale * 10
	WRITE_BYTE(15); // framerate
	WRITE_BYTE(TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + pev->velocity * 15, dont_ignore_monsters, ENT(pev), &tr);

	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_SPRITE_SPRAY);		// This makes a dynamic light and the explosion sprites/sound
	WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(tr.vecPlaneNormal.x);
	WRITE_COORD(tr.vecPlaneNormal.y);
	WRITE_COORD(tr.vecPlaneNormal.z);
	WRITE_SHORT(g_sModelIndexTinySpit);
	WRITE_BYTE(50); // count
	WRITE_BYTE(30); // speed
	WRITE_BYTE(640);
	MESSAGE_END();

	if (CVAR_GET_FLOAT("r_particles")) {
		MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
		WRITE_SHORT(0);
		WRITE_BYTE(0);
		WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(0);
		WRITE_COORD(0);
		WRITE_COORD(0);
		WRITE_SHORT(iDefaultSporeExplosion);
		MESSAGE_END();
	}

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(pev->origin.x);	// X
	WRITE_COORD(pev->origin.y);	// Y
	WRITE_COORD(pev->origin.z);	// Z
	WRITE_BYTE(20);		// radius * 0.1
	WRITE_BYTE(0);		// r
	WRITE_BYTE(180);		// g
	WRITE_BYTE(0);		// b
	WRITE_BYTE(20);		// time * 10
	WRITE_BYTE(20);		// decay * 0.1
	MESSAGE_END();

	entvars_t *pevOwner;
	if (pev->owner)
		pevOwner = VARS(pev->owner);
	else
		pevOwner = NULL;

	::RadiusDamage(pev->origin, pev, pevOwner, pev->dmg, 128, CLASS_PLAYER_BIOWEAPON, DMG_GENERIC);

	if (pev->movetype == MOVETYPE_FLY) {
		TraceResult tr;
		UTIL_TraceLine(pev->origin, pev->origin + pev->velocity * 10, dont_ignore_monsters, ENT(pev), &tr);
		UTIL_DecalTrace(&tr, DECAL_SPORESPLAT1 + RANDOM_LONG(0, 2));
	}

	pev->velocity = g_vecZero;

	if (m_pSprite) {
		UTIL_Remove(m_pSprite);
		m_pSprite = NULL;
	}

	SetNextThink(0.3);
	UTIL_Remove(this);
}