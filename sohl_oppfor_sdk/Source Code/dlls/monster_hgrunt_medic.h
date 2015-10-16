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

#ifndef MONSTER_HGRUNT_MEDIC_H
#define MONSTER_HGRUNT_MEDIC_H

#include "monster_hgrunt_opfor.h"

class CMedic : public CHFGrunt
{
public:
	void Spawn(void);
	void Precache(void);
	void SetYawSpeed(void);
	int  ISoundMask(void);
	int  Classify(void);
	void CheckAmmo(void);
	void KeyValue(KeyValueData *pkvd);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	void SetActivity(Activity NewActivity);
	void RunTask(Task_t *pTask);
	void StartTask(Task_t *pTask);
	virtual int	ObjectCaps(void) { return CRCAllyMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
	BOOL FCanCheckAttacks(void);
	BOOL CheckRangeAttack1(float flDot, float flDist);
	BOOL CheckRangeAttack2(float flDot, float flDist);
	BOOL CheckMeleeAttack1(float flDot, float flDist);
	void DeclineFollowing(void);
	void PrescheduleThink(void);
	Vector GetGunPosition(void);
	void Shoot(void);
	void Pistol(void);

	void HealerFollow(CBaseEntity *pHealTarget);

	BOOL CanHeal(void); // Can we heal the player, or the injured grunt?
	void Heal(void);// Lets apply the healing.

					// Override these to set behavior
	CBaseEntity	*Kick(void);
	Schedule_t *GetScheduleOfType(int Type);
	Schedule_t *GetSchedule(void);
	MONSTERSTATE GetIdealState(void);

	void DeathSound(void);
	void PainSound(void);
	void GibMonster(void);
	void TalkInit(void);

	BOOL FOkToSpeak(void);
	void JustSpoke(void);

	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);

	int IRelationship(CBaseEntity *pTarget);
	void Killed(entvars_t *pevAttacker, int iGib);

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	// UNDONE: What is this for?  It isn't used?
	float	m_flPlayerDamage;// how much pain has the player inflicted on me?

							 // checking the feasibility of a grenade toss is kind of costly, so we do it every couple of seconds,
							 // not every server frame.
	float m_flNextGrenadeCheck;
	float m_flNextPainTime;
	float m_flLastEnemySightTime;

	Vector	m_vecTossVelocity;

	BOOL	m_fThrowGrenade;
	BOOL	m_fStanding;
	BOOL	m_fFirstEncounter;// only put on the handsign show in the squad's first encounter.
	int		m_cClipSize;

	BOOL	m_fDepleteLine;

	int		m_iHead;
	int		m_iBrassShell;
	int		m_iSentence;
	float	m_flHealAnount;

	static const char *pGruntSentences[];

	CUSTOM_SCHEDULES;

private:
	float m_healTime;
};

class CMedicRepel : public CHFGruntRepel
{
public:
	void Spawn(void);
	void Precache(void);
	void EXPORT RepelUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int m_iSpriteTexture;	// Don't save, precache
};

#endif // MONSTER_HGRUNT_MEDIC_H