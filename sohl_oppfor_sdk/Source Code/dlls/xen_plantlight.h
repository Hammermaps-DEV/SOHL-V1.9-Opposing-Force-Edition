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

#ifndef XEN_LIGHT_H
#define XEN_LIGHT_H

class CXenPLight : public CActAnimating {
public:
	void		Spawn(void);
	void		Precache(void);
	void		Touch(CBaseEntity *pOther);
	void		Think(void);

	void		LightOn(void);
	void		LightOff(void);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

private:
	CSprite		*m_pGlow;
};
#endif // XEN_LIGHT_H