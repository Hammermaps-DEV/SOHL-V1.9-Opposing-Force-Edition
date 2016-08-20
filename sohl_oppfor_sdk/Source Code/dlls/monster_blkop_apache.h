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
// NPC: AH-64 Apache * http://half-life.wikia.com/wiki/AH-64_Apache
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_BLKOP_APACHE_H
#define MONSTER_BLKOP_APACHE_H

class CBlackOPApache : public CApache
{
public:
	void Spawn(void);
	void Precache(void);
	int  Classify(void);
};

#endif // MONSTER_BLKOP_APACHE_H