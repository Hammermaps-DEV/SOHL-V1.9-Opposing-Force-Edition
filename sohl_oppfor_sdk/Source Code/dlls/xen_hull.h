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

#ifndef XEN_HULL_H
#define XEN_HULL_H

class CXenHull : public CPointEntity {
	public:
		static CXenHull	*CreateHull(CBaseEntity *source, const Vector &mins, const Vector &maxs, const Vector &offset);
		int			Classify(void) { return CLASS_BARNACLE; }
};

#endif // XEN_HULL_H