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
// NPC: Black Ops * http://half-life.wikia.com/wiki/Black_Ops
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_MASSN_H
#define MONSTER_MASSN_H

// class definition
class CMassn : public CHGrunt {
public:
	int  Classify(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void Sniperrifle(void);

	BOOL FOkToSpeak(void);

	void Spawn(void);
	void Precache(void);

	void IdleSound(void);
};

#endif // MONSTER_MASSN_H