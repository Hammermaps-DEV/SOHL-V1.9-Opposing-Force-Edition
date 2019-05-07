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
//=========================================================
// NPC: Barney * http://half-life.wikia.com/wiki/Barney_Calhoun
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef MONSTER_BARNEY_H
#define MONSTER_BARNEY_H

// include
#include "talkmonster.h"

// class definition
class CBarney : public CTalkMonster {
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void SetYawSpeed(void);
	virtual int  ISoundMask(void);
	virtual void FirePistol(void);
	virtual void AlertSound(void);
	virtual int  Classify(void);
	virtual void HandleAnimEvent(MonsterEvent_t *pEvent);
	virtual void KeyValue(KeyValueData *pkvd);

	virtual void RunTask(Task_t *pTask);
	virtual void StartTask(Task_t *pTask);
	virtual int	ObjectCaps(void) { return CTalkMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
	virtual int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	virtual BOOL CheckRangeAttack1(float flDot, float flDist);

	virtual void DeclineFollowing(void);

	virtual Schedule_t *GetScheduleOfType(int Type);
	virtual Schedule_t *GetSchedule(void);
	virtual MONSTERSTATE GetIdealState(void);

	virtual void DeathSound(void);
	virtual void PainSound(void);

	virtual void TalkInit(void);

	virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual void Killed(entvars_t *pevAttacker, int iGib);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	static const char *pPainSounds[];
	static const char *pDeathSounds[];
	static const char *pAttackSounds[];

	int		m_iBaseBody;
	BOOL	m_fGunDrawn;
	float	m_painTime;
	float	m_checkAttackTime;
	BOOL	m_lastAttackCheck;
	float	m_flPlayerDamage;

	CUSTOM_SCHEDULES;
protected:
	BOOL m_flDebug = false;
	float m_flHitgroupHead;
	float m_flHitgroupChest;
	float m_flHitgroupStomach;
	float m_flHitgroupArm;
	float m_flHitgroupLeg;
	int	  m_iBrassShell;
	int	  head;
};

#endif // MONSTER_BARNEY_H