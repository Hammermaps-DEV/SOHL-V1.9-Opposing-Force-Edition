/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
// Projectile: Spore Grenade for Shocktrooper & Spore Launcher
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

#ifndef SPORE_GRENADE_H
#define SPORE_GRENADE_H

// Contact/Timed spore grenade
class CSporeGrenade : public CBaseMonster {
	public:
		void Spawn(void);
		void Precache(void);
		void Glow(void);
		void Explode(void);

		void EXPORT ExplodeThink(CBaseEntity *pOther);
		void EXPORT BounceThink(CBaseEntity *pOther);
		void EXPORT FlyThink(void);

		static CSporeGrenade *ShootTimed(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time);
		static CSporeGrenade *ShootContact(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	private:
		CSprite *m_pSprite;
};

#endif // SPORE_GRENADE_H