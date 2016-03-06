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

//========================================
//Rope
//========================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"player.h"
#include	"weapons.h"
#include	"env_rope_segment.h"

#define clamp( val, min, max ) ( ((val) > (max)) ? (max) : ( ((val) < (min)) ? (min) : (val) ) )

LINK_ENTITY_TO_CLASS(rope_segment, CRopeSegment);

TYPEDESCRIPTION CRopeSegment::m_SaveData[] =
{
	DEFINE_FIELD(CRopeSegment, m_pNext, FIELD_CLASSPTR),
	DEFINE_FIELD(CRopeSegment, m_pPrev, FIELD_CLASSPTR),
	DEFINE_FIELD(CRopeSegment, m_vecJointPos, FIELD_POSITION_VECTOR),
};

IMPLEMENT_SAVERESTORE(CRopeSegment, CBaseEntity);


void CRopeSegment::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/rope16.mdl");

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->gravity = 0.0f;

	m_pPrev = NULL;
	m_pNext = NULL;
	m_vecJointPos = Vector(0, 0, 0);

	SetTouch(&CRopeSegment::Touch);
	SetThink(&CRopeSegment::SegmentThink);
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CRopeSegment::SegmentThink(void)
{
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CRopeSegment::SegmentTouch(CBaseEntity* pOther)
{
	ALERT(at_console, "Touched segment!\n");

	SetTouch(NULL);
}