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

#ifndef SERVER_GUNMAN_UNARMED_H
#define SERVER_GUNMAN_UNARMED_H
#include "talkmonster.h"

enum humanunarmed_bg {
	GROUP_WEAPON = 1,
	GROUP_DECORE
};

const int SF_HUMANUNARMED_NOTSOLID = 4;

class CGunmanUnarmed : public CBaseMonster
{
public:
	void Spawn() override;
	void Precache() override;
	void SetYawSpeed() override;
	int  Classify() override;
	int ISoundMask() override { return	NULL; };

	int	ObjectCaps() override { return CBaseMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;
	void PlayScriptedSentence(const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener) override;

	Schedule_t *GetSchedule() override;

	void KeyValue(KeyValueData *pkvd) override;

	void IdleSound() override;
	void PainSound() override;

	int	Save(CSave &save) override;
	int	Restore(CRestore &restore) override;
	static	TYPEDESCRIPTION m_SaveData[];

	float m_useTime;						// Don't allow +USE until this time
	int	m_iBodygroup;
	int	m_szIdleSentences;
	int	m_szStareSentences;
	int	m_szPainSentences;
	int	m_szUseSentences;
	int	m_szUnuseSentences;
};

#endif // SERVER_GUNMAN_UNARMED_H