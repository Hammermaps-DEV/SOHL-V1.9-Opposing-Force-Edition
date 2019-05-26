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
// NPC: Voltigore * http://half-life.wikia.com/wiki/Voltigore
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef MONSTER_ALIEN_VOLTIGORE_H
#define MONSTER_ALIEN_VOLTIGORE_H

#define	VOLTIGORE_MAX_BEAMS	 3

class CVoltigore : public CSquadMonster {
public:
	virtual void Spawn();
	virtual void Precache();
	virtual void SetYawSpeed();
	virtual void DieBlast();
	virtual int  ISoundMask();
	virtual int  Classify();
	virtual void HandleAnimEvent(MonsterEvent_t *pEvent);
	virtual void IdleSound();
	virtual void PainSound();
	virtual void DeathSound();
	virtual void AlertSound();
	virtual void AttackSound();
	virtual void StartTask(Task_t *pTask);
	virtual bool CheckMeleeAttack1(float flDot, float flDist);
	virtual bool CheckMeleeAttack2(float flDot, float flDist);
	virtual bool CheckRangeAttack1(float flDot, float flDist);
	virtual void RunAI();
	BOOL FValidateHintType(short sHint);
	Schedule_t *GetSchedule() override;
	Schedule_t *GetScheduleOfType(int Type) override;
	virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int IgnoreConditions();
	MONSTERSTATE GetIdealState() override;
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

	virtual bool	CanThrowEnergyBall()	const { return true; }
	virtual bool	CanGib()				const { return true; }
	virtual void	PrintBloodDecal(CBaseEntity* pHurt, const Vector& vecOrigin, const Vector& vecVelocity, float maxDist, int bloodColor = DONT_BLEED);


	void CreateBeams();
	void ClearBeams();
	void UpdateBeams();

	void CreateGlow();
	void DestroyGlow();
	void GlowUpdate();
	void GlowOff();
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