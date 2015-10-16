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

#ifndef MONSTER_CLEANSUIT_SCIENTIST_H
#define MONSTER_CLEANSUIT_SCIENTIST_H

#include "monster_scientist.h"

class CCleansuitScientist : public CScientist
{
public:
	void Spawn(void);
	void Precache(void);
};

#endif // MONSTER_CLEANSUIT_SCIENTIST_H