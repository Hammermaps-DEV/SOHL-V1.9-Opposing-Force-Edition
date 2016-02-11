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

#ifndef TRIGGER_XENTREE_H
#define TRIGGER_XENTREE_H

class CXenTreeTrigger : public CBaseEntity
{
public:
	void		Touch(CBaseEntity *pOther);
	static CXenTreeTrigger *TriggerCreate(edict_t *pOwner, const Vector &position);
};

#endif // TRIGGER_XENTREE_H