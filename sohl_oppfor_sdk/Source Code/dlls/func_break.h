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
#ifndef FUNC_BREAK_H
#define FUNC_BREAK_H

typedef enum { expRandom, expDirected } Explosions;
typedef enum { matGlass = 0, matWood, matMetal, matFlesh, matCinderBlock, matCeilingTile, matComputer, matUnbreakableGlass, matRocks, matNone, matLastMaterial } Materials;

#define	NUM_SHARDS 6 // this many shards spawned when breakable objects break;
#define SF_BREAKABLE_INVERT 16

class CBreakable : public CBaseDelay
{
public:
	// basic functions
	void Spawn();
	void Precache();
	void KeyValue(KeyValueData* pkvd);
	virtual float CalcRatio(CBaseEntity *pLocus, int mode);//AJH added 'mode' = ratio to return);
	void EXPORT BreakTouch(CBaseEntity *pOther);
	void EXPORT BreakUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT RespawnUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT RespawnThink();
	void EXPORT RespawnFadeThink();
	void DamageSound();
	virtual void DoRespawn(); //AJH Fix for respawnable breakable pushables
	virtual int Classify() { return m_iClass; }

	// breakables use an overridden takedamage
	virtual int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	// To spark when hit
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	BOOL IsBreakable();
	BOOL SparkWhenHit();

	STATE GetState();

	int	 DamageDecal(int bitsDamageType);

	void EXPORT		Die();
	virtual int		ObjectCaps() { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION); }
	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);

	inline BOOL		Explodable() { return ExplosionMagnitude() > 0; }
	inline int		ExplosionMagnitude() { return pev->impulse; }
	inline void		ExplosionSetMagnitude(int magnitude) { pev->impulse = magnitude; }

	static void MaterialSoundPrecache(Materials precacheMaterial);
	static void MaterialSoundRandom(edict_t *pEdict, Materials soundMaterial, float volume);
	static const char **MaterialSoundList(Materials precacheMaterial, int &soundCount);

	static const char *pSoundsWood[];
	static const char *pSoundsFlesh[];
	static const char *pSoundsGlass[];
	static const char *pSoundsMetal[];
	static const char *pSoundsConcrete[];
	static const char *pSpawnObjects[];

	static	TYPEDESCRIPTION m_SaveData[];

	Materials	m_Material;
	Explosions	m_Explosion;
	int			m_idShard;
	float		m_angle;
	int			m_iszGibModel;
	int			m_iszSpawnObject;
	//LRC
	int			m_iRespawnTime;
	int			m_iInitialHealth;
	int			m_iInitialRenderAmt;
	int			m_iInitialRenderMode;
	int			m_iClass; //so that monsters will attack it
	int			m_iszWhenHit; // locus trigger
	CPointEntity	*m_pHitProxy;
};

#endif	// FUNC_BREAK_H
