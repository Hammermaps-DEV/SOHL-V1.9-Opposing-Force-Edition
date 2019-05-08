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
#ifndef ROPE_CROPESAMPLE_H
#define ROPE_CROPESAMPLE_H

class CRope;

/**
*	Data for a single rope joint.
*/
struct RopeSampleData
{
	Vector mPosition;
	Vector mVelocity;
	Vector mForce;
	Vector mExternalForce;

	bool mApplyExternalForce;

	float mMassReciprocal;
};

/**
*	Represents a single joint in a rope. There are numSegments + 1 samples in a rope.
*/
class CRopeSample : public CBaseEntity
{
public:
	using BaseClass = CBaseEntity;

	void Spawn() override;

	int Save(CSave &save);
	int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	static CRopeSample* CreateSample();

	const RopeSampleData& GetData() const { return m_Data; }

	RopeSampleData& GetData() { return m_Data; }

	CRope* GetMasterRope() { return m_pMasterRope; }

	void SetMasterRope(CRope* pRope)
	{
		m_pMasterRope = pRope;
	}

private:
	RopeSampleData m_Data;
	CRope* m_pMasterRope;
};

#endif
