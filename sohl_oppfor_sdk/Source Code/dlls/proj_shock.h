/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

class CShock : public CBaseEntity
{
public:
	void Spawn(void);
	void Precache(void);

	static void Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void Touch(CBaseEntity *pOther);
	void EXPORT ShockThink(void);

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	void EXPORT FadeShock(void);
	void Glow(void);

	void CreateBeam(const Vector& start, const Vector& end, int width);
	void ClearBeam();
	void UpdateBeam(const Vector& start, const Vector& end);
	void ComputeBeamPositions(const Vector& vel, Vector* pos1, Vector* pos2);

	CBeam *m_pBeam;
	CSprite *m_pSprite;
	Vector m_vecBeamStart, m_vecBeamEnd;
	CBaseEntity *pShockedEnt;
};