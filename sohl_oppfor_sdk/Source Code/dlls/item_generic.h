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

#ifndef ITEM_GENERIC_H
#define ITEM_GENERIC_H

const auto SF_ITEMGENERIC_DROP_TO_FLOOR = 1 << 0;

class CItemGeneric : public CBaseAnimating {
public:
	int		Save(CSave &save) override;
	int		Restore(CRestore &restore) override;

	static	TYPEDESCRIPTION m_SaveData[];
	void	Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;

	void Spawn() override;
	void Precache() override;
	void KeyValue(KeyValueData* pkvd) override;

	void EXPORT StartupThink();
	void EXPORT SequenceThink();

	string_t m_iszSequenceName;
};
#endif // ITEM_GENERIC_H
