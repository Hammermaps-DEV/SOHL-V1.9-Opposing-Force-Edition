/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Spirit of Half-Life and their logos are the property of their respective owners.
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
*   All Rights Reserved.
*
*	Base Source-Code written by Raven City and Marc-Antoine Lortie (https://github.com/malortie).
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/

#define MAX_ROPE_SEGMENTS 64

class CRope : public CBaseEntity {
public:
	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	void KeyValue(KeyValueData* pkvd);
	void Spawn(void);
	void Precache(void);

	void EXPORT StartupThink(void);
	void EXPORT RopeThink(void);

	void CreateSegments();
	void DestroySegments();

	int FindClosestSegment(Vector vecTo, float epsilon, int iMin, int iMax);

	CRopeSegment*	m_pSegments[MAX_ROPE_SEGMENTS];

	int				m_nSegments;
	BOOL			m_fEnabled;

	string_t	m_iszBodyModel;
	string_t	m_iszEndingModel;

	void ApplyFunctor(void(*functor)(CRopeSegment* pSegment), int startIndex, int endIndex);
	void ApplyFunctor(void(*functor)(CRopeSegment* pSegment));

	int j;
	int m_iSegments;
	int m_zforce;
	int m_yforce;
	int m_xforce;
	int m_loop = 0;
	int m_disable = 0;

	float m_ilightningfrequency;
	float m_ibodysparkfrequency;
	float m_isparkfrequency;
};
