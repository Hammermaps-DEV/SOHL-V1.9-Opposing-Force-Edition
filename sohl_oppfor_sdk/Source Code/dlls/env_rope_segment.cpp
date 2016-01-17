/***
*
*	Copyright (c) 2003, TCIS-Team. All rights reserved.
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
//========================================
//Rope
//========================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"player.h"
#include	"weapons.h"

class CRope : public CBaseMonster {
public:
	//Define the class and all the functions we need.
	void Spawn(void);
	void Precache(void);
	int  Classify(void);
	void KeyValue(KeyValueData *pkvd);
	void RopePendulum(int);	//Pendulum on rope. 1/-1 aka forward/back 
	void EXPORT RopeTouch(CBaseEntity *pOther); //so we know when we hit something
	void EXPORT RopeThink(void);
	void EXPORT SegmentThink(void);

	virtual CRope *RopeCreate(int loop);
	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);

	static	TYPEDESCRIPTION m_SaveData[];

	CBaseEntity *RopeTouchEnt(float *pflLength);
	CBaseEntity *pList[10];

	Vector m_vPlayerHangOrigin, m_start, m_end;

	int j;
	int m_iSegments;
	int m_zforce;
	int m_yforce;
	int m_xforce;
	int m_loop = 0;

	float m_ilightningfrequency;
	float m_ibodysparkfrequency;
	float m_isparkfrequency;

	bool m_disable;
	string_t m_BodyModel;
	string_t m_EndingModel;
	

	BOOL m_iCount;
	BOOL m_fRopeActive;
	BOOL m_fRopeExtended;

	CRope *pSegment[100];	//100 segments in limit (8-800ich; 16-1600ich; 24-2400ich;32-3200ich).
	CRope *pNewSegment;
};
//LINK_ENTITY_TO_CLASS(env_electrified_wire, CRope);
//LINK_ENTITY_TO_CLASS(env_rope, CRope);

TYPEDESCRIPTION	CRope::m_SaveData[] =
{
	DEFINE_FIELD(CRope, m_iSegments, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CRope, CBaseMonster);

void CRope::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "segments"))
	{
		m_iSegments = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "bodymodel"))
	{
		m_BodyModel = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "endingmodel"))
	{
		m_EndingModel = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "disable"))
	{
		m_disable = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "zforce"))
	{
		m_zforce = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "yforce"))
	{
		m_yforce = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "xforce"))
	{
		m_xforce = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "lightningfrequency"))
	{
		m_ilightningfrequency = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "bodysparkfrequency"))
	{
		m_ibodysparkfrequency = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "sparkfrequency"))
	{
		m_isparkfrequency = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseMonster::KeyValue(pkvd);
	}
}

CRope *CRope::RopeCreate(int m_loop)
{
	// Lets make a new Rope
	CRope *pRope = GetClassPtr((CRope *)NULL);
	pRope->pev->classname = MAKE_STRING("segments");
	pRope->m_loop = m_loop;
	pRope->Spawn();
	return pRope;
}

void CRope::Spawn()
{
	PRECACHE_MODEL("models/wire_blue8.mdl");

	PRECACHE_SOUND("items/grab_rope.wav");
	PRECACHE_SOUND("items/rope1.wav");
	PRECACHE_SOUND("items/rope2.wav");
	PRECACHE_SOUND("items/rope3.wav");

	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_PUSH;
	//	pev->movetype = MOVETYPE_FOLLOW;								//BUGBUG! MUST BE IT!!!
	//	pev->aiment = g_engfuncs.pfnPEntityOfEntIndex( m_iSegments );	//BUGBUG! MUST BE SOME HERE...

	//lower grav to make up for the engine not being able to
	// calculate wind resistance.
	pev->gravity = 0.5;

	ALERT(at_console, "Set Segments %d\n", m_loop);
	ALERT(at_console, "Set Model: %s\n", m_BodyModel);

	SET_MODEL(ENT(pev),"models/rope16.mdl");

	//if(!m_loop && !m_iSegments)
		//SET_MODEL(ENT(pev),STRING(m_EndingModel));
	//else
	//	SET_MODEL(ENT(pev),STRING(m_BodyModel));

	Vector vecDir = pev->velocity.Normalize();
	UTIL_SetOrigin(this, pev->origin - vecDir * 12);
	pev->angles = UTIL_VecToAngles(vecDir);
	pev->velocity = Vector(0, 0, 0);
	pev->avelocity.z = 0;
	pev->angles.x = 180;
	pev->angles.y = 180;
	pev->angles.z = 90;

	UTIL_SetSize(pev, Vector(-1, -1, -1), Vector(1, 1, 1));

	pev->nextthink = gpGlobals->time + 0.001;
	SetThink(&CRope::RopeThink);
	SetTouch(&CRope::RopeTouch);
}

// Precache is used only to continue after a game has loaded.
void CRope::Precache()
{
	PRECACHE_MODEL("models/rope16.mdl");
	PRECACHE_MODEL( (char *)STRING(m_BodyModel) );
	PRECACHE_MODEL( (char *)STRING(m_EndingModel) );
	SetThink(&CRope::RopeThink);
	Think();
}

int CRope::Classify(void)
{
	return  CLASS_NONE;
}

CBaseEntity *CRope::RopeTouchEnt(float *pflLength)
{
	TraceResult	tr;
	float	length;

	// trace once to hit architecture and see if the tongue needs to change position.
	UTIL_TraceLine(pev->origin, pev->origin - Vector(0, 0, 2048), ignore_monsters, ENT(pev), &tr);
	length = fabs(pev->origin.z - tr.vecEndPos.z);
	if (pflLength)
	{
		*pflLength = length;
	}

	Vector delta = Vector(5, 5, 0);
	Vector mins = pev->origin - delta;
	Vector maxs = pev->origin + delta;
	maxs.z = pev->origin.z;
	mins.z -= length;

	int count = UTIL_EntitiesInBox(pList, 10, mins, maxs, (FL_CLIENT | FL_MONSTER));
	if (count)
	{
		for (int i = 0; i < count; i++)
		{
			// only clients and monsters
			if (pList[i] != this && IRelationship(pList[i]) > R_NO && pList[i]->pev->deadflag == DEAD_NO)
			{
				return pList[i];
			}
		}
	}

	return NULL;
}

void CRope::RopeThink(void)
{
	if (!m_fRopeActive)
	{
		CBaseEntity *pTouchEnt;
		float flLength;

		pTouchEnt = RopeTouchEnt(&flLength);
		for (int i = 1; i < m_iSegments; i++) {
			Vector vecSrc;
			Vector vecDir = pev->velocity.Normalize();
			pSegment[j] = CRope::RopeCreate(i);
			if (!m_iCount)
			{
				GetAttachment(0, vecSrc, vecDir);
			} else {
				j = j - 1;
				pSegment[j]->GetAttachment(0, vecSrc, vecDir);
				j = j + 1;
			}

			//set it's angle and velocity and stuff
			pSegment[j]->pev->origin = vecSrc;
			pSegment[j]->pev->angles = UTIL_VecToAngles(vecDir);
			pSegment[j]->pev->angles.x = 180;
			pSegment[j]->pev->angles.y = 180;
			pSegment[j]->pev->angles.z = 90;
			pSegment[j]->pev->velocity = Vector(0, 0, 0);
			pSegment[j]->pev->speed = 0;
			pSegment[j]->pev->owner = edict();

			UTIL_SetSize(pSegment[j]->pev, Vector(-1, -1, -1), Vector(1, 1, 1));

			m_iCount = TRUE;
			j++;
		}

		m_fRopeActive = TRUE;
	}

	if (m_fRopeExtended)
	{
		ALERT(at_console, "Segment reached VOID, removed\n");

		SetThink(&CRope::SUB_Remove);

		m_fRopeExtended = FALSE;
	}

	SetNextThink(1.0);
}

void CRope::RopePendulum(int)
{
	return;
}

void CRope::SegmentThink(void)
{
	ALERT(at_console, "SEGMENT THINKS\n");
	SetThink(NULL);

	switch (RANDOM_LONG(0, 2))
	{
		case 0:	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "items/rope1.wav", 1, ATTN_NORM);	break;
		case 1:	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "items/rope2.wav", 1, ATTN_NORM);	break;
		case 2:	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "items/rope3.wav", 1, ATTN_NORM);	break;
	}

	edict_t* pEntFind;
	pEntFind = FIND_ENTITY_BY_CLASSNAME(NULL, "segments");

	CBaseEntity *pEnt = Instance(pEntFind);

	pEnt->pev->angles.x = RANDOM_LONG(178, 180);
	pEnt->pev->angles.y = RANDOM_LONG(178, 180);
	pEnt->pev->angles.z = RANDOM_LONG(88, 91);

	// set destdelta to the vector needed to move
	Vector vecDestDelta = m_end - pEnt->pev->angles;

	pEnt->pev->avelocity = vecDestDelta;
	pEnt->pev->angles = UTIL_VecToAngles(pev->velocity);
	pEnt->pev->nextthink = pEnt->pev->ltime + 1;

}












void CRope::RopeTouch(CBaseEntity *pOther)
{
	SetThink(NULL);

	if (pOther->IsPlayer())
	{

		//	ALERT(at_console, "TOUCH\n");

		CBasePlayer *pPlayer = (CBasePlayer *)pOther;

		Vector vecSrc = pPlayer->pev->origin;
		Vector vecAiming = gpGlobals->v_forward;
		Vector vecTarget = gpGlobals->v_forward;
		Vector vecDir;

		if ((pPlayer->pev->button & IN_JUMP))
		{
			//Traceline does to... We use it to find where to jump at.	
			TraceResult tr;
			UTIL_TraceLine(vecSrc, vecSrc + vecDir, ignore_monsters, ENT(pev), &tr);

			float flSpeed = pPlayer->pev->velocity.Length();

			vecDir = tr.vecEndPos - pPlayer->pev->origin;
			vecTarget = vecDir;
			pPlayer->pev->velocity = (pPlayer->pev->velocity * 1 + vecTarget * (flSpeed * 0.2 + 400)).Normalize() * 350;
			pPlayer->pev->movetype = MOVETYPE_WALK;
			pPlayer->pev->gravity = 1;

			return;
		}
		else 	if ((pPlayer->pev->button & IN_FORWARD))
		{
			//Traceline does to... We use it to find where to jump at.	
			TraceResult tr;
			UTIL_TraceLine(pPlayer->pev->origin, pPlayer->pev->origin + vecAiming, ignore_monsters, ENT(pev), &tr);

			float flSpeed = pPlayer->pev->velocity.Length();

			vecDir = tr.vecEndPos - pPlayer->pev->origin;
			vecTarget = vecDir;
			pPlayer->pev->velocity = (pPlayer->pev->velocity * 0.2 + vecTarget * (flSpeed * 0.2 + 400)).Normalize() * 350;
			pPlayer->pev->velocity.x = 0;
			pPlayer->pev->velocity.y = 0;
			pPlayer->pev->movetype = MOVETYPE_WALK;
			pPlayer->pev->gravity = 0;

			return;
		}
		else	if ((pPlayer->pev->button & IN_BACK))
		{
			//Traceline does to... We use it to find where to jump at.	
			TraceResult tr;
			UTIL_TraceLine(pPlayer->pev->origin, pPlayer->pev->origin + vecAiming, ignore_monsters, ENT(pev), &tr);

			float flSpeed = pPlayer->pev->velocity.Length();

			vecDir = tr.vecEndPos - pPlayer->pev->origin;
			vecTarget = vecDir;
			pPlayer->pev->velocity = (pPlayer->pev->velocity * 0.2 + vecTarget * (flSpeed * 0.2 + 400)).Normalize() * -350;
			pPlayer->pev->velocity.x = 0;
			pPlayer->pev->velocity.y = 0;
			pPlayer->pev->movetype = MOVETYPE_WALK;
			pPlayer->pev->gravity = 0;

			return;
		}


		Vector m_vPlayerHangOrigin;
		m_vPlayerHangOrigin = pPlayer->pev->origin;

		if (!pPlayer->pev->button) // Passive or undone keys, must be hang.
		{
			//	   ALERT(at_console, "HANG\n");

			//	   m_vPlayerHangOrigin = pPlayer->pev->origin;

			//Stop the player from moving so he can hang.
			pPlayer->pev->origin = m_vPlayerHangOrigin;
			pPlayer->pev->velocity = Vector(0, 0, 0);
			pPlayer->pev->gravity = 0;
			pPlayer->pev->speed = 0;
			int flSpeed = 0;

			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "items/grab_rope.wav", 1, ATTN_NORM);

		}
	}
	//	SetNextThink(0.5);

	pev->nextthink = gpGlobals->time + 0.1;
	SetThink(&CRope::SegmentThink);
}