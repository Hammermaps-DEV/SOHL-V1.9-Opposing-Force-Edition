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

#ifndef CGIB_H
#define CGIB_H

#define GERMAN_GIB_COUNT		4
#define	HUMAN_GIB_COUNT			6
#define ALIEN_GIB_COUNT			4

struct GibLimit
{
	const int MaxGibs;
};

/**
*	@brief Data used to spawn gibs
*/
struct GibData
{
	const char* const ModelName;
	const int FirstSubModel;
	const int SubModelCount;

	/**
	*	@brief Optional list of limits to apply to each submodel
	*	Must be SubModelCount elements large
	*	If used, instead of randomly selecting a submodel each submodel is used until the requested number of gibs have been spawned
	*/
	const GibLimit* const Limits = nullptr;
};

//
// A gib is a chunk of a body, or a piece of wood/metal/rocks/etc.
//
class CGib : public CBaseEntity
{
public:
	void Spawn(const char *szGibModel);
	void EXPORT BounceGibTouch(CBaseEntity *pOther);
	void EXPORT StickyGibTouch(CBaseEntity *pOther);
	void EXPORT WaitTillLand();
	void		LimitVelocity();

	virtual int	ObjectCaps() { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE; }
	static	void SpawnHeadGib(entvars_t *pevVictim);
	static	void SpawnHeadGib(entvars_t *pevVictim, const char *szGibModel);
	static	void SpawnRandomGibs(entvars_t *pevVictim, int cGibs, const GibData& gibData);
	static	void SpawnRandomGibs(entvars_t *pevVictim, int cGibs, int human);
	static	void SpawnRandomGibs(entvars_t *pevVictim, int cGibs, int notfirst, const char *szGibModel); //LRC
	static  void SpawnStickyGibs(entvars_t *pevVictim, Vector vecOrigin, int cGibs);

	int		m_bloodColor;
	int		m_cBloodDecals;
	int		m_material;
	float	m_lifeTime;
};
#endif // CGIB_H
