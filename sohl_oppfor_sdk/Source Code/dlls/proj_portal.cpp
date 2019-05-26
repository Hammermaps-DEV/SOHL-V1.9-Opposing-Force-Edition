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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "shake.h"
#include "decals.h"
#include "proj_portal.h"

#define DISPLACER_BEAM_LENGTH 50
#define DISPLACER_BEAM_LENGTH_HALF	DISPLACER_BEAM_LENGTH * 0.5f

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(portal, CPortal);

TYPEDESCRIPTION	CPortal::m_SaveData[] =
{
	DEFINE_FIELD(CPortal, m_maxFrame, FIELD_INTEGER),
	DEFINE_ARRAY(CPortal, m_pBeam, FIELD_CLASSPTR, MAX_PORTAL_BEAMS),
	DEFINE_FIELD(CPortal, m_vecBeamStart, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(CPortal, m_vecBeamEnd, FIELD_POSITION_VECTOR),
};

IMPLEMENT_SAVERESTORE(CPortal, CBaseEntity);

void CPortal::Spawn() {
	Precache();

	SetMoveType(MOVETYPE_FLY);
	SetClassname("portal");

	SetSolidType(SOLID_BBOX);
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;

	SET_MODEL(ENT(pev), "sprites/exit1.spr");
	pev->frame = 0;
	pev->scale = 0.7;
	pev->dmg = gSkillData.voltigoreDmgBeam;

	UTIL_SetOrigin(this, pev->origin);
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	Vector vDir = pev->velocity.Normalize();
	ComputeBeamPositions(vDir, &m_vecBeamStart, &m_vecBeamEnd);

	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex) - 1;

	Glow();
	CreateBeams();
	SetThink(&CPortal::GlowThink);
	SetNextThink(0.1);
}

void CPortal::GlowThink() {
	Vector vDir = pev->velocity.Normalize();
	ComputeBeamPositions(vDir, &m_vecBeamStart, &m_vecBeamEnd);
	SetNextThink(0.01);
}

void CPortal::Precache()
{
	iRingSprite = PRECACHE_MODEL("sprites/disp_ring.spr");
	PRECACHE_MODEL("sprites/blueflare2.spr");
	PRECACHE_MODEL("sprites/exit1.spr");
	PRECACHE_MODEL("sprites/xflare1.spr");
	PRECACHE_MODEL("sprites/plasma.spr");
	PRECACHE_SOUND("weapons/displacer_impact.wav");
	PRECACHE_SOUND("weapons/displacer_teleport.wav");
	PRECACHE_SOUND("weapons/displacer_teleport_player.wav");
}

void CPortal::Animate() {
	pev->frame += 1; //animate teleball
	if (pev->frame > 24)
		pev->frame = fmod(pev->frame, 24);

	// Update beams.
	UpdateBeams();
	SetNextThink(0.1);
}

void CPortal::Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity) {
	CPortal *pSpit = GetClassPtr((CPortal *)NULL);
	pSpit->Spawn();

	UTIL_SetOrigin(pSpit, vecStart);
	pSpit->pev->velocity = vecVelocity;
	pSpit->pev->owner = ENT(pevOwner);

	pSpit->SetThink(&CPortal::Animate);
	pSpit->SetNextThink(0.1);
}

void CPortal::Touch(CBaseEntity *pOther)
{
	TraceResult tr;
	Vector vecSpot;
	Vector vecSrc;
	CBaseEntity *pTarget = NULL;
	vecSpot = pev->origin - pev->velocity.Normalize() * 32;

	if (m_pSprite) {
		UTIL_Remove(m_pSprite);
		m_pSprite = NULL;
	}

	if (UTIL_PointContents(pev->origin) == CONTENT_SKY) {
		ClearBeams();
		UTIL_Remove(this);
		return;
	}

	UTIL_TraceLine(vecSpot, vecSpot + pev->velocity.Normalize() * 64, dont_ignore_monsters, ENT(pev), &tr);

	entvars_t *pevOwner;
	if (pev->owner)
		pevOwner = VARS(pev->owner);
	else
		pevOwner = NULL;

	if (!pOther->pev->takedamage) {
		// make a splat on the wall
		UTIL_TraceLine(pev->origin, pev->origin + pev->velocity * 10, dont_ignore_monsters, ENT(pev), &tr);
		UTIL_DecalTrace(&tr, DECAL_SCORCH1 + RANDOM_LONG(0, 2));
		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/displacer_impact.wav", VOL_NORM, ATTN_NORM, 0, 100);
		UTIL_Sparks(pev->origin);
	}
	else {
		if (g_pGameRules->IsMultiplayer()) {
			if (pOther->IsPlayer()) {
				for (int i = RANDOM_LONG(1, 5); i > 0; i--)
					pTarget = UTIL_FindEntityByClassname(pTarget, "info_player_deathmatch");

				if (pTarget) {
					UTIL_ScreenFade(pOther, Vector(0, 160, 0), 0.5, 0.5, 255, FFADE_IN);

					Vector tmp = pTarget->pev->origin;
					tmp.z -= pOther->pev->mins.z;
					tmp.z++;
					UTIL_SetOrigin(pOther, tmp);

					pOther->pev->angles = pTarget->pev->angles;
					pOther->pev->velocity = pOther->pev->basevelocity = g_vecZero;

					CSprite *pSpr = CSprite::SpriteCreate("sprites/xflare1.spr", vecSrc, TRUE);
					pSpr->AnimateAndDie(6);
					pSpr->SetTransparency(kRenderGlow, 184, 250, 214, 255, kRenderFxNoDissipation);

					EMIT_SOUND(ENT(pOther->pev), CHAN_WEAPON, "weapons/displacer_teleport_player.wav", VOL_NORM, ATTN_NORM);

					vecSrc = pTarget->pev->origin;
					MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
					WRITE_BYTE(TE_DLIGHT);
					WRITE_COORD(vecSrc.x);	// X
					WRITE_COORD(vecSrc.y);	// Y
					WRITE_COORD(vecSrc.z);	// Z
					WRITE_BYTE(24);		// radius * 0.1
					WRITE_BYTE(255);		// r
					WRITE_BYTE(180);		// g
					WRITE_BYTE(96);		// b
					WRITE_BYTE(20);		// time * 10
					WRITE_BYTE(0);		// decay * 0.1
					MESSAGE_END();
				}
			}
		}
		else if (pOther->pev->flags & FL_MONSTER) {
			if (pOther->pev->health <= 200 && !FClassnameIs(pOther->pev, "monster_nihilanth")
				&& !FClassnameIs(pOther->pev, "monster_apache") && !FClassnameIs(pOther->pev, "monster_osprey")
				&& !FClassnameIs(pOther->pev, "monster_gargantua") && !FClassnameIs(pOther->pev, "monster_bigmomma")) {
				EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/displacer_teleport_player.wav", VOL_NORM, ATTN_NORM, 0, 100);
				pOther->Killed(pev, GIB_NEVER);
				pOther->SUB_Remove();
			}
			else {
				EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/displacer_teleport.wav", VOL_NORM, ATTN_NORM, 0, 100);
				pOther->TakeDamage(pev, pev, pev->dmg, DMG_ENERGYBEAM | DMG_SHOCK);
			}
		}
		else {
			pOther->TakeDamage(pev, pev, pev->dmg, DMG_ENERGYBEAM | DMG_ALWAYSGIB);
		}

		RadiusDamage(pev->origin, pev, pevOwner, pev->dmg, 256, CLASS_NONE, DMG_ENERGYBEAM);
	}

	// portal circle
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMCYLINDER);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z + 840);
	WRITE_SHORT(iRingSprite);
	WRITE_BYTE(0);
	WRITE_BYTE(10);
	WRITE_BYTE(3);
	WRITE_BYTE(20);
	WRITE_BYTE(0);
	WRITE_BYTE(255);
	WRITE_BYTE(255);
	WRITE_BYTE(255);
	WRITE_BYTE(255);
	WRITE_BYTE(0);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(pev->origin.x);	// X
	WRITE_COORD(pev->origin.y);	// Y
	WRITE_COORD(pev->origin.z);	// Z
	WRITE_BYTE(32);		// radius * 0.1
	WRITE_BYTE(255);		// r
	WRITE_BYTE(180);		// g
	WRITE_BYTE(96);		// b
	WRITE_BYTE(60);		// time * 10
	WRITE_BYTE(20);		// decay * 0.1
	MESSAGE_END();

	ClearBeams();
	SetThink(&CPortal::SUB_Remove);
	SetNextThink(0);
}

void CPortal::CreateBeams() {
	for (int i = 0; i < MAX_PORTAL_BEAMS; i++) {
		m_pBeam[i] = CBeam::BeamCreate("sprites/plasma.spr", RANDOM_LONG(2, 3) * 10);
		m_pBeam[i]->PointEntInit(pev->origin, entindex());
		m_pBeam[i]->SetColor(90, 170, 16);
		m_pBeam[i]->SetNoise(65);
		m_pBeam[i]->SetBrightness(255);
		m_pBeam[i]->SetWidth(30);
		m_pBeam[i]->SetScrollRate(35);
		m_pBeam[i]->SetScrollRate(35);
	}
}

void CPortal::ClearBeams() {
	for (int i = 0; i < MAX_PORTAL_BEAMS; i++) {
		if (m_pBeam[i]) {
			UTIL_Remove(m_pBeam[i]);
			m_pBeam[i] = NULL;
		}
	}
}

void CPortal::UpdateBeams() {
	TraceResult tr, tr1;
	int i, j;
	float flDist = 1.0;

	Vector vecSrc, vecAim;
	vecSrc = pev->origin;

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(pev->origin.x);	// X
	WRITE_COORD(pev->origin.y);	// Y
	WRITE_COORD(pev->origin.z);	// Z
	WRITE_BYTE(25);     // radius
	WRITE_BYTE(255);		// r
	WRITE_BYTE(180);		// g
	WRITE_BYTE(96);		// b
	WRITE_BYTE(1);     // life * 10
	WRITE_BYTE(0); // decay
	MESSAGE_END();

	for (i = 0; i < MAX_PORTAL_BEAMS; i++) {
		for (j = 0; j < 10; ++j) {
			vecAim = gpGlobals->v_right * RANDOM_FLOAT(0, 2) + gpGlobals->v_up * RANDOM_FLOAT(-1, 2);
			UTIL_TraceLine(vecSrc, vecSrc + vecAim * 512, dont_ignore_monsters, ENT(pev), &tr1);
			if (flDist > tr1.flFraction) {
				tr = tr1;
				flDist = tr.flFraction;
			}
		}

		DecalGunshot(&tr, BULLET_PLAYER_CROWBAR);

		// Update the beams.
		m_pBeam[i]->SetStartPos(tr.vecEndPos);
		m_pBeam[i]->SetEndEntity(entindex());
		m_pBeam[i]->RelinkBeam();
	}
}

void CPortal::ComputeBeamPositions(const Vector& vel, Vector* pos1, Vector* pos2) {
	Vector vNormVelocity = pev->velocity.Normalize();
	*pos1 = pev->origin + (vNormVelocity *  DISPLACER_BEAM_LENGTH_HALF);
	*pos2 = pev->origin + (vNormVelocity * -DISPLACER_BEAM_LENGTH_HALF);
}

void CPortal::FadeShock() {
	if (pShockedEnt) {
		pShockedEnt->pev->renderfx = kRenderFxNone;
		pShockedEnt->pev->rendercolor.x = 0; // R
		pShockedEnt->pev->rendercolor.y = 0; // G
		pShockedEnt->pev->rendercolor.z = 0; // B
		pShockedEnt->pev->renderamt = 255;
	}

	UTIL_Remove(this);
}

void CPortal::Glow() {
	m_pSprite = CSprite::SpriteCreate("sprites/glow03.spr", m_vecBeamEnd, TRUE);
	m_pSprite->SetTransparency(kRenderTransAdd, 255, 255, 255, 80, kRenderFxNoDissipation);
	m_pSprite->SetAttachment(edict(), 1);
	m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;
	m_pSprite->pev->flags |= FL_SKIPLOCALHOST;
	m_pSprite->SetScale(1);
}