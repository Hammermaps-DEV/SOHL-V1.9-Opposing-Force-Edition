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
// NPC: Voltigore * http://half-life.wikia.com/wiki/Voltigore
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef MONSTER_ALIEN_VOLTIGORE_H
#define MONSTER_ALIEN_VOLTIGORE_H

#define	VOLTIGORE_MAX_BEAMS	 3

class CVoltigore : public CSquadMonster {
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	void SetYawSpeed(void);
	void DieBlast(void);
	int  ISoundMask(void);
	virtual int  Classify(void);
	virtual void HandleAnimEvent(MonsterEvent_t *pEvent);
	virtual void IdleSound(void);
	virtual void PainSound(void);
	virtual void DeathSound(void);
	virtual void AlertSound(void);
	virtual void AttackSound(void);
	virtual void StartTask(Task_t *pTask);
	virtual BOOL CheckMeleeAttack1(float flDot, float flDist);
	virtual BOOL CheckMeleeAttack2(float flDot, float flDist);
	virtual BOOL CheckRangeAttack1(float flDot, float flDist);
	virtual void RunAI(void);
	BOOL FValidateHintType(short sHint);
	Schedule_t *GetSchedule(void);
	Schedule_t *GetScheduleOfType(int Type);
	virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int IgnoreConditions(void);
	MONSTERSTATE GetIdealState(void);
	virtual void Killed(entvars_t *pevAttacker, int iGib);
	void CallForHelp(char *szClassname, float flDist, EHANDLE hEnemy, Vector &vecLocation);

	int	Save(CSave &save);
	int Restore(CRestore &restore);

	CUSTOM_SCHEDULES;
	static TYPEDESCRIPTION m_SaveData[];

	float m_flLastHurtTime;
	float m_flNextSpitTime; // last time the voltigore used the spit attack.
	BOOL m_fShouldUpdateBeam;
	CBeam *m_pBeam[VOLTIGORE_MAX_BEAMS];
	int m_iBeams;

	CSprite *m_pBeamGlow;
	CSprite *m_pSprite;
	CBeam *pBeam;
	int m_glowBrightness;
	int m_iVgib;

	static const char* pAlertSounds[];
	static const char* pAttackMeleeSounds[];
	static const char* pMeleeHitSounds[];
	static const char* pMeleeMissSounds[];
	static const char* pComSounds[];
	static const char* pDeathSounds[];
	static const char* pFootstepSounds[];
	static const char* pIdleSounds[];
	static const char* pPainSounds[];
	static const char* pGruntSounds[];

	virtual BOOL	CanThrowEnergyBall(void)	const { return TRUE; }
	virtual BOOL	CanGib(void)				const { return TRUE; }
	virtual void	PrintBloodDecal(CBaseEntity* pHurt, const Vector& vecOrigin, const Vector& vecVelocity, float maxDist, int bloodColor = DONT_BLEED);


	void CreateBeams();
	void ClearBeams();
	void UpdateBeams();

	void CreateGlow();
	void DestroyGlow();
	void GlowUpdate();
	void GlowOff(void);
	void GlowOn(int level);

protected:
	BOOL m_flDebug = false;
	float m_flHitgroupHead;
	float m_flHitgroupChest;
	float m_flHitgroupStomach;
	float m_flHitgroupArm;
	float m_flHitgroupLeg;
	float m_flDmgPunch;
};

#endif // MONSTER_ALIEN_VOLTIGORE_H