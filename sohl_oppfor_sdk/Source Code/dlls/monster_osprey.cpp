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
// NPC: V-22 Osprey * http://half-life.wikia.com/wiki/V-22_Osprey
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "effects.h"
#include "particle_defs.h"
#include "customentity.h"
#include "monster_osprey.h"

//=========================================================
// Monster's link to Class & Saverestore Begins
//=========================================================
LINK_ENTITY_TO_CLASS(monster_osprey, COsprey);

TYPEDESCRIPTION	COsprey::m_SaveData[] = {
	DEFINE_FIELD(COsprey, m_pGoalEnt, FIELD_CLASSPTR),
	DEFINE_FIELD(COsprey, m_vel1, FIELD_VECTOR),
	DEFINE_FIELD(COsprey, m_vel2, FIELD_VECTOR),
	DEFINE_FIELD(COsprey, m_pos1, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(COsprey, m_pos2, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(COsprey, m_ang1, FIELD_VECTOR),
	DEFINE_FIELD(COsprey, m_ang2, FIELD_VECTOR),

	DEFINE_FIELD(COsprey, m_startTime, FIELD_TIME),
	DEFINE_FIELD(COsprey, m_dTime, FIELD_FLOAT),
	DEFINE_FIELD(COsprey, m_velocity, FIELD_VECTOR),

	DEFINE_FIELD(COsprey, m_flIdealtilt, FIELD_FLOAT),
	DEFINE_FIELD(COsprey, m_flRotortilt, FIELD_FLOAT),

	DEFINE_FIELD(COsprey, m_flRightHealth, FIELD_FLOAT),
	DEFINE_FIELD(COsprey, m_flLeftHealth, FIELD_FLOAT),

	DEFINE_FIELD(COsprey, m_iUnits, FIELD_INTEGER),
	DEFINE_FIELD(COsprey, m_monster_spawn, FIELD_INTEGER),
	DEFINE_ARRAY(COsprey, m_hGrunt, FIELD_EHANDLE, MAX_CARRY),
	DEFINE_ARRAY(COsprey, m_vecOrigin, FIELD_POSITION_VECTOR, MAX_CARRY),
	DEFINE_ARRAY(COsprey, m_hRepel, FIELD_EHANDLE, 4),

	DEFINE_FIELD(COsprey, m_iDoLeftSmokePuff, FIELD_INTEGER),
	DEFINE_FIELD(COsprey, m_iDoRightSmokePuff, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(COsprey, CBaseMonster);

//=========================================================
// Monster Sounds
//=========================================================
const char *COsprey::pDebrisSounds[] = {
	"weapons/debris1.wav",
	"weapons/debris2.wav",
	"weapons/debris3.wav"
};

const char *COsprey::pExplodeSounds[] = {
	"explosions/explode1.wav",
	"explosions/explode2.wav",
	"explosions/explode3.wav",
	"explosions/explode4.wav",
	"explosions/explode5.wav"
};

//=========================================================
// KeyValue
//=========================================================
void COsprey::KeyValue(KeyValueData *pkvd) {
	if (FStrEq(pkvd->szKeyName, "m_iMonster")) {
		m_iMonster = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else {
		CBaseMonster::KeyValue(pkvd);
	}
}

//=========================================================
// Spawn Osprey
//=========================================================
void COsprey::Spawn() {
	if (!m_iMonster) {
		m_iMonster = 0;
	}

	switch (m_iMonster) {
	case 1: m_monster_spawn = "monster_human_grunt_ally"; break; //Human Grunt (Ally)
	case 2:	m_monster_spawn = "monster_human_medic_ally"; break; //Human Grunt (Ally Medic)
	case 3: m_monster_spawn = "monster_human_torch_ally"; break; //Human Grunt (Ally Torch)
	case 5: m_monster_spawn = "monster_male_assassin"; break; //Black Ops
	default: m_monster_spawn = "monster_human_grunt"; break; //Human Grunt
	}

	Precache();

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/osprey.mdl");

	UTIL_SetSize(pev, Vector(-400, -400, -100), Vector(400, 400, 32));
	UTIL_SetOrigin(this, pev->origin);

	// Motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	pev->flags |= FL_MONSTER | FL_FLY;
	pev->takedamage = DAMAGE_YES;
	pev->speed = 80; //LRC - default speed, in case path corners don't give a speed.

	if (pev->health == 0)
		pev->health = gSkillData.ospreyHealth;

	m_flFieldOfView = 0; // 180 degrees
	m_flRightHealth = (pev->health / 2);
	m_flLeftHealth = (pev->health / 2);

	pev->sequence = 0;
	ResetSequenceInfo();
	pev->frame = RANDOM_LONG(0, 0xFF);

	InitBoneControllers();

	SetThink(&COsprey::FindAllThink);
	SetUse(&COsprey::CommandUse);

	if (!(pev->spawnflags & SF_MONSTER_SPAWNFLAG_32)) {
		SetNextThink(1.0);
	}

	m_pos2 = pev->origin;
	m_ang2 = pev->angles;
	m_vel2 = pev->velocity;
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	COsprey::Classify() {
	return m_iClass ? m_iClass : CLASS_MACHINE;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void COsprey::Precache() {
	if (m_iMonster == 4) {
		UTIL_PrecacheOther("monster_human_grunt_ally");
		UTIL_PrecacheOther("monster_human_medic_ally");
		UTIL_PrecacheOther("monster_human_torch_ally");
	}
	else {
		UTIL_PrecacheOther(m_monster_spawn);
	}

	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model)); //LRC
	else
		PRECACHE_MODEL("models/osprey.mdl");

	PRECACHE_MODEL("models/HVR.mdl");

	m_iSpriteTexture = PRECACHE_MODEL("sprites/rope.spr");

	m_iExplode = PRECACHE_MODEL("sprites/fexplo.spr");
	m_iTailGibs = PRECACHE_MODEL("models/osprey_tailgibs.mdl");
	m_iBodyGibs = PRECACHE_MODEL("models/osprey_bodygibs.mdl");
	m_iEngineGibs = PRECACHE_MODEL("models/osprey_enginegibs.mdl");

	PRECACHE_SOUND_ARRAY(pDebrisSounds);
	PRECACHE_SOUND_ARRAY(pExplodeSounds);

	PRECACHE_SOUND("apache/ap_whine1.wav");
	PRECACHE_SOUND("apache/ap_rotor4.wav");
	PRECACHE_SOUND("weapons/mortarhit.wav");
}

//=========================================================
// CommandUse
//=========================================================
void COsprey::CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) {
	SetNextThink(0.1);
}

//=========================================================
// FindAllThink
//=========================================================
void COsprey::FindAllThink() {
	CBaseEntity *pEntity = NULL;

	m_iUnits = 0;
	if (m_iMonster == 4) {
		while (m_iUnits < MAX_CARRY &&
			((pEntity = UTIL_FindEntityByClassname(pEntity, "monster_human_grunt_ally")) != NULL) ||
			((pEntity = UTIL_FindEntityByClassname(pEntity, "monster_human_medic_ally")) != NULL) ||
			((pEntity = UTIL_FindEntityByClassname(pEntity, "monster_human_torch_ally")) != NULL)) {
			if (pEntity->IsAlive()) {
				m_hGrunt[m_iUnits] = pEntity;
				m_vecOrigin[m_iUnits] = pEntity->pev->origin;
				m_iUnits++;
			}
		}
	}
	else {
		while (m_iUnits < MAX_CARRY && (pEntity = UTIL_FindEntityByClassname(pEntity, m_monster_spawn)) != NULL) {
			if (pEntity->IsAlive()) {
				m_hGrunt[m_iUnits] = pEntity;
				m_vecOrigin[m_iUnits] = pEntity->pev->origin;
				m_iUnits++;
			}
		}
	}

	if (m_iUnits == 0) {
		m_iUnits = 4; //LRC - stop whining, just make the damn grunts...
	}

	SetThink(&COsprey::FlyThink);
	SetNextThink(0.1);
	m_startTime = UTIL_GlobalTimeBase();
}

//=========================================================
// DeployThink
//=========================================================
void COsprey::DeployThink() {
	UTIL_MakeAimVectors(pev->angles);

	Vector vecForward = gpGlobals->v_forward;
	Vector vecRight = gpGlobals->v_right;
	Vector vecUp = gpGlobals->v_up;

	Vector vecSrc;

	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + Vector(0, 0, -4096.0), ignore_monsters, ENT(pev), &tr);
	CSoundEnt::InsertSound(bits_SOUND_DANGER, tr.vecEndPos, 400, 0.3);

	vecSrc = pev->origin + vecForward * 32 + vecRight * 100 + vecUp * -96;
	m_hRepel[0] = MakeGrunt(vecSrc);

	vecSrc = pev->origin + vecForward * -64 + vecRight * 100 + vecUp * -96;
	m_hRepel[1] = MakeGrunt(vecSrc);

	vecSrc = pev->origin + vecForward * 32 + vecRight * -100 + vecUp * -96;
	m_hRepel[2] = MakeGrunt(vecSrc);

	vecSrc = pev->origin + vecForward * -64 + vecRight * -100 + vecUp * -96;
	m_hRepel[3] = MakeGrunt(vecSrc);

	SetThink(&COsprey::HoverThink);
	SetNextThink(0.1);
}

//=========================================================
// HasDead
//=========================================================
BOOL COsprey::HasDead() {
	for (int i = 0; i < m_iUnits; i++) {
		if (m_hGrunt[i] == NULL || !m_hGrunt[i]->IsAlive()) {
			return TRUE;
		}
		else {
			m_vecOrigin[i] = m_hGrunt[i]->pev->origin;  // send them to where they died
		}
	}

	return FALSE;
}

//=========================================================
// MakeGrunt
//=========================================================
CBaseMonster *COsprey::MakeGrunt(Vector vecSrc) {
	CBaseEntity *pEntity;
	CBaseMonster *pGrunt;

	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecSrc + Vector(0, 0, -4096.0), dont_ignore_monsters, ENT(pev), &tr);
	if (tr.pHit && Instance(tr.pHit)->pev->solid != SOLID_BSP)
		return NULL;

	for (int i = 0; i < m_iUnits; i++) {
		if (m_hGrunt[i] == NULL || !m_hGrunt[i]->IsAlive()) {
			if (m_hGrunt[i] != NULL && m_hGrunt[i]->pev->rendermode == kRenderNormal) {
				m_hGrunt[i]->SUB_StartFadeOut();
			}

			if (m_iMonster == 4) {
				switch (RANDOM_LONG(0, 2)) {
				case 1: m_monster_spawn = "monster_human_torch_ally"; break;
				case 2: m_monster_spawn = "monster_human_medic_ally"; break;
				default: m_monster_spawn = "monster_human_grunt_ally"; break;
				}
			}

			pEntity = Create(m_monster_spawn, vecSrc, pev->angles);
			pGrunt = pEntity->MyMonsterPointer();
			pGrunt->pev->movetype = MOVETYPE_FLY;
			pGrunt->pev->velocity = Vector(0, 0, RANDOM_FLOAT(-196, -128));
			pGrunt->SetActivity(ACT_GLIDE);

			CBeam *pBeam = CBeam::BeamCreate("sprites/rope.spr", 10);
			pBeam->PointEntInit(vecSrc + Vector(0, 0, 112), pGrunt->entindex());
			pBeam->SetFlags(BEAM_FSOLID);
			pBeam->SetColor(255, 255, 255);
			pBeam->SetThink(&CBeam::SUB_Remove);
			pBeam->SetNextThink(-4096.0 * tr.flFraction / pGrunt->pev->velocity.z + 0.5);

			// ALERT( at_console, "%d at %.0f %.0f %.0f\n", i, m_vecOrigin[i].x, m_vecOrigin[i].y, m_vecOrigin[i].z );  
			pGrunt->m_vecLastPosition = m_vecOrigin[i];
			m_hGrunt[i] = pGrunt;
			return pGrunt;
		}
	}

	// ALERT( at_console, "none dead\n");
	return NULL;
}

//=========================================================
// HoverThink
//=========================================================
void COsprey::HoverThink() {
	int i;
	for (i = 0; i < 4; i++) {
		if (m_hRepel[i] != NULL && m_hRepel[i]->pev->health > 0 && !(m_hRepel[i]->pev->flags & FL_ONGROUND)) {
			break;
		}
	}

	if (i == 4) {
		m_startTime = UTIL_GlobalTimeBase();
		SetThink(&COsprey::FlyThink);
	}

	SetNextThink(0.1);
	UTIL_MakeAimVectors(pev->angles);
	ShowDamage();
}

//=========================================================
// UpdateGoal
//=========================================================
void COsprey::UpdateGoal() {
	if (m_pGoalEnt) {
		m_pos1 = m_pos2;
		m_ang1 = m_ang2;
		m_vel1 = m_vel2;
		m_pos2 = m_pGoalEnt->pev->origin;
		m_ang2 = m_pGoalEnt->pev->angles;
		UTIL_MakeAimVectors(Vector(0, m_ang2.y, 0));

		//LRC - ugh. we shouldn't require our path corners to specify a speed!
		if (m_pGoalEnt->pev->speed)
			pev->speed = m_pGoalEnt->pev->speed;

		m_vel2 = gpGlobals->v_forward * pev->speed; //LRC

		m_startTime = m_startTime + m_dTime;
		m_dTime = 2.0 * (m_pos1 - m_pos2).Length() / (m_vel1.Length() + pev->speed);

		//ALERT(at_console, "osprey m_dTime = %f / %f + %f\n", (m_pos1 - m_pos2).Length(), m_vel1.Length(), m_pGoalEnt->pev->speed);

		if (m_ang1.y - m_ang2.y < -180)
		{
			m_ang1.y += 360;
		}
		else if (m_ang1.y - m_ang2.y > 180) {
			m_ang1.y -= 360;
		}

		if (pev->speed < 400)
			m_flIdealtilt = 0;
		else
			m_flIdealtilt = -90;
	}
	else
	{
		ALERT(at_debug, "osprey missing target");
	}
}

//=========================================================
// FlyThink
//=========================================================
void COsprey::FlyThink()
{
	StudioFrameAdvance();
	SetNextThink(0.1);

	if (m_pGoalEnt == NULL && !FStringNull(pev->target))// this monster has a target
	{
		m_pGoalEnt = UTIL_FindEntityByTargetname(NULL, STRING(pev->target));
		UpdateGoal();
	}

	if (UTIL_GlobalTimeBase() > m_startTime + m_dTime)
	{
		if (m_pGoalEnt->pev->speed == 0)
		{
			SetThink(&COsprey::DeployThink);
		}
		int loopbreaker = 100; //LRC - <slap> don't loop indefinitely!
		do {
			m_pGoalEnt = UTIL_FindEntityByTargetname(NULL, STRING(m_pGoalEnt->pev->target));
			loopbreaker--; //LRC
		} while (m_pGoalEnt->pev->speed < 400 && !HasDead() && loopbreaker > 0);
		UpdateGoal();
	}

	Flight();
	ShowDamage();
}

//=========================================================
// Flight
//=========================================================
void COsprey::Flight()
{
	float t = (UTIL_GlobalTimeBase() - m_startTime);
	float scale = 1.0 / m_dTime;

	float f = UTIL_SplineFraction(t * scale, 1.0);

	//	ALERT(at_console, "Osprey setorigin m_pos1 %f, m_vel1 %f, m_pos2 %f, m_vel2 %f, m_dTime %f, t %f, f %f\n", m_pos1.x, m_vel1.x, m_pos2.x, m_vel2.x, m_dTime, t, f);

	Vector pos = (m_pos1 + m_vel1 * t) * (1.0 - f) + (m_pos2 - m_vel2 * (m_dTime - t)) * f;
	Vector ang = (m_ang1) * (1.0 - f) + (m_ang2)* f;
	m_velocity = m_vel1 * (1.0 - f) + m_vel2 * f;

	UTIL_SetOrigin(this, pos);
	pev->angles = ang;
	UTIL_MakeAimVectors(pev->angles);
	float flSpeed = DotProduct(gpGlobals->v_forward, m_velocity);

	// float flSpeed = DotProduct( gpGlobals->v_forward, pev->velocity );

	float m_flIdealtilt = (160 - flSpeed) / 10.0;

	// ALERT( at_console, "%f %f\n", flSpeed, flIdealtilt );
	if (m_flRotortilt < m_flIdealtilt)
	{
		m_flRotortilt += 0.5;
		if (m_flRotortilt > 0)
			m_flRotortilt = 0;
	}
	if (m_flRotortilt > m_flIdealtilt)
	{
		m_flRotortilt -= 0.5;
		if (m_flRotortilt < -90)
			m_flRotortilt = -90;
	}
	SetBoneController(0, m_flRotortilt);


	if (m_iSoundState == 0)
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "apache/ap_rotor4.wav", 1.0, 0.15, 0, 110);
		// EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "apache/ap_whine1.wav", 0.5, 0.2, 0, 110 );

		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions
	}
	else
	{
		CBaseEntity *pPlayer = NULL;

		pPlayer = UTIL_FindEntityByClassname(NULL, "player");
		// UNDONE: this needs to send different sounds to every player for multiplayer.	
		if (pPlayer)
		{
			float pitch = DotProduct(m_velocity - pPlayer->pev->velocity, (pPlayer->pev->origin - pev->origin).Normalize());

			pitch = (int)(100 + pitch / 75.0);

			if (pitch > 250)
				pitch = 250;
			if (pitch < 50)
				pitch = 50;

			if (pitch == 100)
				pitch = 101;

			if (pitch != m_iPitch)
			{
				m_iPitch = pitch;
				EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "apache/ap_rotor4.wav", VOL_NORM, 0.15, SND_CHANGE_PITCH | SND_CHANGE_VOL, pitch);
				// ALERT( at_console, "%.0f\n", pitch );
			}
		}
		// EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "apache/ap_whine1.wav", flVol, 0.2, SND_CHANGE_PITCH | SND_CHANGE_VOL, pitch);
	}

}

//=========================================================
// HitTouch
//=========================================================
void COsprey::HitTouch(CBaseEntity *pOther)
{
	SetNextThink(2.0);
}

//=========================================================
// TakeDamage
//=========================================================
int COsprey::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) {
	if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_64) {
		CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
		if (pEnt->IsPlayer()) {
			pev->health = pev->max_health / 2;
			if (flDamage > 0) //Override all damage
				SetConditions(bits_COND_LIGHT_DAMAGE);

			if (flDamage >= 20) //Override all damage
				SetConditions(bits_COND_HEAVY_DAMAGE);

			return CBaseEntity::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
		}

		if (pevAttacker->owner) {
			pEnt = CBaseEntity::Instance(pevAttacker->owner);
			if (pEnt->IsPlayer()) {
				pev->health = pev->max_health / 2;
				if (flDamage > 0) //Override all damage
					SetConditions(bits_COND_LIGHT_DAMAGE);

				if (flDamage >= 20) //Override all damage
					SetConditions(bits_COND_HEAVY_DAMAGE);

				return CBaseEntity::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
			}
		}
	}

	if (!IsAlive() || pev->deadflag == DEAD_DYING || m_iPlayerReact) {
		return CBaseEntity::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	}

	if (pevInflictor->owner == edict())
		return 0;

	if (bitsDamageType & DMG_BLAST) {
		flDamage *= 2;
	}

	if (m_flRotortilt <= -90) {
		m_flRotortilt = 0;
	}
	else {
		m_flRotortilt -= 45;
	}
	SetBoneController(0, m_flRotortilt);

	// ALERT( at_console, "%.0f\n", flDamage );
	return CBaseEntity::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

//=========================================================
// Killed
//=========================================================
void COsprey::Killed(entvars_t *pevAttacker, int iGib)
{
	pev->movetype = MOVETYPE_TOSS;
	pev->gravity = 0.3;

	pev->velocity = m_velocity;
	pev->avelocity = Vector(RANDOM_FLOAT(-20, 20), 0, RANDOM_FLOAT(-50, 50));

	STOP_SOUND(ENT(pev), CHAN_STATIC, "apache/ap_rotor4.wav");
	STOP_SOUND(ENT(pev), CHAN_BODY, "apache/ap_whine1.wav");

	UTIL_SetSize(pev, Vector(-32, -32, -64), Vector(32, 32, 0));
	SetThink(&COsprey::DyingThink);
	SetTouch(&COsprey::CrashTouch);

	SetNextThink(0.1);
	pev->health = 0;
	pev->takedamage = DAMAGE_NO;

	m_startTime = UTIL_GlobalTimeBase() + 4.0;
}

//=========================================================
// CrashTouch
//=========================================================
void COsprey::CrashTouch(CBaseEntity *pOther)
{
	// only crash if we hit something solid
	if (pOther->pev->solid == SOLID_BSP)
	{
		SetTouch(NULL);
		m_startTime = UTIL_GlobalTimeBase();
		SetNextThink(0);
		m_velocity = pev->velocity;
	}
}

//=========================================================
// DyingThink
//=========================================================
void COsprey::DyingThink()
{
	StudioFrameAdvance();
	SetNextThink(0.1);

	pev->avelocity = pev->avelocity * 1.02;

	// still falling?
	if (m_startTime > UTIL_GlobalTimeBase())
	{
		UTIL_MakeAimVectors(pev->angles);
		ShowDamage();

		Vector vecSpot = pev->origin + pev->velocity * 0.2;

		if (CVAR_GET_FLOAT("cl_expdetail") != 0) {
			MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexFireball_0);
			WRITE_BYTE(1); // scale * 10 -50
			WRITE_BYTE(35); // framerate
			WRITE_BYTE(TE_EXPLFLAG_NONE);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD(pev->origin.x);	// Send to PAS because of the sound
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexFireball_1);
			WRITE_BYTE(1); // scale * 10 -50
			WRITE_BYTE(35); // framerate 15
			WRITE_BYTE(TE_EXPLFLAG_NONE);
			MESSAGE_END();

			// create explosion particle system
			if (CVAR_GET_FLOAT("r_particles") != 0) {
				MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
				WRITE_SHORT(0);
				WRITE_BYTE(0);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_SHORT(iExplosionDefault);
				MESSAGE_END();

				MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
				WRITE_SHORT(0);
				WRITE_BYTE(0);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_SHORT(iDefaultFinalFire);
				MESSAGE_END();

				MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
				WRITE_SHORT(0);
				WRITE_BYTE(0);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_COORD(0);
				WRITE_SHORT(iDefaultFinalSmoke);
				MESSAGE_END();
			}

			// Big Explosion
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_GLOWSPRITE);//Big Flare Effect
			WRITE_COORD(pev->origin.x); //where to make the sprite appear on x axis
			WRITE_COORD(pev->origin.y);//where to make the sprite appear on y axis
			WRITE_COORD(pev->origin.z);//where to make the sprite appear on zaxis
			WRITE_SHORT(g_sModelIndexFireballFlash); //Name of the sprite to use, as defined at begining of tut
			WRITE_BYTE(1); // scale
			WRITE_BYTE(30); // framerate 15
			WRITE_BYTE(80); // brightness
			MESSAGE_END();
		}

		// random explosions
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
		WRITE_BYTE(TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD(vecSpot.x + RANDOM_FLOAT(-150, 150));
		WRITE_COORD(vecSpot.y + RANDOM_FLOAT(-150, 150));
		WRITE_COORD(vecSpot.z + RANDOM_FLOAT(-150, -50));
		WRITE_SHORT(g_sModelIndexFireball);
		WRITE_BYTE(RANDOM_LONG(0, 29) + 30); // scale * 10
		WRITE_BYTE(12); // framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
		WRITE_SHORT(0);
		WRITE_BYTE(0);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(0);
		WRITE_COORD(0);
		WRITE_COORD(0);
		WRITE_SHORT(iDefaultSmoke);
		MESSAGE_END();

		// lots of smoke
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(vecSpot.x + RANDOM_FLOAT(-150, 150));
		WRITE_COORD(vecSpot.y + RANDOM_FLOAT(-150, 150));
		WRITE_COORD(vecSpot.z + RANDOM_FLOAT(-150, -50));
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(100); // scale * 10
		WRITE_BYTE(10); // framerate
		MESSAGE_END();


		vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
		WRITE_BYTE(TE_BREAKMODEL);

		// position
		WRITE_COORD(vecSpot.x);
		WRITE_COORD(vecSpot.y);
		WRITE_COORD(vecSpot.z);

		// size
		WRITE_COORD(800);
		WRITE_COORD(800);
		WRITE_COORD(132);

		// velocity
		WRITE_COORD(pev->velocity.x);
		WRITE_COORD(pev->velocity.y);
		WRITE_COORD(pev->velocity.z);

		// randomization
		WRITE_BYTE(50);

		// Model
		WRITE_SHORT(m_iTailGibs);	//model id#

		// # of shards
		WRITE_BYTE(8);	// let client decide

		// duration
		WRITE_BYTE(200);// 10.0 seconds

		// flags

		WRITE_BYTE(BREAK_METAL);
		MESSAGE_END();

		EMIT_SOUND_ARRAY_DYN(CHAN_ITEM, pExplodeSounds);
		EMIT_SOUND_ARRAY_DYN(CHAN_VOICE, pDebrisSounds);

		UTIL_ScreenShake(pev->origin, 12.0, 100.0, 2.0, 1000);

		// don't stop it we touch a entity
		pev->flags &= ~FL_ONGROUND;
		SetNextThink(0.2);
		return;
	}
	else
	{
		Vector vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;

		// fireball
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
		WRITE_BYTE(TE_SPRITE);
		WRITE_COORD(vecSpot.x);
		WRITE_COORD(vecSpot.y);
		WRITE_COORD(vecSpot.z + 512);
		WRITE_SHORT(m_iExplode);
		WRITE_BYTE(250); // scale * 10
		WRITE_BYTE(255); // brightness
		MESSAGE_END();

		// big smoke
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(vecSpot.x);
		WRITE_COORD(vecSpot.y);
		WRITE_COORD(vecSpot.z + 512);
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(250); // scale * 10
		WRITE_BYTE(5); // framerate
		MESSAGE_END();

		// blast circle
		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_BEAMCYLINDER);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z + 2000); // reach damage radius over .2 seconds
		WRITE_SHORT(m_iSpriteTexture);
		WRITE_BYTE(0); // startframe
		WRITE_BYTE(0); // framerate
		WRITE_BYTE(4); // life
		WRITE_BYTE(32);  // width
		WRITE_BYTE(0);   // noise
		WRITE_BYTE(255);   // r, g, b
		WRITE_BYTE(255);   // r, g, b
		WRITE_BYTE(192);   // r, g, b
		WRITE_BYTE(128); // brightness
		WRITE_BYTE(0);		// speed
		MESSAGE_END();

		if (pev->flags & FL_ONGROUND) {
			CBaseEntity *pWreckage = Create("cycler_wreckage", pev->origin, pev->angles);
			UTIL_SetSize(pWreckage->pev, Vector(-200, -200, -128), Vector(200, 200, -32));
			pWreckage->pev->frame = pev->frame;
			pWreckage->pev->sequence = pev->sequence;
			pWreckage->pev->framerate = 0;
			pWreckage->pev->dmgtime = UTIL_GlobalTimeBase() + 5;

			EMIT_SOUND(ENT(pev), CHAN_STATIC, "weapons/mortarhit.wav", VOL_NORM, 0.3);
			RadiusDamage(pev->origin, pev, pev, 600, CLASS_NONE, DMG_BLAST);
			UTIL_ScreenShake(pev->origin, 12.0, 100.0, 3.0, 8000);
		}

		// gibs
		vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;
		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, vecSpot);
		WRITE_BYTE(TE_BREAKMODEL);

		// position
		WRITE_COORD(vecSpot.x);
		WRITE_COORD(vecSpot.y);
		WRITE_COORD(vecSpot.z + 64);

		// size
		WRITE_COORD(800);
		WRITE_COORD(800);
		WRITE_COORD(128);

		// velocity
		WRITE_COORD(m_velocity.x);
		WRITE_COORD(m_velocity.y);
		WRITE_COORD(fabs(m_velocity.z) * 0.25);

		// randomization
		WRITE_BYTE(40);

		// Model
		WRITE_SHORT(m_iBodyGibs);	//model id#

		// # of shards
		WRITE_BYTE(128);

		// duration
		WRITE_BYTE(200);// 10.0 seconds

		// flags

		WRITE_BYTE(BREAK_METAL);
		MESSAGE_END();

		UTIL_Remove(this);
	}
}

//=========================================================
// ShowDamage
//=========================================================
void COsprey::ShowDamage()
{
	if (m_iDoLeftSmokePuff > 0 || RANDOM_LONG(0, 99) > m_flLeftHealth)
	{
		Vector vecSrc = pev->origin + gpGlobals->v_right * -340;
		if (CVAR_GET_FLOAT("r_particles") != 0) {
			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
			WRITE_SHORT(0);
			WRITE_BYTE(0);
			WRITE_COORD(vecSrc.x);
			WRITE_COORD(vecSrc.y);
			WRITE_COORD(vecSrc.z);
			WRITE_COORD(0);
			WRITE_COORD(0);
			WRITE_COORD(0);
			WRITE_SHORT(iDefaultFinalFire);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
			WRITE_SHORT(0);
			WRITE_BYTE(0);
			WRITE_COORD(vecSrc.x);
			WRITE_COORD(vecSrc.y);
			WRITE_COORD(vecSrc.z);
			WRITE_COORD(0);
			WRITE_COORD(0);
			WRITE_COORD(0);
			WRITE_SHORT(iDefaultFinalSmoke);
			MESSAGE_END();
		}

		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(RANDOM_LONG(0, 9) + 20); // scale * 10
		WRITE_BYTE(12); // framerate
		MESSAGE_END();

		if (m_iDoLeftSmokePuff > 0)
			m_iDoLeftSmokePuff--;
	}

	if (m_iDoRightSmokePuff > 0 || RANDOM_LONG(0, 99) > m_flRightHealth)
	{
		Vector vecSrc = pev->origin + gpGlobals->v_right * 340;
		if (CVAR_GET_FLOAT("r_particles") != 0) {
			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
			WRITE_SHORT(0);
			WRITE_BYTE(0);
			WRITE_COORD(vecSrc.x);
			WRITE_COORD(vecSrc.y);
			WRITE_COORD(vecSrc.z);
			WRITE_COORD(0);
			WRITE_COORD(0);
			WRITE_COORD(0);
			WRITE_SHORT(iDefaultFinalFire);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
			WRITE_SHORT(0);
			WRITE_BYTE(0);
			WRITE_COORD(vecSrc.x);
			WRITE_COORD(vecSrc.y);
			WRITE_COORD(vecSrc.z);
			WRITE_COORD(0);
			WRITE_COORD(0);
			WRITE_COORD(0);
			WRITE_SHORT(iDefaultFinalSmoke);
			MESSAGE_END();
		}

		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(RANDOM_LONG(0, 9) + 20); // scale * 10
		WRITE_BYTE(12); // framerate
		MESSAGE_END();

		if (m_iDoRightSmokePuff > 0)
			m_iDoRightSmokePuff--;
	}
}

//=========================================================
// TraceAttack
//=========================================================
void COsprey::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	if (!IsAlive()) {
		CBaseEntity::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
		return;
	}

	if (pev->spawnflags & SF_MONSTER_SPAWNFLAG_64) {
		CBaseEntity *pEnt = CBaseEntity::Instance(pevAttacker);
		if (pEnt->IsPlayer()) { return; }
		if (pevAttacker->owner) {
			pEnt = CBaseEntity::Instance(pevAttacker->owner);
			if (pEnt->IsPlayer()) { return; }
		}
	}

	switch (ptr->iHitgroup) {
	case HITGROUP_HEAD:
		if (m_flRightHealth < 0)
			return;
		else {
			m_flRightHealth -= (flDamage / 2);
			m_flLeftHealth -= (flDamage / 2);
		}
	case HITGROUP_STOMACH:
		if (m_flRightHealth < 0)
			return;
		else
			m_flRightHealth -= flDamage;
		m_iDoLeftSmokePuff = 3 + (flDamage / 5.0);
	case HITGROUP_CHEST:
		if (m_flLeftHealth < 0)
			return;
		else
			m_flLeftHealth -= flDamage;
		m_iDoRightSmokePuff = 3 + (flDamage / 5.0);
	default:
		UTIL_Sparks(ptr->vecEndPos);
	}

	// hit hard, hits cockpit, hits engines
	if (flDamage > 50 || ptr->iHitgroup == HITGROUP_HEAD
		|| ptr->iHitgroup == HITGROUP_CHEST || ptr->iHitgroup == HITGROUP_STOMACH) {
		// ALERT( at_console, "%.0f\n", flDamage );
		AddMultiDamage(pevAttacker, this, flDamage, bitsDamageType);
	}
}