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

#ifndef MONSTER_CONSTRUCTION_H
#define MONSTER_CONSTRUCTION_H

class CConstruction : public CScientist
{
	public:
		void Spawn(void);
		void Precache(void);
		void RunTask(Task_t *pTask);
		void StartTask(Task_t *pTask);

		void TalkInit(void);
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