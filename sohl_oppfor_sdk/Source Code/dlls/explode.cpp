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
/*

===== explode.cpp ========================================================

  Explosion-related code

*/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "decals.h"
#include "explode.h"
#include "locus.h"
#include "weapons.h"
#include "particle_defs.h"

extern int gmsgParticles;//define external message

// Spark Shower
class CShower : public CBaseEntity
{
	void Spawn(void);
	void Think(void);
	void Touch(CBaseEntity *pOther);
	int ObjectCaps(void) { return FCAP_DONT_SAVE; }
};

LINK_ENTITY_TO_CLASS(spark_shower, CShower);

void CShower::Spawn(void)
{
	pev->velocity = RANDOM_FLOAT(200, 300) * pev->angles;
	pev->velocity.x += RANDOM_FLOAT(-100.f, 100.f);
	pev->velocity.y += RANDOM_FLOAT(-100.f, 100.f);
	if (pev->velocity.z >= 0)
		pev->velocity.z += 200;
	else
		pev->velocity.z -= 200;
	pev->movetype = MOVETYPE_BOUNCE;
	pev->gravity = 0.5;
	SetNextThink(0.1);
	pev->solid = SOLID_NOT;
	SET_MODEL(edict(), "models/grenade.mdl");	// Need a model, just use the grenade, we don't draw it anyway
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
	pev->effects |= EF_NODRAW;
	pev->speed = RANDOM_FLOAT(0.5, 1.5);

	pev->angles = g_vecZero;
}


void CShower::Think(void)
{
	UTIL_Sparks(pev->origin);

	pev->speed -= 0.1;
	if (pev->speed > 0)
		SetNextThink(0.1);
	else
		UTIL_Remove(this);
	pev->flags &= ~FL_ONGROUND;
}

void CShower::Touch(CBaseEntity *pOther)
{
	if (pev->flags & FL_ONGROUND)
		pev->velocity = pev->velocity * 0.1;
	else
		pev->velocity = pev->velocity * 0.6;

	if ((pev->velocity.x*pev->velocity.x + pev->velocity.y*pev->velocity.y) < 10.0)
		pev->speed = 0;
}

class CEnvExplosion : public CBaseMonster
{
public:
	void Spawn();
	void EXPORT Smoke(void);
	void KeyValue(KeyValueData *pkvd);
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int m_iMagnitude;// how large is the fireball? how much damage?
	int m_spriteScale; // what's the exact fireball sprite scale? 
};

TYPEDESCRIPTION	CEnvExplosion::m_SaveData[] =
{
	DEFINE_FIELD(CEnvExplosion, m_iMagnitude, FIELD_INTEGER),
	DEFINE_FIELD(CEnvExplosion, m_spriteScale, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CEnvExplosion, CBaseMonster);
LINK_ENTITY_TO_CLASS(env_explosion, CEnvExplosion);

void CEnvExplosion::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "iMagnitude"))
	{
		m_iMagnitude = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CEnvExplosion::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;

	pev->movetype = MOVETYPE_NONE;
	/*
	if ( m_iMagnitude > 250 )
	{
		m_iMagnitude = 250;
	}
	*/

	float flSpriteScale;
	flSpriteScale = (m_iMagnitude - 50) * 0.6;

	/*
	if ( flSpriteScale > 50 )
	{
		flSpriteScale = 50;
	}
	*/
	if (flSpriteScale < 10)
	{
		flSpriteScale = 10;
	}

	m_spriteScale = (int)flSpriteScale;
}

void CEnvExplosion::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	TraceResult tr;

	pev->model = iStringNull;//invisible
	pev->solid = SOLID_NOT;// intangible

	Vector		vecSpot;// trace starts here!

	//LRC
	if (FStringNull(pev->target))
	{
		vecSpot = pev->origin;
	}
	else
	{
		vecSpot = CalcLocus_Position(this, pActivator, STRING(pev->target));
	}

	UTIL_TraceLine(vecSpot + Vector(0, 0, 8), vecSpot + Vector(0, 0, -32), ignore_monsters, ENT(pev), &tr);

	// Pull out of the wall a bit
	if (tr.flFraction != 1.0)
	{
		pev->origin = tr.vecEndPos + (tr.vecPlaneNormal * (m_iMagnitude - 24) * 0.6);
	}
	else
	{
		pev->origin = vecSpot; //LRC
	}

	// draw decal
	if (!(pev->spawnflags & SF_ENVEXPLOSION_NODECAL))
	{
		CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
		PLAYBACK_EVENT_FULL(FEV_RELIABLE | FEV_GLOBAL, edict(), m_usDecals, 0.0, (float *)&tr.vecEndPos, (float *)&g_vecZero, 0.0, 0.0, pHit->entindex(), 0, 0, 0);
	}

	// draw fireball
	if (!(pev->spawnflags & SF_ENVEXPLOSION_NOFIREBALL))
	{
		PLAYBACK_EVENT_FULL(FEV_RELIABLE | FEV_GLOBAL, edict(), m_usEfx, 0.0, (float *)&pev->origin, (float *)&g_vecZero, pev->dmg, 0.0, 0, 0, 0, 0);

		if (CVAR_GET_FLOAT("cl_expdetail") != 0) {
			MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexFireball_0);
			WRITE_BYTE(((BYTE)m_spriteScale - 50) * 3); // scale * 10 -50
			WRITE_BYTE(35); // framerate
			WRITE_BYTE(TE_EXPLFLAG_NONE);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexFireball_1);
			WRITE_BYTE(((BYTE)m_spriteScale - 50) * 3); // scale * 10 -50
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

		switch (RANDOM_LONG(0, 4)) {
		case 0:	EMIT_SOUND(ENT(pev), CHAN_ITEM, "explosions/explode1.wav", VOL_NORM, ATTN_NORM);	break;
		case 1:	EMIT_SOUND(ENT(pev), CHAN_ITEM, "explosions/explode2.wav", VOL_NORM, ATTN_NORM);	break;
		case 2:	EMIT_SOUND(ENT(pev), CHAN_ITEM, "explosions/explode3.wav", VOL_NORM, ATTN_NORM);	break;
		case 3:	EMIT_SOUND(ENT(pev), CHAN_ITEM, "explosions/explode4.wav", VOL_NORM, ATTN_NORM);	break;
		case 4:	EMIT_SOUND(ENT(pev), CHAN_ITEM, "explosions/explode5.wav", VOL_NORM, ATTN_NORM);	break;
		}

		UTIL_ScreenShake(pev->origin, 12.0, 100.0, 2.0, 1000);

		switch (RANDOM_LONG(0, 2)) {
		case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris1.wav", VOL_LOWER, ATTN_NORM);	break;
		case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris2.wav", VOL_LOWER, ATTN_NORM);	break;
		case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris3.wav", VOL_LOWER, ATTN_NORM);	break;
		}
	}
	else
	{
		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_EXPLOSION);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(g_sModelIndexFireball);
		WRITE_BYTE(0); // no sprite
		WRITE_BYTE(15); // framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);
		MESSAGE_END();
	}

	// do damage
	if (!(pev->spawnflags & SF_ENVEXPLOSION_NODAMAGE))
	{
		RadiusDamage(pev, pev, m_iMagnitude, CLASS_NONE, DMG_BLAST);
	}

	SetThink(&CEnvExplosion::Smoke);
	SetNextThink(0.3);

	// draw sparks
	if (!(pev->spawnflags & SF_ENVEXPLOSION_NOSPARKS))
	{
		int sparkCount = RANDOM_LONG(0, 3);
		Vector mirpos = UTIL_MirrorPos(pev->origin);

		for (int i = 0; i < sparkCount; i++)
			Create("spark_shower", pev->origin, tr.vecPlaneNormal, NULL);

		if (mirpos != Vector(0, 0, 0))
			for (int i = 0; i < sparkCount; i++)
				Create("spark_shower", mirpos, tr.vecPlaneNormal, NULL);
	}
}

void CEnvExplosion::Smoke(void)
{
	if (!(pev->spawnflags & SF_ENVEXPLOSION_NOSMOKE)) {
		if (CVAR_GET_FLOAT("r_particles") != 0) {
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(((BYTE)m_spriteScale - 50) * 0.80); // scale * 10
			WRITE_BYTE(12); // framerate
			MESSAGE_END();

			Vector mirpos = UTIL_MirrorPos(pev->origin);
			if (mirpos != Vector(0, 0, 0))
			{
				MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
				WRITE_BYTE(TE_SMOKE);
				WRITE_COORD(mirpos.x);
				WRITE_COORD(mirpos.y);
				WRITE_COORD(mirpos.z);
				WRITE_SHORT(g_sModelIndexSmoke);
				WRITE_BYTE(((BYTE)m_spriteScale - 50) * 0.80); // scale * 10
				WRITE_BYTE(12); // framerate
				MESSAGE_END();
			}
		}
		else {
			MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE((BYTE)m_spriteScale); // scale * 10
			WRITE_BYTE(12); // framerate
			MESSAGE_END();

			Vector mirpos = UTIL_MirrorPos(pev->origin);
			if (mirpos != Vector(0, 0, 0))
			{
				MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
				WRITE_BYTE(TE_SMOKE);
				WRITE_COORD(mirpos.x);
				WRITE_COORD(mirpos.y);
				WRITE_COORD(mirpos.z);
				WRITE_SHORT(g_sModelIndexSmoke);
				WRITE_BYTE((BYTE)m_spriteScale); // scale * 10
				WRITE_BYTE(12); // framerate
				MESSAGE_END();
			}
		}
	}

	if (!(pev->spawnflags & SF_ENVEXPLOSION_REPEATABLE))
	{
		UTIL_Remove(this);
	}
}


// HACKHACK -- create one of these and fake a keyvalue to get the right explosion setup
void ExplosionCreate(const Vector &center, const Vector &angles, edict_t *pOwner, int magnitude, BOOL doDamage)
{
	KeyValueData	kvd;
	char			buf[128];

	CBaseEntity *pExplosion = CBaseEntity::Create("env_explosion", center, angles, pOwner);
	snprintf(buf, 128, "%3d", magnitude);
	kvd.szKeyName = "iMagnitude";
	kvd.szValue = buf;
	pExplosion->KeyValue(&kvd);
	if (!doDamage)
		pExplosion->pev->spawnflags |= SF_ENVEXPLOSION_NODAMAGE;

	pExplosion->Spawn();
	pExplosion->Use(NULL, NULL, USE_TOGGLE, 0);
}
