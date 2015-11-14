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
// Projectile: Spit for PitDrone
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#ifndef PIDDRONESPLIT_H
#define PIDDRONESPLIT_H

class CPitDroneSpit : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	int	 Classify(void);
	int  IRelationship(CBaseEntity *pTarget);

	void IgniteTrail(void);
	void EXPORT StartTrack(void);
	void EXPORT StartDart(void);
	void EXPORT TrackTarget(void);
	void EXPORT TrackTouch(CBaseEntity *pOther);
	void EXPORT DieTouch(CBaseEntity *pOther);

	static const char *pHitSounds[];
	static const char *pMissSounds[];

	int iDroneSpitTrail;
};

#endif // PIDDRONESPLIT_H
