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

#ifndef MONSTER_CONSTRUCTION_H
#define MONSTER_CONSTRUCTION_H

class CConstruction : public CScientist
{
public:
	void Spawn();
	void Precache();
	void RunTask(Task_t *pTask);
	void StartTask(Task_t *pTask);

	void TalkInit();
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void MoveExecute(CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval);

	CUSTOM_SCHEDULES;

private:
	bool m_flDebug;
	float m_flHitgroupHead;
	float m_flHitgroupChest;
	float m_flHitgroupStomach;
	float m_flHitgroupArm;
	float m_flHitgroupLeg;
	float m_flDmgPunch;
};

#endif // MONSTER_CONSTRUCTION_H