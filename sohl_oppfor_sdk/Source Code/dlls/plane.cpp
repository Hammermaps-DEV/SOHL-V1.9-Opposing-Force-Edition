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
#include "plane.h"

//=========================================================
// Plane
//=========================================================
CPlane::CPlane(void)
{
	m_fInitialized = FALSE;
}

//=========================================================
// InitializePlane - Takes a normal for the plane and a
// point on the plane and 
//=========================================================
void CPlane::InitializePlane(const Vector &vecNormal, const Vector &vecPoint)
{
	m_vecNormal = vecNormal;
	m_flDist = DotProduct(m_vecNormal, vecPoint);
	m_fInitialized = TRUE;
}


//=========================================================
// PointInFront - determines whether the given vector is 
// in front of the plane. 
//=========================================================
BOOL CPlane::PointInFront(const Vector &vecPoint)
{
	float flFace;

	if (!m_fInitialized)
	{
		return FALSE;
	}

	flFace = DotProduct(m_vecNormal, vecPoint) - m_flDist;

	if (flFace >= 0)
	{
		return TRUE;
	}

	return FALSE;
}

