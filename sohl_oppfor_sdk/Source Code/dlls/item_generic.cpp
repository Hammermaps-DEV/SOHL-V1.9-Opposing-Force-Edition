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
// Item: Generic
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"item_generic.h"

//=========================================================
// Link ENTITY
//=========================================================
LINK_ENTITY_TO_CLASS(item_generic, CItemGeneric);

//=========================================================
// Saverestore
//=========================================================
TYPEDESCRIPTION CItemGeneric::m_SaveData[] = {
	DEFINE_FIELD(CItemGeneric, m_iszSequenceName, FIELD_STRING),
};

IMPLEMENT_SAVERESTORE(CItemGeneric, CBaseAnimating);

//=========================================================
// Spawn Generic
//=========================================================
void CItemGeneric::Spawn() {
	Precache();

	SET_MODEL(ENT(pev), const_cast<char*>(STRING(pev->model)));

	UTIL_SetOrigin(this, pev->origin);
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 32));

	pev->takedamage = DAMAGE_NO;
	SetSolidType(SOLID_NOT);
	pev->sequence = -1;
	pev->movetype = 0;
	pev->effects = 0;
	pev->frame = 0;

	// Call startup sequence to look for a sequence to play.
	if (m_iszSequenceName)
	{
		SetThink(&CItemGeneric::StartupThink);
		SetNextThink(0.1);
	}

	if (pev->spawnflags & SF_ITEMGENERIC_DROP_TO_FLOOR)
	{
		if (!DROP_TO_FLOOR(pev->pContainingEntity))
		{
			ALERT(at_error, "Item %s fell out of level at %f,%f,%f", GetClassname(), pev->origin.x, pev->origin.y, pev->origin.z);
			UTIL_Remove(this);
		}
	}
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CItemGeneric::Precache() {
	PRECACHE_MODEL(const_cast<char*>(STRING(pev->model)));
}

//=========================================================
// KeyValue
//=========================================================
void CItemGeneric::KeyValue(KeyValueData* pkvd) {
	if (FStrEq(pkvd->szKeyName, "sequencename")) {
		m_iszSequenceName = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq("skin", pkvd->szKeyName))
	{
		pev->skin = static_cast<int>(atof(pkvd->szValue));
		pkvd->fHandled = true;
	}
	else if (FStrEq("body", pkvd->szKeyName))
	{
		pev->body = atoi(pkvd->szValue);
		pkvd->fHandled = true;
	}
	else
		CBaseAnimating::KeyValue(pkvd);
}

//=========================================================
// StartupThink
//=========================================================
void CItemGeneric::StartupThink() {
	// Try to look for a sequence to play.
	int iSequence = -1;
	iSequence = LookupSequence(STRING(m_iszSequenceName));

	// Validate sequence.
	if (iSequence != -1) {
		pev->sequence = iSequence;
		SetThink(&CItemGeneric::SequenceThink);
		SetNextThink(0.01);
	}
	else {
		// Cancel play sequence.
		SetThink(NULL);
	}
}

//=========================================================
// SequenceThink
//=========================================================
void CItemGeneric::SequenceThink() {
	// Set next think time.
	SetNextThink(0.1);

	// Advance frames and dispatch events.
	StudioFrameAdvance();
	DispatchAnimEvents();

	// Restart sequence 
	if (m_fSequenceFinished) {
		pev->frame = 0;
		ResetSequenceInfo();

		if (!m_fSequenceLoops) {
			// Prevent from calling ItemThink.
			SetThink(NULL);
			m_fSequenceFinished = TRUE;
			return;
		}

		pev->frame = 0;
		ResetSequenceInfo();
	}
}

//=========================================================
// Use
//=========================================================
void CItemGeneric::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	SetThink(&CItemGeneric::SUB_Remove);
	SetNextThink(0.1);
}