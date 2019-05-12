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

#ifndef MONSTER_HOUNDEYE_H
#define MONSTER_HOUNDEYE_H

class CHoundeye : public CSquadMonster
{
public:
	void Spawn();
	void Precache();
	int  Classify();
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void SetYawSpeed();
	void WarmUpSound();
	void AlertSound();
	void DeathSound();
	void WarnSound();
	void PainSound();
	void IdleSound();
	void StartTask(Task_t *pTask);
	void RunTask(Task_t *pTask);
	void SonicAttack();
	void PrescheduleThink();
	void SetActivity(Activity NewActivity);
	void WriteBeamColor();
	bool CheckRangeAttack1(float flDot, float flDist);
	BOOL FValidateHintType(short sHint);
	BOOL FCanActiveIdle();
	Schedule_t *GetScheduleOfType(int Type);
	Schedule_t *CHoundeye::GetSchedule();

	int	Save(CSave &save);
	int Restore(CRestore &restore);

	CUSTOM_SCHEDULES;
	static TYPEDESCRIPTION m_SaveData[];

	int m_iSpriteTexture;
	BOOL m_fAsleep;// some houndeyes sleep in idle mode if this is set, the houndeye is lying down
	BOOL m_fDontBlink;// don't try to open/close eye if this bit is set!
	Vector	m_vecPackCenter; // the center of the pack. The leader maintains this by averaging the origins of all pack members.
};

#endif // MONSTER_HOUNDEYE_H