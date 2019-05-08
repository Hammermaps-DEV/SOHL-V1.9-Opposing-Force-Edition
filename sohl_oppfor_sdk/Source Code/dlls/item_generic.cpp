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
#include	"schedule.h"
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
void CItemGeneric::Spawn(void) {
	Precache();
	SET_MODEL(ENT(pev), (char*)STRING(pev->model));

	UTIL_SetOrigin(this, pev->origin);
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 32));

	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_BBOX;
	pev->sequence = -1;

	// Call startup sequence to look for a sequence to play.
	SetThink(&CItemGeneric::StartupThink);

	SetNextThink(0.1);
}

//=========================================================
// Precache - precaches all resources this weapon needs
//=========================================================
void CItemGeneric::Precache(void) {
	PRECACHE_MODEL((char*)STRING(pev->model));
}

//=========================================================
// KeyValue
//=========================================================
void CItemGeneric::KeyValue(KeyValueData* pkvd) {
	if (FStrEq(pkvd->szKeyName, "sequencename")) {
		m_iszSequenceName = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseAnimating::KeyValue(pkvd);
}

//=========================================================
// StartupThink
//=========================================================
void CItemGeneric::StartupThink(void) {
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
void CItemGeneric::SequenceThink(void) {
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
		else {
			pev->frame = 0;
			ResetSequenceInfo();
		}
	}
}