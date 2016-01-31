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
// NPC: Cleansuit - Scientist
// http://half-life.wikia.com/wiki/Black_Mesa_Science_Team
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_CLEANSUIT_SCIENTIST_H
#define MONSTER_CLEANSUIT_SCIENTIST_H

class CCleansuitScientist : public CScientist {
	public:
		void Spawn(void);
		void Precache(void);
		void RunTask(Task_t *pTask);
		void StartTask(Task_t *pTask);

		CUSTOM_SCHEDULES;
};

#endif // MONSTER_CLEANSUIT_SCIENTIST_H