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
#include "../engine/studio.h" //LRC
#include "CGib.h"

// HACKHACK -- The gib velocity equations don't work
void CGib::LimitVelocity()
{
	float length = pev->velocity.Length();

	// ceiling at 1500.  The gib velocity equation is not bounded properly.  Rather than tune it
	// in 3 separate places again, I'll just limit it here.
	if (length > 1500.0)
		pev->velocity = pev->velocity.Normalize() * 1500;		// This should really be sv_maxvelocity * 0.75 or something
}


void CGib::SpawnStickyGibs(entvars_t *pevVictim, Vector vecOrigin, int cGibs)
{
	for (int i = 0; i < cGibs; i++)
	{
		CGib *pGib = GetClassPtr((CGib *)NULL);

		pGib->Spawn("models/stickygib.mdl");
		pGib->pev->body = RANDOM_LONG(0, 2);

		if (pevVictim)
		{
			pGib->pev->origin.x = vecOrigin.x + RANDOM_FLOAT(-3, 3);
			pGib->pev->origin.y = vecOrigin.y + RANDOM_FLOAT(-3, 3);
			pGib->pev->origin.z = vecOrigin.z + RANDOM_FLOAT(-3, 3);

			/*
			pGib->pev->origin.x = pevVictim->absmin.x + pevVictim->size.x * (RANDOM_FLOAT ( 0 , 1 ) );
			pGib->pev->origin.y = pevVictim->absmin.y + pevVictim->size.y * (RANDOM_FLOAT ( 0 , 1 ) );
			pGib->pev->origin.z = pevVictim->absmin.z + pevVictim->size.z * (RANDOM_FLOAT ( 0 , 1 ) );
			*/

			// make the gib fly away from the attack vector
			pGib->pev->velocity = g_vecAttackDir * -1;

			// mix in some noise
			pGib->pev->velocity.x += RANDOM_FLOAT(-0.15, 0.15);
			pGib->pev->velocity.y += RANDOM_FLOAT(-0.15, 0.15);
			pGib->pev->velocity.z += RANDOM_FLOAT(-0.15, 0.15);

			pGib->pev->velocity = pGib->pev->velocity * 900;

			pGib->pev->avelocity.x = RANDOM_FLOAT(250, 400);
			pGib->pev->avelocity.y = RANDOM_FLOAT(250, 400);

			// copy owner's blood color
			pGib->m_bloodColor = (CBaseEntity::Instance(pevVictim))->BloodColor();

			if (pevVictim->health > -50)
			{
				pGib->pev->velocity = pGib->pev->velocity * 0.7;
			}
			else if (pevVictim->health > -200)
			{
				pGib->pev->velocity = pGib->pev->velocity * 2;
			}
			else
			{
				pGib->pev->velocity = pGib->pev->velocity * 4;
			}


			pGib->SetMoveType(MOVETYPE_TOSS);
			pGib->SetSolidType(SOLID_BBOX);
			UTIL_SetSize(pGib->pev, Vector(0, 0, 0), Vector(0, 0, 0));
			pGib->SetTouch(&CGib::StickyGibTouch);
			pGib->SetThink(NULL);
		}
		pGib->LimitVelocity();
	}
}

void CGib::SpawnHeadGib(entvars_t *pevVictim)
{
	SpawnHeadGib(pevVictim, "models/hgibs.mdl");
}

void CGib::SpawnHeadGib(entvars_t *pevVictim, const char* szGibModel)
{
	CGib *pGib = GetClassPtr((CGib *)NULL);

	pGib->Spawn(szGibModel);// throw one head
	pGib->pev->body = 0;

	if (pevVictim)
	{
		pGib->pev->origin = pevVictim->origin + pevVictim->view_ofs;

		edict_t		*pentPlayer = FIND_CLIENT_IN_PVS(pGib->edict());

		if (RANDOM_LONG(0, 100) <= 5 && pentPlayer)
		{
			// 5% chance head will be thrown at player's face.
			entvars_t	*pevPlayer;

			pevPlayer = VARS(pentPlayer);
			pGib->pev->velocity = ((pevPlayer->origin + pevPlayer->view_ofs) - pGib->pev->origin).Normalize() * 300;
			pGib->pev->velocity.z += 100;
		}
		else
		{
			pGib->pev->velocity = Vector(RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(200, 300));
		}


		pGib->pev->avelocity.x = RANDOM_FLOAT(100, 200);
		pGib->pev->avelocity.y = RANDOM_FLOAT(100, 300);

		// copy owner's blood color
		pGib->m_bloodColor = (CBaseEntity::Instance(pevVictim))->BloodColor();

		if (pevVictim->health > -50)
		{
			pGib->pev->velocity = pGib->pev->velocity * 0.7;
		}
		else if (pevVictim->health > -200)
		{
			pGib->pev->velocity = pGib->pev->velocity * 2;
		}
		else
		{
			pGib->pev->velocity = pGib->pev->velocity * 4;
		}
	}
	pGib->LimitVelocity();
}

void CGib::SpawnRandomGibs(entvars_t *pevVictim, int cGibs, const GibData& gibData)
{
	//Track the number of uses of a particular submodel so we can avoid spawning too many of the same
	auto pLimitTracking = gibData.Limits != nullptr ? stackalloc<int[]>(gibData.SubModelCount) : nullptr;

	if (pLimitTracking)
	{
		memset(pLimitTracking, 0, sizeof(int) * gibData.SubModelCount);
	}

	auto currentBody = 0;

	int cSplat;

	for (cSplat = 0; cSplat < cGibs; cSplat++)
	{
		CGib *pGib = GetClassPtr((CGib *)NULL);


		pGib->Spawn(gibData.ModelName);

		if (pLimitTracking)
		{
			if (pLimitTracking[currentBody] >= gibData.Limits[currentBody].MaxGibs)
			{
				++currentBody;
			}

			pGib->pev->body = currentBody;

			++pLimitTracking[currentBody];
		}
		else
		{
			pGib->pev->body = RANDOM_LONG(gibData.FirstSubModel, gibData.SubModelCount - 1);
		}

		if (pevVictim)
		{
			// spawn the gib somewhere in the monster's bounding volume
			pGib->pev->origin.x = pevVictim->absmin.x + pevVictim->size.x * (RANDOM_FLOAT(0, 1));
			pGib->pev->origin.y = pevVictim->absmin.y + pevVictim->size.y * (RANDOM_FLOAT(0, 1));
			pGib->pev->origin.z = pevVictim->absmin.z + pevVictim->size.z * (RANDOM_FLOAT(0, 1)) + 1;	// absmin.z is in the floor because the engine subtracts 1 to enlarge the box

			// make the gib fly away from the attack vector
			pGib->pev->velocity = g_vecAttackDir * -1;

			// mix in some noise
			pGib->pev->velocity.x += RANDOM_FLOAT(-0.25, 0.25);
			pGib->pev->velocity.y += RANDOM_FLOAT(-0.25, 0.25);
			pGib->pev->velocity.z += RANDOM_FLOAT(-0.25, 0.25);

			pGib->pev->velocity = pGib->pev->velocity * RANDOM_FLOAT(300, 400);

			pGib->pev->avelocity.x = RANDOM_FLOAT(100, 200);
			pGib->pev->avelocity.y = RANDOM_FLOAT(100, 300);

			// copy owner's blood color
			pGib->m_bloodColor = (CBaseEntity::Instance(pevVictim))->BloodColor();

			if (pevVictim->health > -50)
			{
				pGib->pev->velocity = pGib->pev->velocity * 0.7;
			}
			else if (pevVictim->health > -200)
			{
				pGib->pev->velocity = pGib->pev->velocity * 2;
			}
			else
			{
				pGib->pev->velocity = pGib->pev->velocity * 4;
			}

			pGib->SetSolidType(SOLID_BBOX);
			UTIL_SetSize(pGib->pev, Vector(0, 0, 0), Vector(0, 0, 0));
		}
		pGib->LimitVelocity();
	}

	stackfree(pLimitTracking);
}

// start at one to avoid throwing random amounts of skulls (0th gib)
const GibData HumanGibs = { "models/hgibs.mdl", 1, HUMAN_GIB_COUNT };
const GibData AlienGibs = { "models/agibs.mdl", 0, ALIEN_GIB_COUNT };

void CGib::SpawnRandomGibs(entvars_t *pevVictim, int cGibs, int human)
{
	SpawnRandomGibs(pevVictim, cGibs, human ? HumanGibs : AlienGibs);
}

//LRC - changed signature, to support custom gib models
void CGib::SpawnRandomGibs(entvars_t *pevVictim, int cGibs, int notfirst, const char *szGibModel)
{
	if (cGibs == 0) return; // spawn nothing!

	CGib *pGib = GetClassPtr((CGib *)NULL);
	pGib->Spawn(szGibModel);

	//LRC - check the model itself to find out how many gibs are available
	studiohdr_t *pstudiohdr = (studiohdr_t *)(GET_MODEL_PTR(ENT(pGib->pev)));
	if (!pstudiohdr)
		return;

	mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)pstudiohdr + pstudiohdr->bodypartindex);
	//ALERT(at_console, "read %d bodyparts, canonical is %d\n", pbodypart->nummodels, HUMAN_GIB_COUNT);

	for (int cSplat = 0; cSplat < cGibs; cSplat++)
	{
		if (pGib == NULL) // first time through, we set pGib before the loop started
		{
			pGib = GetClassPtr((CGib *)NULL);
			pGib->Spawn(szGibModel);
		}

		if (notfirst)
			pGib->pev->body = RANDOM_LONG(1, pbodypart->nummodels - 1);// start at one to avoid throwing random amounts of skulls (0th gib)
		else
			pGib->pev->body = RANDOM_LONG(0, pbodypart->nummodels - 1);

		if (pevVictim)
		{
			// spawn the gib somewhere in the monster's bounding volume
			pGib->pev->origin.x = pevVictim->absmin.x + pevVictim->size.x * (RANDOM_FLOAT(0, 1));
			pGib->pev->origin.y = pevVictim->absmin.y + pevVictim->size.y * (RANDOM_FLOAT(0, 1));
			pGib->pev->origin.z = pevVictim->absmin.z + pevVictim->size.z * (RANDOM_FLOAT(0, 1)) + 1;	// absmin.z is in the floor because the engine subtracts 1 to enlarge the box

			// make the gib fly away from the attack vector
			pGib->pev->velocity = g_vecAttackDir * -1;

			// mix in some noise
			pGib->pev->velocity.x += RANDOM_FLOAT(-0.25, 0.25);
			pGib->pev->velocity.y += RANDOM_FLOAT(-0.25, 0.25);
			pGib->pev->velocity.z += RANDOM_FLOAT(-0.25, 0.25);

			pGib->pev->velocity = pGib->pev->velocity * RANDOM_FLOAT(300, 400);

			pGib->pev->avelocity.x = RANDOM_FLOAT(100, 200);
			pGib->pev->avelocity.y = RANDOM_FLOAT(100, 300);

			// copy owner's blood color
			pGib->m_bloodColor = (CBaseEntity::Instance(pevVictim))->BloodColor();

			if (pevVictim->health > -50)
			{
				pGib->pev->velocity = pGib->pev->velocity * 0.7;
			}
			else if (pevVictim->health > -200)
			{
				pGib->pev->velocity = pGib->pev->velocity * 2;
			}
			else
			{
				pGib->pev->velocity = pGib->pev->velocity * 4;
			}

			pGib->SetSolidType(SOLID_BBOX);
			UTIL_SetSize(pGib->pev, Vector(0, 0, 0), Vector(0, 0, 0));
		}
		pGib->LimitVelocity();
		pGib = NULL; //LRC
	}
}