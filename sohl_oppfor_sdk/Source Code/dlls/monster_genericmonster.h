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

#ifndef MONSTER_GENERICMONSTER_H
#define MONSTER_GENERICMONSTER_H

class CGenericMonster : public CTalkMonster
{
public:
	void Spawn(void);
	void Precache(void);
	void SetYawSpeed(void);
	int  Classify(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);
	int ISoundMask(void);
	void KeyValue(KeyValueData *pkvd);
	void Torch(void);
	void MakeGas(void);
	void UpdateGas(void);
	void KillGas(void);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	virtual int HasCustomGibs(void) { return m_iszGibModel; }

	CBeam *m_pBeam;
	int m_iszGibModel;
};

#endif // MONSTER_GENERICMONSTER_H