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
*	Base Source-Code written by Raven City and Marc-Antoine Lortie (https://github.com/malortie).
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/

//=========================================================
// Projectile: Energy Ball for Voltigore
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Build: 00001 / Date: 12.11.2015
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"effects.h"
#include	"skill.h"
#include	"decals.h"
#include    "weapons.h"
#include    "proj_voltigore_energy_ball.h"

//=========================================================
// Voltigore's energy ball projectile
//=========================================================
LINK_ENTITY_TO_CLASS(voltigore_energy_ball, CVoltigoreEnergyBall);

TYPEDESCRIPTION	CVoltigoreEnergyBall::m_SaveData[] = {
	DEFINE_FIELD(CVoltigoreEnergyBall, m_maxFrame, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CVoltigoreEnergyBall, CBaseEntity);

//=========================================================
// Purpose:
//=========================================================
void CVoltigoreEnergyBall::Spawn(void) {
	Precache();

	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING("voltigore_energy_ball");

	pev->solid = SOLID_BBOX;
	pev->rendermode = kRenderGlow;
	pev->renderamt = 220;

	SET_MODEL(ENT(pev), "sprites/glow_prp.spr");
	pev->frame = 0;
	pev->scale = 0.08;
	pev->dmg = gSkillData.voltigoreDmgBeam;

	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex) - 1;

	Glow(); //Start Glow

	SetThink(&CVoltigoreEnergyBall::Think);
	SetTouch(&CVoltigoreEnergyBall::Touch);
}

void CVoltigoreEnergyBall::Precache(void) {
	PRECACHE_MODEL("sprites/blueflare2.spr");
	PRECACHE_MODEL("sprites/glow_prp.spr");
	PRECACHE_MODEL("sprites/plasma.spr");
	PRECACHE_SOUND("weapons/displacer_impact.wav");
}

void CVoltigoreEnergyBall::Animate(void) {
	SetNextThink(0.1);
	if (pev->frame++) {
		if (pev->frame > m_maxFrame) {
			pev->frame = 0;
		}
	}
}

//=========================================================
// Purpose:
//=========================================================
void CVoltigoreEnergyBall::Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity) {
	CVoltigoreEnergyBall *pEnergyBall = GetClassPtr((CVoltigoreEnergyBall *)NULL);
	pEnergyBall->Spawn();

	UTIL_SetOrigin(pEnergyBall, vecStart);
	pEnergyBall->pev->velocity = vecVelocity;
	pEnergyBall->pev->owner = ENT(pevOwner);

	pEnergyBall->SetThink(&CVoltigoreEnergyBall::Animate);
	pEnergyBall->SetNextThink(0.1);
}

//=========================================================
// Purpose:
//=========================================================
void CVoltigoreEnergyBall::Touch(CBaseEntity *pOther) {
	TraceResult tr;
	Vector vecSpot;

	pev->enemy = pOther->edict();

	vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine(vecSpot, vecSpot + pev->velocity.Normalize() * 64, dont_ignore_monsters, ENT(pev), &tr);

	if (m_pSprite) {
		UTIL_Remove(m_pSprite);
		m_pSprite = NULL;
	}

	if (UTIL_PointContents(pev->origin) == CONTENT_SKY) {
		UTIL_Remove(this);
		return;
	}

	entvars_t *pevOwner;
	if (pev->owner)
		pevOwner = VARS(pev->owner);
	else
		pevOwner = NULL;

	if (!pOther->pev->takedamage) {
		// make a splat on the wall
		UTIL_TraceLine(pev->origin, pev->origin + pev->velocity * 10, dont_ignore_monsters, ENT(pev), &tr);
		UTIL_DecalTrace(&tr, DECAL_SCORCH1 + RANDOM_LONG(0, 2));
		::RadiusDamage(pev->origin, pev, pevOwner, pev->dmg, 32, CLASS_ALIEN_BIOWEAPON, DMG_ENERGYBEAM | DMG_SHOCK);
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/displacer_impact.wav", 1, ATTN_NORM);
		UTIL_Sparks(pev->origin);
	} else {
		pOther->TakeDamage(pev, pev, pev->dmg, DMG_ENERGYBEAM | DMG_SHOCK);
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/displacer_impact.wav", VOL_NORM, ATTN_NORM);
	}

	UTIL_Remove(this);
	SetNextThink(0);
}

void CVoltigoreEnergyBall::Think(void) {
	CBeam *pBeam;
	TraceResult tr;
	float flDist = 1.0;

	for (int i = 0; i < 10; i++) {
		Vector vecDir = Vector(RANDOM_FLOAT(-1.0, 1.0), RANDOM_FLOAT(-1.0, 1.0), RANDOM_FLOAT(-1.0, 1.0));
		vecDir = vecDir.Normalize();
		TraceResult tr1;
		UTIL_TraceLine(pev->origin, pev->origin + vecDir * 1024, ignore_monsters, ENT(pev), &tr1);
		if (flDist > tr1.flFraction) {
			tr = tr1;
			flDist = tr.flFraction;
		}
	}

	if (flDist == 1.0) return;
	pBeam = CBeam::BeamCreate("sprites/lgtning.spr", 10);
	pBeam->PointEntInit(tr.vecEndPos, entindex());
	pBeam->SetStartPos(tr.vecEndPos);
	pBeam->SetEndEntity(entindex());
	pBeam->SetColor(125, 61, 177);
	pBeam->SetNoise(70);
	pBeam->SetBrightness(200);
	pBeam->SetWidth(30);
	pBeam->SetScrollRate(35);
	pBeam->LiveForTime(0.3);

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(pev->origin.x);	// X
		WRITE_COORD(pev->origin.y);	// Y
		WRITE_COORD(pev->origin.z);	// Z
		WRITE_BYTE(25);     // radius
		WRITE_BYTE(125);     // R
		WRITE_BYTE(61);     // G
		WRITE_BYTE(177);     // B
		WRITE_BYTE(1);     // life * 10
		WRITE_BYTE(0); // decay
	MESSAGE_END();

	if (UTIL_PointContents(pev->origin) == CONTENT_WATER) { //Impact on Water
		entvars_t *pevOwner = VARS(pev->owner);
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/displacer_impact.wav", VOL_NORM, ATTN_NORM);
		::RadiusDamage(pev->origin, pev, pevOwner, pev->dmg, pev->dmg, CLASS_NONE, DMG_ENERGYBEAM | DMG_SHOCK);
		UTIL_Remove(this);
		return;
	}

	SetNextThink(0.05);
}

void CVoltigoreEnergyBall::Glow(void) {
	if(m_pSprite) {
		UTIL_Remove(m_pSprite);
		m_pSprite = NULL;
	}

	m_pSprite = CSprite::SpriteCreate("sprites/blueflare1.spr", pev->origin, FALSE);
	m_pSprite->SetAttachment(edict(), 0);
	m_pSprite->pev->scale = 1;
	m_pSprite->SetTransparency(kRenderGlow, 125, 61, 177, 255, kRenderFxNoDissipation);
	m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;
	m_pSprite->pev->flags |= FL_SKIPLOCALHOST;
}
