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
// Projectile: Spore Grenade for Shocktrooper & Spore Launcher
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef SPORE_GRENADE_H
#define SPORE_GRENADE_H

// Contact/Timed spore grenade
class CSporeGrenade : public CBaseMonster {
public:
	void Spawn();
	void Precache();
	void Glow();
	void Explode();

	void EXPORT ExplodeThink(CBaseEntity *pOther);
	void EXPORT BounceThink(CBaseEntity *pOther);
	void EXPORT FlyThink();

	static CSporeGrenade *ShootTimed(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time);
	static CSporeGrenade *ShootContact(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
private:
	CSprite *m_pSprite;
};

#endif // SPORE_GRENADE_H