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
*	Base Source-Code written by Marc-Antoine Lortie (https://github.com/malortie).
*   Update Source-Code written by SamVanheer (https://github.com/SamVanheer).
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/

#ifndef MONSTER_LOADER_H
#define MONSTER_LOADER_H

#include "soundent.h"

#define	SF_LOADER_NOTSOLID	4 

class CLoader : public CBaseMonster
{
public:
	void Spawn() override;
	void Precache() override;
	void SetYawSpeed() override;
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) override;
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override;
	void StartTask(Task_t* pTask) override;
	void SetTurnActivity() override;
	int Classify() override { return m_iClass ? m_iClass : CLASS_PLAYER_ALLY; }
	int ISoundMask() override { return bits_SOUND_NONE; }
};

#endif // MONSTER_LOADER_H