/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
//=========================================================
// Projectile: Bolt for Crossbow
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef PROJ_BOLT_H
#define PROJ_BOLT_H

// class definition
class CBolt : public CBaseEntity {
	void Spawn(void);
	void Precache(void);
	int Classify(void) { return CLASS_NONE; }
	void EXPORT BubbleThink(void);
	void EXPORT BoltTouch(CBaseEntity *pOther);
	void EXPORT ExplodeThink(void);
public:
	static CBolt *BoltCreate(void);
};

#endif // PROJ_BOLT_H