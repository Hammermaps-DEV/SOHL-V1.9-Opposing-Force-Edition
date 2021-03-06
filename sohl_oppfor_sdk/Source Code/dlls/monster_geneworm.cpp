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
//=========================================================
// Generic Monster - purely for scripted sequence work.
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"player.h"
#include	"weapons.h"

#define GENEWORM_LEVEL0					0
#define GENEWORM_LEVEL1					1

#define GENEWORM_LEVEL0_HEIGHT			244
#define GENEWORM_LEVEL1_HEIGHT			304


#define GENEWORM_SKIN_EYE_OPEN			0
#define GENEWORM_SKIN_EYE_LEFT			1
#define GENEWORM_SKIN_EYE_RIGHT			2
#define GENEWORM_SKIN_EYE_CLOSED		3


//=========================================================
// Monster's Anim Events Go Here
//=========================================================

#define GENEWORM_AE_BEAM			( 0 )		// Toxic beam attack (sprite trail)
#define GENEWORM_AE_PORTAL			( 2 )		// Create a portal that spawns an enemy.

#define GENEWORM_AE_MELEE_LEFT1		( 3 )		// Play hit sound
#define GENEWORM_AE_MELEE_LEFT2		( 4 )		// Activates the trigger_geneworm_hit

#define GENEWORM_AE_MELEE_RIGHT1	( 5 )		// Play hit sound
#define GENEWORM_AE_MELEE_RIGHT2	( 6 )		// Activates the trigger_geneworm_hit

#define GENEWORM_AE_MELEE_FORWARD1  ( 7 )		// Play hit sound
#define GENEWORM_AE_MELEE_FORWARD2  ( 8 )		// Activates the trigger_geneworm_hit

#define GENEWORM_AE_MAD				( 9 )		// Room starts shaking!

#define GENEWORM_AE_EYEPAIN			( 1012 )	// Still put here (In case we need to toggle eye pain status)

//=========================================================
// CGeneWorm
//=========================================================
class CGeneWorm : public CBaseMonster
{
public:

	int		Save(CSave &save);
	int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn();
	void Precache();
	int  Classify() { return CLASS_ALIEN_MONSTER; };
	int  BloodColor() { return BLOOD_COLOR_YELLOW; }
	void Killed(entvars_t *pevAttacker, int iGib);

	void SetObjectCollisionBox()
	{
		pev->absmin = pev->origin + Vector(-95, -95, 0);
		pev->absmax = pev->origin + Vector(95, 95, 0);
	}

	void HandleAnimEvent(MonsterEvent_t *pEvent);

	void EXPORT StartupThink();
	void EXPORT HuntThink();
	void EXPORT CrashTouch(CBaseEntity *pOther);
	void EXPORT DyingThink();
	void EXPORT StartupUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT NullThink();
	void EXPORT CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void FloatSequence();
	void NextActivity();

	void Flight();

	int  TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);

	void PainSound();
	void DeathSound();
	void IdleSound();

	int		Level(float dz);
	int		MyEnemyLevel();
	float	MyEnemyHeight();

	Vector m_vecTarget;
	Vector m_posTarget;

	Vector m_vecDesired;
	Vector m_posDesired;

	float m_flLastSeen;
	float m_flPrevSeen;

	int m_iLevel;
	EHANDLE m_hHitVolumes[3];

	void PlaySequenceAttack(int side, BOOL bMelee);
	void PlaySequencePain(int side);

	//
	// SOUNDS
	//

	static const char *pAttackSounds[];
	static const char *pDeathSounds[];
	static const char *pEntrySounds[];
	static const char *pPainSounds[];
	static const char *pIdleSounds[];
	static const char *pEyePainSounds[];


	//
	// ANIMATIONS
	//

	static const char *pDeathAnims[];
	static const char *pEntryAnims[];
	static const char *pIdleAnims[];

	static const char *pMadAnims[];
	static const char *pScreamAnims[];

	static const char *pBigPainAnims[];
	static const char *pEyePainAnims[];
	static const char *pIdlePainAnims[];

	static const char *pMeleeAttackAnims[];
	static const char *pBeamAttackAnims[];

	//
	// HIT VOLUMES
	//
	static const char* pHitVolumes[];
};

LINK_ENTITY_TO_CLASS(monster_geneworm, CGeneWorm);

TYPEDESCRIPTION CGeneWorm::m_SaveData[] =
{
	DEFINE_FIELD(CGeneWorm, m_vecTarget, FIELD_VECTOR),
	DEFINE_FIELD(CGeneWorm, m_posTarget, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(CGeneWorm, m_vecDesired, FIELD_VECTOR),
	DEFINE_FIELD(CGeneWorm, m_posDesired, FIELD_POSITION_VECTOR),

	DEFINE_FIELD(CGeneWorm, m_flLastSeen, FIELD_TIME),
	DEFINE_FIELD(CGeneWorm, m_flPrevSeen, FIELD_TIME),

	DEFINE_FIELD(CGeneWorm, m_iLevel, FIELD_INTEGER),
	DEFINE_ARRAY(CGeneWorm, m_hHitVolumes, FIELD_EHANDLE, 3),
};
IMPLEMENT_SAVERESTORE(CGeneWorm, CBaseMonster);


//=========================================================
//=========================================================

const char *CGeneWorm::pAttackSounds[] =
{
	"geneworm/geneworm_attack_mounted_gun.wav",
	"geneworm/geneworm_attack_mounted_rocket.wav",
	"geneworm/geneworm_beam_attack.wav",
	"geneworm/geneworm_big_attack_forward.wav",
};

const char *CGeneWorm::pDeathSounds[] =
{
	"geneworm/geneworm_death.wav",
};

const char *CGeneWorm::pEntrySounds[] =
{
	"geneworm/geneworm_entry.wav",
};

const char *CGeneWorm::pPainSounds[] =
{
	"geneworm/geneworm_final_pain1.wav",
	"geneworm/geneworm_final_pain2.wav",
	"geneworm/geneworm_final_pain3.wav",
	"geneworm/geneworm_final_pain4.wav",
};

const char *CGeneWorm::pIdleSounds[] =
{
	"geneworm/geneworm_idle1.wav",
	"geneworm/geneworm_idle2.wav",
	"geneworm/geneworm_idle3.wav",
	"geneworm/geneworm_idle4.wav",
};

const char *CGeneWorm::pEyePainSounds[] =
{
	"geneworm/geneworm_shot_in_eye.wav",
};

const char* CGeneWorm::pHitVolumes[]
{
	"GeneWormRightSlash",
	"GeneWormCenterSlash",
	"GeneWormLeftSlash",
};


//=========================================================
//=========================================================

const char *CGeneWorm::pDeathAnims[] =
{
	"death",
};

const char *CGeneWorm::pEntryAnims[] =
{
	"entry",
};

const char *CGeneWorm::pIdleAnims[] =
{
	"idle",
};

const char *CGeneWorm::pMadAnims[] =
{
	"mad",
};

const char *CGeneWorm::pScreamAnims[] =
{
	"scream1",
	"scream2",
};

const char *CGeneWorm::pBigPainAnims[] =
{
	"bigpain1",
	"bigpain2",
	"bigpain3",
	"bigpain4",
};

const char *CGeneWorm::pEyePainAnims[] =
{
	"eyepain1",
	"eyepain2",
};

const char *CGeneWorm::pIdlePainAnims[] =
{

	"idlepain",
	"idlepain2",
	"idlepain3",
};

const char *CGeneWorm::pMeleeAttackAnims[] =
{
	"melee1",
	"melee2",
	"melee3",
};

const char *CGeneWorm::pBeamAttackAnims[] =
{
	"dattack1",
	"dattack2",
	"dattack3",
};

//=========================================================
// Spawn
//=========================================================
void CGeneWorm::Spawn()
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;


	SET_MODEL(ENT(pev), "models/geneworm.mdl");
	UTIL_SetSize(pev, Vector(-32, -32, 0), Vector(32, 32, 64));
	UTIL_SetOrigin(this, pev->origin);

	pev->flags |= FL_MONSTER;
	pev->takedamage = DAMAGE_AIM;
	pev->health = 999;
	pev->view_ofs = Vector(0, 0, 192);

	pev->sequence = 0;
	ResetSequenceInfo();

	InitBoneControllers();

	SetThink(&CGeneWorm::StartupThink);
	SetNextThink(0.1);

	m_vecDesired = Vector(1, 0, 0);
	m_posDesired = Vector(pev->origin.x, pev->origin.y, 512);
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CGeneWorm::Precache()
{
	PRECACHE_MODEL("models/geneworm.mdl");

	PRECACHE_SOUND_ARRAY(pAttackSounds);
	PRECACHE_SOUND_ARRAY(pDeathSounds);
	PRECACHE_SOUND_ARRAY(pEntrySounds);
	PRECACHE_SOUND_ARRAY(pPainSounds);
	PRECACHE_SOUND_ARRAY(pIdleSounds);
	PRECACHE_SOUND_ARRAY(pEyePainSounds);
}

//=========================================================
// NullThink
//=========================================================
void CGeneWorm::NullThink()
{
	StudioFrameAdvance();
	SetNextThink(0.5);
}

//=========================================================
// StartupUse
//=========================================================
void CGeneWorm::StartupUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	SetThink(&CGeneWorm::HuntThink);
	SetNextThink(0.1);
	SetUse(&CGeneWorm::CommandUse);
}

//=========================================================
//
//=========================================================
void CGeneWorm::StartupThink()
{
	CBaseEntity *pEntity = NULL;

	int i;
	for (i = 0; i < 3; i++)
	{
		pEntity = UTIL_FindEntityByTargetname(NULL, pHitVolumes[i]);
		if (pEntity)
			m_hHitVolumes[i] = pEntity;
	}

	SetThink(&CGeneWorm::HuntThink);
	SetUse(&CGeneWorm::CommandUse);
	SetNextThink(0.1);
}


void CGeneWorm::Killed(entvars_t *pevAttacker, int iGib)
{
	CBaseMonster::Killed(pevAttacker, iGib);
}

void CGeneWorm::DyingThink()
{
	SetNextThink(0.1);
	DispatchAnimEvents();
	StudioFrameAdvance();

	if (pev->deadflag == DEAD_NO)
	{
		FireTargets(STRING(pev->target), this, this, USE_ON, 1.0);
		pev->velocity = Vector(0, 0, 0);

		DeathSound();
		pev->deadflag = DEAD_DYING;
	}

	if (pev->deadflag == DEAD_DYING)
	{
		Flight();
		pev->deadflag = DEAD_DEAD;
	}

	if (m_fSequenceFinished)
	{
		pev->sequence = LookupSequence("death");
		pev->framerate = 0.5f;
	}

	if (m_hHitVolumes)
	{
		int i;
		for (i = 0; i < 3; i++)
		{
			UTIL_Remove(m_hHitVolumes[i]);
			m_hHitVolumes[i] = NULL;
		}
	}

	return;
}

//=========================================================
//=========================================================
void CGeneWorm::FloatSequence()
{
	pev->sequence = LookupSequence("idle");
}

//=========================================================
//=========================================================
void CGeneWorm::NextActivity()
{
	UTIL_MakeAimVectors(pev->angles);

	float flDist = (m_posDesired - pev->origin).Length();
	float flDot = DotProduct(m_vecDesired, gpGlobals->v_forward);

	if (m_hEnemy != NULL && !m_hEnemy->IsAlive())
	{
		m_hEnemy = NULL;
	}

	if (m_flLastSeen + 15 < UTIL_GlobalTimeBase())
	{
		m_hEnemy = NULL;
	}

	if (m_hEnemy == NULL)
	{
		Look(4096);
		m_hEnemy = BestVisibleEnemy();

		if (!m_hEnemy)
		{
			m_hEnemy = UTIL_PlayerByIndex(1);
			ASSERT(m_hEnemy);
		}
	}

	if (m_hEnemy != NULL)
	{
		if (m_flLastSeen + 5 > UTIL_GlobalTimeBase())
		{
			return;
		}
	}


	if (m_iLevel != -1)
	{
		ASSERT(m_hEnemy);
	}

	FloatSequence();
}

void CGeneWorm::HuntThink()
{
	SetNextThink(0.1);
	DispatchAnimEvents();
	StudioFrameAdvance();

	// if dead, force cancelation of current animation
	if (pev->health <= 0)
	{
		SetThink(&CGeneWorm::DyingThink);
		m_fSequenceFinished = TRUE;
		return;
	}

	// get new sequence
	if (m_fSequenceFinished)
	{
		pev->frame = 0;
		NextActivity();
		ResetSequenceInfo();
	}

	// look for current enemy	
	if (m_hEnemy != NULL)
	{
		// Update level.
		m_iLevel = Level(m_hEnemy->pev->origin.z);

		if (FVisible(m_hEnemy))
		{
			if (m_flLastSeen < UTIL_GlobalTimeBase() - 5)
				m_flPrevSeen = UTIL_GlobalTimeBase();
			m_flLastSeen = UTIL_GlobalTimeBase();
			m_posTarget = m_hEnemy->pev->origin;
			m_vecTarget = (m_posTarget - pev->origin).Normalize();
			m_vecDesired = m_vecTarget;
			m_posDesired = Vector(pev->origin.x, pev->origin.y, pev->origin.z);
		}
		else
		{
			//m_flAdj = min(m_flAdj + 10, 1000);
		}
	}
	else
	{
		m_iLevel = -1;
	}

#if 0
	// don't go too high
	if (m_posDesired.z > m_flMaxZ)
		m_posDesired.z = m_flMaxZ;

	// don't go too low
	if (m_posDesired.z < m_flMinZ)
		m_posDesired.z = m_flMinZ;
#endif

	Flight();
}



//=========================================================
//=========================================================
void CGeneWorm::Flight()
{

}

void CGeneWorm::HandleAnimEvent(MonsterEvent_t *pEvent)
{
	switch (pEvent->event)
	{
	case 1:	// shoot 
		break;
	default:
		break;
	}
}

void CGeneWorm::CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	switch (useType)
	{
	case USE_OFF:
		break;
	case USE_ON:
		break;
	case USE_SET:
		break;
	case USE_TOGGLE:
		break;
	}
}

int CGeneWorm::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	if (pevInflictor->owner == edict())
		return 0;

	if (flDamage >= pev->health)
	{
		pev->health = 1;
	}

	PainSound();

	pev->health -= flDamage;
	return 0;
}

void CGeneWorm::PainSound()
{

}

void CGeneWorm::DeathSound()
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pDeathSounds), VOL_NORM, ATTN_NORM);
}

void CGeneWorm::IdleSound()
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, RANDOM_SOUND_ARRAY(pIdleSounds), VOL_NORM, ATTN_NORM);
}

int CGeneWorm::Level(float dz)
{
	if (dz < GENEWORM_LEVEL1_HEIGHT)
		return GENEWORM_LEVEL0;

	return GENEWORM_LEVEL1;
}
int CGeneWorm::MyEnemyLevel()
{
	if (!m_hEnemy)
		return -1;

	return Level(m_hEnemy->pev->origin.z);
}

float CGeneWorm::MyEnemyHeight()
{
	switch (m_iLevel)
	{
	case GENEWORM_LEVEL0:
		return GENEWORM_LEVEL0_HEIGHT;
	case GENEWORM_LEVEL1:
		return GENEWORM_LEVEL1_HEIGHT;
	}

	return GENEWORM_LEVEL1_HEIGHT;
}