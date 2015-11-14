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
#include	"effects.h"
#include	"decals.h"
#include	"proj_gonomespit.h"

//=========================================================
// Gonome's spit projectile
//=========================================================
LINK_ENTITY_TO_CLASS(gonomespit, CGonomeSpit);

TYPEDESCRIPTION	CGonomeSpit::m_SaveData[] = {
	DEFINE_FIELD(CGonomeSpit, m_maxFrame, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CGonomeSpit, CBaseEntity);

void CGonomeSpit::Spawn(void) {
	Precache();

	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING("gonomespit");

	pev->solid = SOLID_BBOX;
	pev->rendermode = kRenderTransAlpha;
	pev->renderamt = 255;

	SET_MODEL(ENT(pev), "sprites/blood_chnk.spr");
	pev->frame = 0;
	pev->scale = 0.5;

	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex) - 1;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CGonomeSpit::Precache() {
	iGonomeSpitSprite = PRECACHE_MODEL("sprites/blood_tinyspit.spr");// client side spittle.
}

void CGonomeSpit::Animate(void) {
	SetNextThink(0.1);
	if (pev->frame++) {
		if (pev->frame > m_maxFrame) {
			pev->frame = 0;
		}
	}
}

void CGonomeSpit::Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity) {
	CGonomeSpit *pGSpit = GetClassPtr((CGonomeSpit *)NULL);
	pGSpit->Spawn();

	UTIL_SetOrigin( pGSpit, vecStart);
	pGSpit->pev->velocity = vecVelocity;
	pGSpit->pev->owner = ENT(pevOwner);

	pGSpit->SetThink(&CGonomeSpit::Animate);
	pGSpit->SetNextThink(0.1);
}

void CGonomeSpit::Touch(CBaseEntity *pOther)
{
	TraceResult tr;
	int		iPitch;

	// splat sound
	iPitch = RANDOM_FLOAT(90, 110);

	switch (RANDOM_LONG(0, 1))
	{
		case 0:
			EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "bullchicken/bc_spithit1.wav", 1, ATTN_NORM, 0, iPitch);
			break;
		case 1:
			EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "bullchicken/bc_spithit2.wav", 1, ATTN_NORM, 0, iPitch);
			break;
	}

	if (!pOther->pev->takedamage) {
		// make a splat on the wall
		UTIL_TraceLine(pev->origin, pev->origin + pev->velocity * 10, dont_ignore_monsters, ENT(pev), &tr);
		UTIL_DecalTrace(&tr, DECAL_BLOOD2 + RANDOM_LONG(0, 1));

		// make some flecks
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos);
		WRITE_BYTE(TE_SPRITE_SPRAY);
		WRITE_COORD(tr.vecEndPos.x);	// pos
		WRITE_COORD(tr.vecEndPos.y);
		WRITE_COORD(tr.vecEndPos.z);
		WRITE_COORD(tr.vecPlaneNormal.x);	// dir
		WRITE_COORD(tr.vecPlaneNormal.y);
		WRITE_COORD(tr.vecPlaneNormal.z);
		WRITE_SHORT(iGonomeSpitSprite);	// model
		WRITE_BYTE(5);			// count
		WRITE_BYTE(30);			// speed
		WRITE_BYTE(80);			// noise ( client will divide by 100 )
		MESSAGE_END();
	} else {
		pOther->TakeDamage(pev, pev, gSkillData.gonomeDmgGuts, DMG_POISON);
	}

	SetThink(&CGonomeSpit::SUB_Remove);
	SetNextThink(0);
}