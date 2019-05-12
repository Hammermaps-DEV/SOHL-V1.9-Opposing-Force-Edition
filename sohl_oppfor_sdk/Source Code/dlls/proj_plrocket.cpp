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
#include "weapon_rpg.h"
#include "proj_grenade.h"
#include "proj_plrocket.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(rpg_rocket, CRpgRocket);

CRpgRocket *CRpgRocket::Create(Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner, CRpg *pLauncher) {
	CRpgRocket *pRocket = GetClassPtr((CRpgRocket *)NULL);

	UTIL_SetOrigin(pRocket, vecOrigin);
	pRocket->pev->angles = vecAngles;
	pRocket->Spawn();
	pRocket->SetTouch(&CRpgRocket::RocketTouch);
	pRocket->m_pLauncher = pLauncher;// remember what RPG fired me. 
	pRocket->m_pLauncher->m_iChargeLevel++;// register this missile as active for the launcher
	pRocket->pev->owner = pOwner->edict();

	return pRocket;
}

TYPEDESCRIPTION	CRpgRocket::m_SaveData[] =
{
	DEFINE_FIELD(CRpgRocket, m_flIgniteTime, FIELD_TIME),
	DEFINE_FIELD(CRpgRocket, m_pLauncher, FIELD_CLASSPTR),
};
IMPLEMENT_SAVERESTORE(CRpgRocket, CGrenade);

void CRpgRocket::Spawn()
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	pev->renderfx = kRenderFxEntInPVS;//for telemetric rocket
	SET_MODEL(ENT(pev), "models/rpgrocket.mdl");
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	UTIL_SetOrigin(this, pev->origin);

	pev->classname = MAKE_STRING("rpg_rocket");

	SetThink(&CRpgRocket::IgniteThink);
	SetTouch(&CRpgRocket::ExplodeTouch);

	pev->angles.x -= 30;
	UTIL_MakeVectors(pev->angles);
	pev->angles.x = -(pev->angles.x + 30);
	pev->velocity = gpGlobals->v_forward * 400;
	pev->gravity = 0.5;

	SetNextThink(0.4);

	pev->dmg = gSkillData.plrDmgRPG;
}

void CRpgRocket::RocketTouch(CBaseEntity *pOther) {
	CBaseEntity *pPlayer = CBaseEntity::Instance(pev->owner);
	if (m_pLauncher) {
		// my launcher is still around, tell it I'm dead.
		m_pLauncher->m_iChargeLevel--;
		if (m_pLauncher->m_pSpot)//make sound only if laser spot created
			EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "weapons/beep2.wav", VOL_LOW, ATTN_NORM);//play sound at player

		m_pLauncher->m_pActiveRocket = true;
	}

	STOP_SOUND(edict(), CHAN_VOICE, "weapons/rocket1.wav");
	ExplodeTouch(pOther);
}

void CRpgRocket::Detonate() {
	TraceResult tr;
	Vector		vecSpot;// trace starts here!
	CBaseEntity *pPlayer = CBaseEntity::Instance(pev->owner);
	if (m_pLauncher)
	{
		// my launcher is still around, tell it I'm dead.
		m_pLauncher->m_iChargeLevel--;
		if (m_pLauncher->m_pSpot)//make sound only if laser spot created
			EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "weapons/beep2.wav", VOL_LOW, ATTN_NORM);//play sound at player

		m_pLauncher->m_pActiveRocket = false;
	}

	STOP_SOUND(edict(), CHAN_VOICE, "weapons/rocket1.wav");
	vecSpot = pev->origin + Vector(0, 0, 8);
	UTIL_TraceLine(vecSpot, vecSpot + Vector(0, 0, -40), ignore_monsters, ENT(pev), &tr);

	Explode(&tr, DMG_BLAST);
}

void CRpgRocket::Precache()
{
	PRECACHE_MODEL("models/rpgrocket.mdl");
	m_iTrail = PRECACHE_MODEL("sprites/smoke.spr");
	PRECACHE_SOUND("weapons/rocket1.wav");
	PRECACHE_SOUND("weapons/beep2.wav");
}

void CRpgRocket::IgniteThink()
{
	pev->movetype = MOVETYPE_FLY;
	pev->effects |= EF_LIGHT;

	CreateTrail();
	m_flIgniteTime = UTIL_GlobalTimeBase();

	// set to follow laser spot
	SetThink(&CRpgRocket::FollowThink);
	SetNextThink(0.1);
}

void CRpgRocket::CreateTrail()
{
	if (!b_setup)
	{
		// make rocket sound after save\load
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/rocket1.wav", VOL_NORM, 0.5);
		// restore rocket trail
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMFOLLOW);
		WRITE_SHORT(entindex());	// entity
		WRITE_SHORT(m_iTrail);	// model
		WRITE_BYTE(30); // life
		WRITE_BYTE(5);  // width
		WRITE_BYTE(200);   // r, g, b
		WRITE_BYTE(200);   // r, g, b
		WRITE_BYTE(200);   // r, g, b
		WRITE_BYTE(200);	// brightness
		MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
		b_setup = TRUE;
	}
}

void CRpgRocket::FollowThink()
{
	CBaseEntity *pOther = NULL;
	Vector vecTarget;
	Vector vecDir;
	float flDist, flMax, flDot;
	TraceResult tr;
	UTIL_MakeAimVectors(pev->angles);

	CreateTrail();

	vecTarget = gpGlobals->v_forward;
	flMax = 4096;
	// Examine all entities within a reasonable radius
	while ((pOther = UTIL_FindEntityByClassname(pOther, "laser_spot")) != NULL)
	{
		UTIL_TraceLine(pev->origin, pOther->pev->origin, dont_ignore_monsters, ENT(pev), &tr);
		// ALERT( at_console, "%f\n", tr.flFraction );
		if (tr.flFraction >= 0.90)
		{
			vecDir = pOther->pev->origin - pev->origin;
			flDist = vecDir.Length();
			vecDir = vecDir.Normalize();
			flDot = DotProduct(gpGlobals->v_forward, vecDir);
			if ((flDot > 0) && (flDist * (1 - flDot) < flMax))
			{
				flMax = flDist * (1 - flDot);
				vecTarget = vecDir;
			}
		}
	}

	pev->angles = UTIL_VecToAngles(vecTarget);

	// this acceleration and turning math is totally wrong, but it seems to respond well so don't change it.
	float flSpeed = pev->velocity.Length();
	if (UTIL_GlobalTimeBase() - m_flIgniteTime < 1.0) {
		pev->velocity = pev->velocity * 0.2 + vecTarget * (flSpeed * 0.8 + 400);
		if (pev->waterlevel == 3 && pev->watertype > CONTENT_FLYFIELD) {
			// go slow underwater
			if (pev->velocity.Length() > 300) {
				pev->velocity = pev->velocity.Normalize() * 300;
			}
			UTIL_BubbleTrail(pev->origin - pev->velocity * 0.1, pev->origin, 4);
		}
		else {
			if (pev->velocity.Length() > 2000) {
				pev->velocity = pev->velocity.Normalize() * 2000;
			}
		}
	}
	else {
		if (pev->effects & EF_LIGHT) {
			pev->effects = 0;
			STOP_SOUND(ENT(pev), CHAN_VOICE, "weapons/rocket1.wav");
		}
		pev->velocity = pev->velocity * 0.2 + vecTarget * flSpeed * 0.798;
		if ((pev->waterlevel == 0 || pev->watertype == CONTENT_FOG) && pev->velocity.Length() < 1500) {
			Detonate();
		}
	}
	//ALERT( at_console, "%.0f\n", flSpeed );

	SetNextThink(0.05);
}