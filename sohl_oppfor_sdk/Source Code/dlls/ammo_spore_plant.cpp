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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "baseactanimating.h"
#include "proj_sporegrenade.h"

#define		SACK_GROUP			1
#define		SACK_EMPTY			0
#define		SACK_FULL			1

class CSporeAmmoPlant : public CBaseEntity
{
public:
	void Spawn() override;
	void Precache() override;
	void EXPORT BornThink();
	void EXPORT IdleThink();
	void EXPORT AmmoTouch(CBaseEntity *pOther);
	int  TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) override;

	int	Save(CSave &save) override;
	int Restore(CRestore &restore) override;
	static TYPEDESCRIPTION m_SaveData[];

	int m_iExplode;
	bool  m_borntime;
	float m_flTimeSporeIdle;
};

enum
{
	SPOREAMMO_IDLE,
	SPOREAMMO_SPAWNUP,
	SPOREAMMO_SNATCHUP,
	SPOREAMMO_SPAWNDOWN,
	SPOREAMMO_SNATCHDOWN,
	SPOREAMMO_IDLE1,
	SPOREAMMO_IDLE2,
};

LINK_ENTITY_TO_CLASS(ammo_spore_plant, CSporeAmmoPlant);

TYPEDESCRIPTION	CSporeAmmoPlant::m_SaveData[] =
{
	DEFINE_FIELD(CSporeAmmoPlant, m_flTimeSporeIdle, FIELD_TIME),
	DEFINE_FIELD(CSporeAmmoPlant, m_borntime, FIELD_BOOLEAN),
};
IMPLEMENT_SAVERESTORE(CSporeAmmoPlant, CBaseEntity);

//=========================================================
// Spawn
//=========================================================
void CSporeAmmoPlant::Spawn() {
	Precache();
	SetModel("models/spore_ammo.mdl");

	UTIL_SetSize(pev, Vector(-20, -20, -8), Vector(20, 20, 16));
	UTIL_SetOrigin(this, pev->origin);

	SetSolidType(SOLID_SLIDEBOX);
	SetMoveType(MOVETYPE_FLY);
	pev->framerate = 1.0;
	pev->animtime = UTIL_GlobalTimeBase() + 0.1;
	m_borntime = true;

	pev->sequence = SPOREAMMO_IDLE;
	pev->body = 0;

	SetThink(&CSporeAmmoPlant::BornThink);
	SetTouch(&CSporeAmmoPlant::AmmoTouch);

	m_flTimeSporeIdle = UTIL_GlobalTimeBase() + 22;
	SetNextThink(0.1);
}

//=========================================================
// Precache
//=========================================================
void CSporeAmmoPlant::Precache()
{
	PRECACHE_MODEL("models/spore_ammo.mdl");
	m_iExplode = PRECACHE_MODEL("sprites/spore_exp_c_01.spr");
	PRECACHE_SOUND("weapons/spore_ammo.wav");
	UTIL_PrecacheOther("spore");
}

//=========================================================
// Override all damage
//=========================================================
int CSporeAmmoPlant::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) {
	return TRUE;
}

void CSporeAmmoPlant::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!m_borntime) {
		Vector vecSrc = pev->origin + gpGlobals->v_forward * -20;

		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_EXPLOSION);		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD(vecSrc.x);	// Send to PAS because of the sound
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_SHORT(m_iExplode);
		WRITE_BYTE(25); // scale * 10
		WRITE_BYTE(12); // framerate
		WRITE_BYTE(TE_EXPLFLAG_NOSOUND);
		MESSAGE_END();

		ALERT(at_aiconsole, "angles %f %f %f\n", pev->angles.x, pev->angles.y, pev->angles.z);
		Vector angles = pev->angles + gpGlobals->v_forward * 17 - gpGlobals->v_right * 27 + gpGlobals->v_up * 6;

		if (fabs(angles.x) != 0)
			angles.x = angles.x + 90;
		if (fabs(angles.y) != 0)
			angles.y = angles.y + 90;
		if (fabs(angles.z) != 0)
			angles.y = angles.y + 90;

		ALERT(at_aiconsole, "angles %f %f %f\n", angles.x, angles.y, angles.z);
		CSporeGrenade::ShootTimed(this->pev, vecSrc, angles, RANDOM_FLOAT(3, 5));

		pev->framerate = 1.0;
		pev->animtime = UTIL_GlobalTimeBase() + 0.1;
		pev->sequence = SPOREAMMO_SNATCHDOWN;
		pev->body = 0;
		m_borntime = true;
		m_flTimeSporeIdle = UTIL_GlobalTimeBase() + 1;
		SetThink(&CSporeAmmoPlant::IdleThink);
	}

	AddMultiDamage(pevAttacker, this, flDamage, bitsDamageType);
}

//=========================================================
// Thinking begin
//=========================================================
void CSporeAmmoPlant::BornThink()
{
	SetNextThink(0.1);
	if (m_flTimeSporeIdle > UTIL_GlobalTimeBase())
		return;

	pev->sequence = 3;
	pev->framerate = 1.0;
	pev->animtime = UTIL_GlobalTimeBase() + 0.1;
	pev->body = 1;
	m_borntime = false;
	SetThink(&CSporeAmmoPlant::IdleThink);

	m_flTimeSporeIdle = UTIL_GlobalTimeBase() + 16;
}

void CSporeAmmoPlant::IdleThink()
{
	SetNextThink(0.1);
	if (m_flTimeSporeIdle > UTIL_GlobalTimeBase())
		return;

	if (m_borntime) {
		pev->sequence = SPOREAMMO_IDLE;
		m_flTimeSporeIdle = UTIL_GlobalTimeBase() + 18;
		SetThink(&CSporeAmmoPlant::BornThink);
		return;
	}

	pev->sequence = SPOREAMMO_IDLE1;
}

//=========================================================
// AddAmmo
//=========================================================
void CSporeAmmoPlant::AmmoTouch(CBaseEntity *pOther)
{
	Vector	vecSpot;
	TraceResult	tr;

	if (pOther->pev->velocity == g_vecZero || !pOther->IsPlayer())
		return;

	if (m_borntime)
		return;

	const bool bResult = (pOther->GiveAmmo(AMMO_SPORE_GIVE, "spore", SPORE_MAX_CARRY) != -1);
	if (bResult)
	{
		EmitSound(CHAN_ITEM, "weapons/spore_ammo.wav", 1, ATTN_NORM);

		pev->framerate = 1.0;
		pev->animtime = UTIL_GlobalTimeBase() + 0.1;
		pev->sequence = SPOREAMMO_SNATCHDOWN;
		pev->body = 0;
		m_borntime = true;
		m_flTimeSporeIdle = UTIL_GlobalTimeBase() + 1;
		SetThink(&CSporeAmmoPlant::IdleThink);
	}
}