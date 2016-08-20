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

#ifndef HVR_ROCKET_H
#define HVR_ROCKET_H

class CApacheHVR : public CGrenade
{
	void Spawn(void);
	void Precache(void);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT IgniteThink(void);
	void EXPORT AccelerateThink(void);

	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int m_iTrail;
	Vector m_vecForward;
};

#endif // HVR_ROCKET_H