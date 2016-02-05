/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#ifndef GRAPPLE_TONGUETIP_H
#define GRAPPLE_TONGUETIP_H

class CGrappleHook : public CBaseEntity
{
public:
	void Spawn(void);
	void Precache(void);
	void EXPORT Move(void);
	void EXPORT Hit(CBaseEntity*);
	void Killed(entvars_t *pev, int gib);

	static	CGrappleHook* Create(Vector Pos, Vector Aim, CBasePlayer* Owner);

	int		m_Chain;
	int		m_iIsMoving;
	int		m_iTrail_Length;
	int		m_iHitMonster;	// Fograin92: Used to handle what monster type we did hit
	BOOL	bPullBack;		// Fograin92: Used to "pull-back" tongue after miss or release

	CBasePlayer *myowner;
	CBaseEntity *myHitMonster;	// Fograin92: Pointer to our monster
	CBeam		*m_pTongue;		// Fograin92: New tongue
};

#endif // GRAPPLE_TONGUETIP_H