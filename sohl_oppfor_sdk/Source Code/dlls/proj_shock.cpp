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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "gamerules.h"
#include "customentity.h"
#include "decals.h"
#include "proj_shock.h"

#define SHOCK_BEAM_LENGTH		50
#define SHOCK_BEAM_LENGTH_HALF	SHOCK_BEAM_LENGTH * 0.5f
#define SHOCK_BEAM_WIDTH		50

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(shock, CShock);

TYPEDESCRIPTION	CShock::m_SaveData[] = {
	DEFINE_ARRAY(CShock, m_pBeam, FIELD_CLASSPTR, 1),
	DEFINE_FIELD(CShock, m_vecBeamStart, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(CShock, m_vecBeamEnd, FIELD_POSITION_VECTOR),
};

IMPLEMENT_SAVERESTORE(CShock, CBaseEntity);

void CShock::Spawn(void) {
	Precache();
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING("shock");

	pev->solid = SOLID_BBOX;
	pev->rendermode = kRenderTransAlpha;
	pev->renderamt = 0;

	SET_MODEL(ENT(pev), "sprites/bigspit.spr");
	pev->frame = 0;
	pev->scale = 0.5;
	pev->dmg = gSkillData.plrDmgShock;

	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	Glow();

	// Make beam NULL to avoid assertions.
	m_pBeam = 0;

	Vector vDir = pev->velocity.Normalize();
	ComputeBeamPositions(vDir, &m_vecBeamStart, &m_vecBeamEnd);

	SetThink(&CShock::ShockThink);
	SetNextThink(0.1);
}

void CShock::Precache() {
	PRECACHE_MODEL("sprites/lgtning.spr");
	PRECACHE_MODEL("sprites/bigspit.spr");
	PRECACHE_MODEL("sprites/lgtning.spr");
	PRECACHE_MODEL("sprites/glow03.spr");

	PRECACHE_SOUND("weapons/shock_impact.wav");
}

void CShock::ShockThink(void) {
	SetNextThink(0.01);

	Vector vDir = pev->velocity.Normalize();
	ComputeBeamPositions(vDir, &m_vecBeamStart, &m_vecBeamEnd);

	// Update the beam.
	UpdateBeam(m_vecBeamStart, m_vecBeamEnd);
}

void CShock::Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity) {
	CShock *pShock = GetClassPtr((CShock *)NULL);
	pShock->Spawn();

	UTIL_SetOrigin(pShock, vecStart);
	pShock->pev->velocity = vecVelocity;
	pShock->pev->owner = ENT(pevOwner);

	pShock->SetThink(&CShock::ShockThink);
	pShock->SetNextThink(0);
}

void CShock::Touch(CBaseEntity *pOther) {
	// Do not collide with the owner.
	if (ENT(pOther->pev) == pev->owner)
		return;

	if (UTIL_PointContents(pev->origin) == CONTENT_SKY) {
		ClearBeam();
		UTIL_Remove(m_pSprite);
		m_pSprite = NULL;
		UTIL_Remove(this);
		return;
	}

	TraceResult tr;
	entvars_t *pevOwner;

	// splat sound
	EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "weapons/shock_impact.wav", VOL_NORM, ATTN_NORM, 0, RANDOM_FLOAT(80, 110));

	// lighting on impact
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(pev->origin.x);	// X
		WRITE_COORD(pev->origin.y);	// Y
		WRITE_COORD(pev->origin.z);	// Z
		WRITE_BYTE(10);		// radius * 0.1
		WRITE_BYTE(0);		// r
		WRITE_BYTE(255);	// g
		WRITE_BYTE(255);	// b
		WRITE_BYTE(10);		// time * 10
		WRITE_BYTE(10);		// decay * 0.1
	MESSAGE_END();

	if (!pOther->pev->takedamage) {
		pevOwner = VARS(pev->owner);
		// UNDONE: this needs to call TraceAttack instead
		ClearMultiDamage();

		if (g_pGameRules->IsMultiplayer())
			pOther->TraceAttack(pevOwner, (pev->dmg / 2), pev->velocity.Normalize(), &tr, DMG_ENERGYBEAM);
		else
			pOther->TraceAttack(pevOwner, pev->dmg, pev->velocity.Normalize(), &tr, DMG_ENERGYBEAM);

		ApplyMultiDamage(pev, pevOwner);

		//RadiusDamage
		RadiusDamage(tr.vecEndPos, pev, pev, pev->dmg, 16, CLASS_ALIEN_BIOWEAPON, DMG_BLAST | DMG_ALWAYSGIB);

		// make a splat on the wall
		UTIL_TraceLine(pev->origin, pev->origin + pev->velocity * 10, dont_ignore_monsters, ENT(pev), &tr);
		UTIL_DecalTrace(&tr, DECAL_SHOCKSCORCH1 + RANDOM_LONG(0, 2));
		UTIL_Sparks(tr.vecPlaneNormal);

		if (pOther->pev->flags & FL_MONSTER | pOther->IsPlayer()) {
			pOther->pev->renderfx = kRenderFxGlowShell;
			pOther->pev->rendercolor.x = 0; // R
			pOther->pev->rendercolor.y = 255; // G
			pOther->pev->rendercolor.z = 255; // B
			pOther->pev->renderamt = 1;
			pShockedEnt = pOther;
		}

		SetThink(&CShock::FadeShock);
		SetNextThink(1.0);
	} else {
		pOther->TakeDamage(pev, pev, pev->dmg, DMG_ENERGYBEAM | DMG_ALWAYSGIB);
	}

	// Clear the beam.
	ClearBeam();

	// Clear the sprite.
	UTIL_Remove(m_pSprite);
	m_pSprite = NULL;

	SetThink(&CShock::SUB_Remove);
	SetNextThink(0);
	UTIL_Sparks(tr.vecEndPos);
}

void CShock::FadeShock(void) {
	if (pShockedEnt) {
		pShockedEnt->pev->renderfx = kRenderFxNone;
		pShockedEnt->pev->rendercolor.x = 0; // R
		pShockedEnt->pev->rendercolor.y = 0; // G
		pShockedEnt->pev->rendercolor.z = 0; // B
		pShockedEnt->pev->renderamt = 255;
	}

	UTIL_Remove(this);
}

//=========================================================
// Purpose:
//=========================================================
void CShock::CreateBeam(const Vector& start, const Vector& end, int width) {
	if (m_pBeam) {
		ClearBeam();
	}

	m_pBeam = CBeam::BeamCreate("sprites/lgtning.spr", width);
	if (!m_pBeam)
		return;

	m_pBeam->PointsInit(start, end);
	m_pBeam->SetColor(180, 255, 250);
	m_pBeam->SetBrightness(RANDOM_LONG(24, 25) * 10);
	m_pBeam->SetFrame(0);
	m_pBeam->SetScrollRate(10);
	m_pBeam->SetNoise(20);
	m_pBeam->SetFlags(SF_BEAM_SHADEIN | SF_BEAM_SHADEOUT);
}

//=========================================================
// Purpose:
//=========================================================
void CShock::ClearBeam() {
	if (m_pBeam) {
		UTIL_Remove(m_pBeam);
		m_pBeam = NULL;
	}
}

void CShock::UpdateBeam(const Vector& start, const Vector& end) {
	if (!m_pBeam) {
		// Create the beam if not already created.
		CreateBeam(start, end, SHOCK_BEAM_WIDTH);
	} else {
		m_pBeam->SetStartPos(start);
		m_pBeam->SetEndPos(end);
		m_pBeam->RelinkBeam();
	}

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(pev->origin.x);	// X
		WRITE_COORD(pev->origin.y);	// Y
		WRITE_COORD(pev->origin.z);	// Z
		WRITE_BYTE(10);     // radius
		WRITE_BYTE(0);		// r
		WRITE_BYTE(255);	// g
		WRITE_BYTE(255);	// b
		WRITE_BYTE(1);     // life * 10
		WRITE_BYTE(0); // decay
	MESSAGE_END();
}

void CShock::ComputeBeamPositions(const Vector& vel, Vector* pos1, Vector* pos2) {
	Vector vNormVelocity = pev->velocity.Normalize();
	*pos1 = pev->origin + (vNormVelocity *  SHOCK_BEAM_LENGTH_HALF);
	*pos2 = pev->origin + (vNormVelocity * -SHOCK_BEAM_LENGTH_HALF);
}

void CShock::Glow(void) {
	m_pSprite = CSprite::SpriteCreate("sprites/glow03.spr", m_vecBeamEnd, TRUE);
	m_pSprite->SetTransparency(kRenderTransAdd, 255, 255, 255, 80, kRenderFxNoDissipation);
	m_pSprite->SetAttachment(edict(), 1);
	m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;
	m_pSprite->pev->flags |= FL_SKIPLOCALHOST;
	m_pSprite->SetScale(0.25);
}