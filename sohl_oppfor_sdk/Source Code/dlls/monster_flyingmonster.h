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

#ifndef MONSTER_FLYINGMONSTER_H
#define MONSTER_FLYINGMONSTER_H

class CFlyingMonster : public CBaseMonster
{
public:
	int 		CheckLocalMove(const Vector &vecStart, const Vector &vecEnd, CBaseEntity *pTarget, float *pflDist);// check validity of a straight move through space
	BOOL		FTriangulate(const Vector &vecStart, const Vector &vecEnd, float flDist, CBaseEntity *pTargetEnt, Vector *pApex);
	Activity	GetStoppedActivity();
	void		Killed(entvars_t *pevAttacker, int iGib);
	void		Stop();
	float		ChangeYaw(int speed);
	void		HandleAnimEvent(MonsterEvent_t *pEvent);
	void		MoveExecute(CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval);
	void		Move(float flInterval = 0.1);
	BOOL		ShouldAdvanceRoute(float flWaypointDist);

	inline void	SetFlyingMomentum(float momentum) { m_momentum = momentum; }
	inline void	SetFlyingFlapSound(const char *pFlapSound) { m_pFlapSound = pFlapSound; }
	inline void	SetFlyingSpeed(float speed) { m_flightSpeed = speed; }
	float		CeilingZ(const Vector &position);
	float		FloorZ(const Vector &position);
	BOOL		ProbeZ(const Vector &position, const Vector &probe, float *pFraction);


	// UNDONE:  Save/restore this stuff!!!
protected:
	Vector		m_vecTravel;		// Current direction
	float		m_flightSpeed;		// Current flight speed (decays when not flapping or gliding)
	float		m_stopTime;			// Last time we stopped (to avoid switching states too soon)
	float		m_momentum;			// Weight for desired vs. momentum velocity
	const char	*m_pFlapSound;
};

#endif // MONSTER_FLYINGMONSTER_H