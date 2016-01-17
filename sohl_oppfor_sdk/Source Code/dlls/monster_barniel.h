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
// NPC: Female Barney * Barniel * http://half-life.wikia.com/wiki/Barney_Calhoun
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_BARNIEL_H
#define MONSTER_BARNIEL_H

// include
#include "monster_barney.h"

// class definition
class CBarniel : public CBarney {
	public:
		void Spawn(void);
		void Precache(void);
		void DeathSound(void);
		void PainSound(void);
		void AlertSound(void);
		void Fire9mmPistol(void);
		void TalkInit(void);

		Schedule_t *GetSchedule(void);

		static const char *pPainSounds[];
		static const char *pDeathSounds[];
		static const char *pAttackSounds[];

		int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
		void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
};

#endif // MONSTER_BARNIEL_H