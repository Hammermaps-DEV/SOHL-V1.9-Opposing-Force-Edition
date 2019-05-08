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
#ifndef ROPE_CELECTRIFIEDWIRE_H
#define ROPE_CELECTRIFIEDWIRE_H

#include "CRope.h"

/**
*	An electrified wire.
*	Can be toggled on and off. Starts on.
*/
class CElectrifiedWire : public CRope
{
public:
	using BaseClass = CRope;

	CElectrifiedWire();

	void KeyValue(KeyValueData* pkvd) override;

	void Precache() override;

	void Spawn() override;

	void Think() override;

	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float flValue) override;

	int Save(CSave &save) override;
	int Restore(CRestore &restore) override;

	static TYPEDESCRIPTION m_SaveData[];

	/**
	*	@return Whether the wire is active.
	*/
	bool IsActive() const { return m_bIsActive; }

	/**
	*	@param iFrequency Frequency.
	*	@return Whether the spark effect should be performed.
	*/
	bool ShouldDoEffect(const int iFrequency);

	/**
	*	Do spark effects.
	*/
	void DoSpark(const size_t uiSegment, const bool bExertForce);

	/**
	*	Do lightning effects.
	*/
	void DoLightning();

public:
	bool m_bIsActive;

	int m_iTipSparkFrequency;
	int m_iBodySparkFrequency;
	int m_iLightningFrequency;

	int m_iXJoltForce;
	int m_iYJoltForce;
	int m_iZJoltForce;

	size_t m_uiNumUninsulatedSegments;
	size_t m_uiUninsulatedSegments[MAX_SEGMENTS];

	int m_iLightningSprite;

	float m_flLastSparkTime;
};

#endif
