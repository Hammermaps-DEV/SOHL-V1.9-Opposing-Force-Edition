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

#ifndef XEN_SPORE_H
#define XEN_SPORE_H

class CXenSpore : public CActAnimating {
public:
	void		Spawn(void);
	void		Precache(void);
	void		Think(void);
	int			TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) { Attack(); return 0; }
	void		Attack(void) {}

	static const char *pModelNames[];
};

class CXenSporeSmall : public CXenSpore {
	void Spawn(void);
};

class CXenSporeMed : public CXenSpore {
	void Spawn(void);
};

class CXenSporeLarge : public CXenSpore {
	void Spawn(void);
	static const Vector m_hullSizes[];
};

#endif // XEN_SPORE_H