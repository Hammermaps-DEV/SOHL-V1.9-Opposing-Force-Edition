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

class CRpgRocket : public CGrenade {
public:
	int	Save(CSave &save);
	int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];
	void Spawn(void);
	void Precache(void);
	void EXPORT FollowThink(void);
	void EXPORT IgniteThink(void);
	void EXPORT RocketTouch(CBaseEntity *pOther);
	void Detonate(void);
	void CreateTrail(void);
	static CRpgRocket *Create(Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner, CRpg *pLauncher);

	int m_iTrail;
	float m_flIgniteTime;
	CRpg *m_pLauncher;// pointer back to the launcher that fired me. 
	BOOL b_setup;
};
