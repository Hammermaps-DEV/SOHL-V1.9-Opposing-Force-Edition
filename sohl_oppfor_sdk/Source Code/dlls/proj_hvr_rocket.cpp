#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "proj_grenade.h"
#include "proj_hvr_rocket.h"

LINK_ENTITY_TO_CLASS(hvr_rocket, CApacheHVR);

TYPEDESCRIPTION	CApacheHVR::m_SaveData[] = {
	DEFINE_FIELD(CApacheHVR, m_vecForward, FIELD_VECTOR),
};

IMPLEMENT_SAVERESTORE(CApacheHVR, CGrenade);

void CApacheHVR::Spawn(void)
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/HVR.mdl");
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	UTIL_SetOrigin(this, pev->origin);

	SetThink(&CApacheHVR::IgniteThink);
	SetTouch(&CApacheHVR::ExplodeTouch);

	UTIL_MakeAimVectors(pev->angles);
	m_vecForward = gpGlobals->v_forward;
	pev->gravity = 0.5;

	SetNextThink(0.1);

	pev->dmg = 150;
}

void CApacheHVR::Precache(void)
{
	PRECACHE_MODEL("models/HVR.mdl");
	m_iTrail = PRECACHE_MODEL("sprites/smoke.spr");
	PRECACHE_SOUND("weapons/rocket1.wav");
}

void CApacheHVR::ExplodeTouch(CBaseEntity *pOther)
{
	TraceResult tr;
	Vector vecSpot;// trace starts here!

	pev->enemy = pOther->edict();

	vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine(vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr);
	STOP_SOUND(ENT(pev), CHAN_STATIC, "weapons/rocket1.wav");

	Explode(&tr, DMG_BLAST);
}

void CApacheHVR::IgniteThink(void)
{
	pev->effects |= EF_LIGHT;

	// make rocket sound
	EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "weapons/rocket1.wav", VOL_NORM, 0.5, 0, 100);

	// rocket trail
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);

	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(entindex());	// entity
	WRITE_SHORT(m_iTrail);	// model
	WRITE_BYTE(15); // life
	WRITE_BYTE(5);  // width
	WRITE_BYTE(224);   // r, g, b
	WRITE_BYTE(224);   // r, g, b
	WRITE_BYTE(255);   // r, g, b
	WRITE_BYTE(255);	// brightness

	MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	// set to accelerate
	SetThink(&CApacheHVR::AccelerateThink);
	SetNextThink(0.1);
}

void CApacheHVR::AccelerateThink(void)
{
	// check world boundaries
	if (pev->origin.x < -4096 || pev->origin.x > 4096 || pev->origin.y < -4096 || 
		pev->origin.y > 4096 || pev->origin.z < -4096 || pev->origin.z > 4096 || !IsInWorld()) {
		STOP_SOUND(ENT(pev), CHAN_BODY, "weapons/rocket1.wav");
		UTIL_Remove(this);
		return;
	}

	// accelerate
	float flSpeed = pev->velocity.Length();
	if (flSpeed < 1800)
	{
		pev->velocity = pev->velocity + m_vecForward * 200;
	}

	// re-aim
	pev->angles = UTIL_VecToAngles(pev->velocity);
	SetNextThink(0.1);
}