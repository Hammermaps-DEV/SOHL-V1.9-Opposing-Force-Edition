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
#ifndef PLANE_H
#define PLANE_H

//=========================================================
// Plane
//=========================================================
class CPlane
{
public:
	CPlane();

	//=========================================================
	// InitializePlane - Takes a normal for the plane and a
	// point on the plane and 
	//=========================================================
	void InitializePlane(const Vector &vecNormal, const Vector &vecPoint);

	//=========================================================
	// PointInFront - determines whether the given vector is 
	// in front of the plane. 
	//=========================================================
	BOOL PointInFront(const Vector &vecPoint);

	Vector	m_vecNormal;
	float	m_flDist;
	BOOL	m_fInitialized;
};

#endif // PLANE_H
