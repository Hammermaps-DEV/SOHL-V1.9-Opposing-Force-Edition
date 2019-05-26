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
#include "weapon_grapple.h"
#include "proj_grapple_tonguetip.h"

LINK_ENTITY_TO_CLASS(proj_hook, CGrappleHook);

CGrappleHook* CGrappleHook::Create(Vector Pos, Vector Aim, CBasePlayer* Owner)
{
	//ALERT(at_console, "Tongue spawned.\n");
	CGrappleHook* Hook = GetClassPtr((CGrappleHook*)NULL);
	UTIL_SetOrigin(Hook, Pos);
	Hook->pev->angles = Aim;
	Hook->Spawn();
	Hook->SetTouch(&CGrappleHook::Hit);
	Hook->pev->owner = Owner->edict();
	Hook->myowner = Owner;
	return Hook;
}

void CGrappleHook::Spawn()
{
	Precache();

	SET_MODEL(ENT(pev), "models/null.mdl");
	SetMoveType(MOVETYPE_FLY);
	SetSolidType(SOLID_BBOX);
	pev->rendermode = kRenderNormal;
	pev->renderamt = 0;

	m_iHitMonster = 0;	// Fograin92: We didn't hit any monsters
	bPullBack = false;	// Fograin92: We just launched the tongue, we don't want to pull it back

	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	UTIL_SetOrigin(this, pev->origin);

	SetClassname("proj_hook");

	SetThink(&CGrappleHook::Move);
	SetTouch(&CGrappleHook::Hit);

	UTIL_MakeVectors(pev->angles);
	pev->velocity = gpGlobals->v_forward * 500;
	pev->gravity = 0;
	SetNextThink(0.01);
	pev->dmg = 0;
}


void CGrappleHook::Precache()
{
	PRECACHE_MODEL("sprites/tongue.spr");
	PRECACHE_MODEL("models/null.mdl");
	PRECACHE_SOUND("weapons/bgrapple_impact.wav");
}


void CGrappleHook::Hit(CBaseEntity* Target)
{
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/bgrapple_impact.wav", VOL_LOW, ATTN_NORM);

	TraceResult TResult;
	Vector StartPosition;
	Vector delta = Vector(8, 8, 0);
	Vector mins = pev->origin - delta;
	Vector maxs = pev->origin + delta;
	maxs.z = pev->origin.z;

	CBaseEntity *pList[1];
	int count = UTIL_EntitiesInBox(pList, 1, mins, maxs, (FL_MONSTER));

	// Fograin92: We did hit a monster
	if (count)
	{
		m_iHitMonster = 1;	// Fograin92: Let's assume we hit bigger monster.
		// Fograin92: Loop through everything we did hit.
		for (int i = 0; i < count; i++)
		{
			//ALERT(at_console, "Hit: %s\n", STRING(pList[i]->pev->classname));
			myHitMonster = pList[i]; // Fograin92: Hook pointer to our monster.

			// Fograin92: Check what did we hit
			if ((FClassnameIs(pList[i]->pev, "monster_babycrab"))
				|| (FClassnameIs(pList[i]->pev, "monster_headcrab"))
				|| (FClassnameIs(pList[i]->pev, "monster_cockroach"))
				|| (FClassnameIs(pList[i]->pev, "monster_rat"))
				)
			{
				m_iHitMonster = 2;	// Fograin92: This is tiny monster.
				SetTouch(NULL);		// Fograin92: Let's reset this, we don't want multiple HIT execs when target is moving.
				break;
			}
			else if (FClassnameIs(pList[i]->pev, "monster_barnacle"))
			{
				// Fograin92: We did hit another barnacle
				bPullBack = true;	// Let's pull back the tongue, we don't want any barnacle <-> barnacle love here...
			}
		}

		// Fograin92: If it's still "1" then we did hit bigger monster
		if (m_iHitMonster == 1)
		{
			pev->velocity = pev->velocity.Normalize();
			myowner->m_afPhysicsFlags |= PFLAG_ON_GRAPPLE; // Set physics flag to "on grapple"
			myowner->SetMoveType(MOVETYPE_BOUNCE); // Remove gravity effect on player
			SetTouch(NULL);	// Fograin92: Let's reset this, we don't want multiple HIT execs when target is moving.
		}
	}
	else
	{
		//ALERT(at_console, "Tongue hit a brush entity.\n");
		TraceResult tr;
		float rgfl1[3];
		float rgfl2[3];
		const char *pTextureName;
		Vector vecSrc = pev->origin;
		Vector vecEnd = vecSrc + gpGlobals->v_forward * 8;

		UTIL_MakeVectors(pev->v_angle);
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		vecSrc.CopyToArray(rgfl1);
		vecEnd.CopyToArray(rgfl2);
		if (pEntity)
		{
			// Fograin92: Did we hit a gib?
			if (pEntity->Classify() == CLASS_GIBS)
			{
				// Fograin92: Let's pull this gib
				myHitMonster = pEntity;
				m_iHitMonster = 2;
				SetTouch(NULL);
				return;
			}

			// Fograin92: If we are still here, try to trace texture
			pTextureName = TRACE_TEXTURE(ENT(pEntity->pev), rgfl1, rgfl2);
		}
		else
			pTextureName = TRACE_TEXTURE(ENT(0), rgfl1, rgfl2);

		if (pTextureName && myowner && (strnicmp(pTextureName, "xeno_", 5) == 0))
		{
			pev->velocity = pev->velocity.Normalize();
			myowner->m_afPhysicsFlags |= PFLAG_ON_GRAPPLE; // Set physics flag to "on grapple"
			myowner->SetMoveType(MOVETYPE_BOUNCE); // Remove gravity effect on player
		}
		else
		{
			// Fograin92: We did hit a non organic wall, let's pull-back the tongue
			bPullBack = true;
		}
	}
}

void CGrappleHook::Killed(entvars_t *pev, int gib)
{
	ALERT(at_console, "Tongue was killed.\n");

	// Fograin92: Clear player
	myowner->SetMoveType(MOVETYPE_WALK); //Re-apply gravity
	myowner->m_afPhysicsFlags &= ~PFLAG_ON_GRAPPLE; //Remove "on grapple" flag
	myowner->m_pGrappleExists = false;
	myowner->m_flNextAttack = UTIL_GlobalTimeBase() + 1.0;

	// Fograin92: Clear monster
	if ((m_iHitMonster == 2) && (myHitMonster->IsAlive()))
		myHitMonster->SetMoveType(MOVETYPE_STEP);	// Re-apply gravity to the pulled monster, if it's alive

														// Fograin92: Clear tongue leftovers
	bPullBack = false;
	UTIL_Remove(m_pTongue);
	m_pTongue = NULL;

	m_iHitMonster = 0;
	SetThink(NULL);
	SetTouch(NULL);
	SUB_Remove();
}

void CGrappleHook::Move()
{
	//ALERT(at_console, "Tongue was move.\n");

	// Fograin92: If owner (player) is dead
	if (!myowner->IsAlive())
	{
		Killed(pev, 0); // Remove tongue instantly
		return;
	}

	// Fograin92: Player isn't holding attack buttons
	if (!(myowner->pev->button & (IN_ATTACK | IN_ATTACK2))) {
		bPullBack = true;	// Fograin92: We should pull the tongue back
	}

	// Fograin92: Animate pull-back tongue animation ONLY if we didn't hit a monster
	if (bPullBack)
	{
		UTIL_MakeVectors(myowner->pev->v_angle + myowner->pev->punchangle);
		Vector GunPosition = myowner->GetGunPosition();
		GunPosition = GunPosition + gpGlobals->v_up * -4 + gpGlobals->v_right * 3 + gpGlobals->v_forward * 6;
		myowner->m_pGrapplePullBack = true;

		pev->velocity = (GunPosition - pev->origin) * 10;	// Pull back the tongue tip
		float fDistance = (GunPosition - pev->origin).Length2D();	// Calculate distance between tongue tip and player
		//ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> %f\n", fDistance );

		if (fDistance < 40)
		{
			Killed(pev, 0);
			return;
		}

	}
	else
	{
		// Fograin92: We did hit a monster
		if (m_iHitMonster > 0)
		{
			// Fograin92: Let's "stick" grapple tongue XYZ to target's center XYZ
			pev->origin = myHitMonster->Center();

			// Fograin92: We did hit tiny monster, let's pull it
			if (m_iHitMonster == 2)
			{
				myHitMonster->SetMoveType(MOVETYPE_FLY);	// Remove gravity effect on our pulled monster
				myHitMonster->pev->velocity = (myowner->pev->origin - myHitMonster->pev->origin) * 4;	// Pull our monster
			}

			// Fograin92: Check distance (player <-> monster)
			float fDistance = (myowner->pev->origin - myHitMonster->pev->origin).Length2D();

			// Fograin92: The monster is very close to player, let's OWN IT!
			if (fDistance < 40)
			{
				//ALERT(at_console, "OWNED -> %s\n", STRING(myHitMonster->pev->classname));

				// Fograin92: Did we pull the gib?
				if (myHitMonster->Classify() == CLASS_GIBS)
					myHitMonster->SUB_Remove();
				else
					myHitMonster->TakeDamage(myHitMonster->pev, myowner->pev, 10000, DMG_GENERIC);

				Killed(pev, 0);	// Fograin92: Target died, kill tongue
			}

		}

		myowner->m_pGrapplePullBack = false;
	}

	// Fograin92: If tongue (beam) exists
	if (m_pTongue)
	{
		UTIL_MakeVectors(myowner->pev->v_angle + myowner->pev->punchangle);
		Vector GunPosition = myowner->GetGunPosition();
		GunPosition = GunPosition + gpGlobals->v_up * -4 + gpGlobals->v_right * 3 + gpGlobals->v_forward * 6;

		m_pTongue->PointEntInit(GunPosition, this->entindex());
	}
	else
	{
		// Fograin92: Create tongue (beam)
		m_pTongue = CBeam::BeamCreate("sprites/tongue.spr", 8);
		m_pTongue->SetFlags(0x20);	// Solid beam
		m_pTongue->SetColor(100, 100, 100);
		m_pTongue->SetScrollRate(20);
	}

	SetNextThink(0.01);
}