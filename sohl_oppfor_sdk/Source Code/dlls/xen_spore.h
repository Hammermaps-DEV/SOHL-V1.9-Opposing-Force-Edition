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