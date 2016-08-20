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
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "entity_types.h"
#include "usercmd.h"
#include "pm_defs.h"
#include "pm_materials.h"

#include "eventscripts.h"
#include "ev_hldm.h"

#include "r_efx.h"
#include "event_api.h"
#include "event_args.h"
#include "in_defs.h"

#include <string.h>

#include "r_studioint.h"
#include "com_model.h"
#include "com_weapons.h"

//Weapons
#include "weapon_crowbar.h"
#include "weapon_knife.h"
#include "weapon_glock.h"
#include "weapon_python.h"
#include "weapon_mp5.h"
#include "weapon_shotgun.h"
#include "weapon_gauss.h"
#include "weapon_pipewrench.h"
#include "weapon_shockrifle.h"
#include "weapon_eagle.h"
#include "weapon_sporelauncher.h"
#include "weapon_sniperrifle.h"
#include "weapon_displacer.h"
#include "weapon_m249.h"
#include "weapon_egon.h"

extern engine_studio_api_t IEngineStudio;

static int tracerCount[ 32 ];

extern "C" char PM_FindTextureType( char *name );

void V_PunchAxis( int axis, float punch );
void VectorAngles( const float *forward, float *angles );
vec3_t previousorigin;//egon use this

extern cvar_t *cl_lw;

extern "C" {
	void EV_FireNull(event_args_t *args);
	void EV_FireCrowbar(struct event_args_s *args);
	void EV_FireWrenchSmall(struct event_args_s *args);
	void EV_FireWrenchLarge(struct event_args_s *args);
	void EV_FireKnife(struct event_args_s *args);
	void EV_FireGlock( struct event_args_s *args  );
	void EV_FireMP5( struct event_args_s *args  );
	void EV_FirePython( struct event_args_s *args  );
	void EV_FireGauss( struct event_args_s *args  );
	void EV_SpinGauss( struct event_args_s *args  );
	void EV_EgonFire( struct event_args_s *args  );
	void EV_EgonStop( struct event_args_s *args  );
	void EV_FireShotGun( struct event_args_s *args  );
	void EV_SnarkFire( struct event_args_s *args  );
	void EV_TrainPitchAdjust( struct event_args_s *args );
	void EV_PlayEmptySound( struct event_args_s *args );
	void EV_Decals( struct event_args_s *args );
	void EV_Explode( struct event_args_s *args );
	void EV_FireM249(struct event_args_s *args);
	void EV_ShockFire(struct event_args_s *args);
	void EV_FireEagle(struct event_args_s *args);
	void EV_FireSpore(struct event_args_s *args);
	void EV_FireSniper(struct event_args_s *args);
	void EV_Displacer(struct event_args_s *args);
}

#define VECTOR_CONE_1DEGREES Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES Vector( 0.06105, 0.06105, 0.06105 )	
#define VECTOR_CONE_8DEGREES Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES Vector( 0.17365, 0.17365, 0.17365 )

//hulls defines
#define VEC_HULL_MIN	Vector(-16, -16, -36)
#define VEC_HULL_MAX	Vector( 16,  16,  36)
#define VEC_HUMAN_HULL_MIN	Vector( -16, -16, 0 )
#define VEC_HUMAN_HULL_MAX	Vector( 16, 16, 72 )
#define VEC_HUMAN_HULL_DUCK	Vector( 16, 16, 36 )

#define VEC_DUCK_HULL_MIN	Vector(-16, -16, -18 )
#define VEC_DUCK_HULL_MAX	Vector( 16,  16,  18)

/*
=================
EV_MuzzleFlash
Flag weapon/view model for muzzle flash
=================
*/
void EV_Dynamic_MuzzleFlash(vec3_t pos, float amount, int red, int green, int blue, float die) {
	dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
	dl->die = gEngfuncs.GetClientTime() + die;
	dl->origin = pos;
	dl->color.r = red; // red
	dl->color.g = green; // green
	dl->color.b = blue; // blue
	dl->radius = amount * 100;
}

//=====================
// EV_WallPuffCallback
//=====================
void EV_WallPuffCallback(struct tempent_s *ent, float frametime, float currenttime) {
	ent->entity.angles = ent->entity.baseline.vuser1;
}

// play a strike sound based on the texture that was hit by the attack traceline.  VecSrc/VecEnd are the
// original traceline endpoints used by the attacker, iBulletType is the type of bullet that hit the texture.
// returns volume of strike instrument (crowbar) to play
float EV_HLDM_PlayTextureSound( int idx, pmtrace_t *ptr, float *vecSrc, float *vecEnd, int iBulletType ) {
	//CONPRINT("There was a serious error within the particle engine. Particles will return on map change\n");

	// hit the world, try to play sound based on texture material type
	vec3_t fwd, angles, forward, right, up;
	VectorAngles(ptr->plane.normal, angles);
	AngleVectors(angles, forward, up, right);
	forward.z = -forward.z;
	physent_t *pe;

	float fvol, fvol_impact, fvolbar, fattn;
	char *rgsz[4], *rgsz_impact[4], texname[64], szbuffer[64], *pTextureName, chTextureType;
	int cnt, impacts, entity, alpha, red, green, blue;
	bool fDoPuffs, fDoStreakSplash;

	entity = gEngfuncs.pEventAPI->EV_IndexFromTrace( ptr );

	//defaults
	chTextureType = 0;
	fDoPuffs = false;
	fDoStreakSplash = false;
	fvol_impact = 0.5;
	impacts = 0;
	cnt = 0;
	chTextureType = CHAR_TEX_CONCRETE;
	fattn = ATTN_NORM;
	alpha = 255; //wallsmoke alpha
	red = 97; //wallsmoke red
	green = 86; //wallsmoke green
	blue = 53; //wallsmoke blue

	// Player
	if ( entity >= 1 && entity <= gEngfuncs.GetMaxClients() ) {
		// hit body
		chTextureType = CHAR_TEX_FLESH;
	} else if ( entity == 0 ) {
		// get texture from entity or world (world is ent(0))
		pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture( ptr->ent, vecSrc, vecEnd );
		
		if ( pTextureName ) {
			strcpy( texname, pTextureName );
			pTextureName = texname;

			if (*pTextureName == '-' || *pTextureName == '+') {
				pTextureName += 2;
			}

			if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ') {
				pTextureName++;
			}
			
			strcpy( szbuffer, pTextureName );
			szbuffer[ CBTEXTURENAMEMAX - 1 ] = 0;
				
			// get texture type
			chTextureType = PM_FindTextureType( szbuffer );	
		}
	}

	VectorAdd(ptr->endpos, ptr->plane.normal, fwd);
	if (gEngfuncs.PM_PointContents(ptr->endpos, NULL) == CONTENT_SKY)
		return FALSE;

	physent_t *pEntity = gEngfuncs.pEventAPI->EV_GetPhysent(ptr->ent);
	if (pEntity->rendermode == kRenderTransAlpha && chTextureType == CHAR_TEX_CONCRETE)
		return FALSE;
	
	switch (chTextureType) {
		default:
		case CHAR_TEX_CONCRETE: 
			fvol = 0.9;	fvolbar = 0.6;
			rgsz[0] = "player/pl_step1.wav";
			rgsz[1] = "player/pl_step2.wav";
			cnt = 2;

			//Extended Impact-Bullet
			rgsz_impact[0] = "debris/concrete_impact_bullet1.wav";
			rgsz_impact[1] = "debris/concrete_impact_bullet2.wav";
			rgsz_impact[2] = "debris/concrete_impact_bullet3.wav";
			rgsz_impact[3] = "debris/concrete_impact_bullet4.wav";
			impacts = 4;
			alpha = 128; //wallsmoke alpha
			red = 50; //wallsmoke red
			green = 50; //wallsmoke green
			blue = 50; //wallsmoke blue
			fDoPuffs = true;
		break;
		case CHAR_TEX_METAL: 
			fvol = 0.9; fvolbar = 0.3;
			rgsz[0] = "player/pl_metal1.wav";
			rgsz[1] = "player/pl_metal2.wav";
			cnt = 2;

			//Extended Impact-Bullet
			rgsz_impact[0] = "debris/metal_impact_bullet1.wav";
			rgsz_impact[1] = "debris/metal_impact_bullet2.wav";
			rgsz_impact[2] = "debris/metal_impact_bullet3.wav";
			rgsz_impact[3] = "debris/metal_impact_bullet4.wav";
			impacts = 4;
			fDoStreakSplash = (iBulletType == BULLET_PLAYER_CROWBAR ? false : true);
		break;
		case CHAR_TEX_DIRT:	
			fvol = 0.9; fvolbar = 0.1;
			rgsz[0] = "player/pl_dirt1.wav";
			rgsz[1] = "player/pl_dirt2.wav";
			rgsz[2] = "player/pl_dirt3.wav";
			cnt = 3;

			//Extended Impact-Bullet
			fDoPuffs = true;
		break;
		case CHAR_TEX_VENT:	
			fvol = 0.5; fvolbar = 0.3;
			rgsz[0] = "player/pl_duct1.wav";
			rgsz[1] = "player/pl_duct1.wav";
			cnt = 2;

			//Extended Impact-Bullet
			rgsz_impact[0] = "debris/metal_impact_bullet1.wav";
			rgsz_impact[1] = "debris/metal_impact_bullet2.wav";
			rgsz_impact[2] = "debris/metal_impact_bullet3.wav";
			rgsz_impact[3] = "debris/metal_impact_bullet4.wav";
			impacts = 4;
			fDoStreakSplash = (iBulletType == BULLET_PLAYER_CROWBAR ? false : true);
		break;
		case CHAR_TEX_GRATE: 
			fvol = 0.9; fvolbar = 0.5;
			rgsz[0] = "player/pl_grate1.wav";
			rgsz[1] = "player/pl_grate4.wav";
			cnt = 2;

			//Extended Impact-Bullet
			rgsz_impact[0] = "debris/metal_impact_bullet1.wav";
			rgsz_impact[1] = "debris/metal_impact_bullet2.wav";
			rgsz_impact[2] = "debris/metal_impact_bullet3.wav";
			rgsz_impact[3] = "debris/metal_impact_bullet4.wav";
			impacts = 4;
			fDoStreakSplash = (iBulletType == BULLET_PLAYER_CROWBAR ? false : true);
		break;
		case CHAR_TEX_TILE:	
			fvol = 0.8; fvolbar = 0.2;
			rgsz[0] = "player/pl_tile1.wav";
			rgsz[1] = "player/pl_tile3.wav";
			rgsz[2] = "player/pl_tile2.wav";
			rgsz[3] = "player/pl_tile4.wav";
			cnt = 4;

			//Extended Impact-Bullet
			rgsz_impact[0] = "debris/tile_impact_bullet1.wav";
			rgsz_impact[1] = "debris/tile_impact_bullet2.wav";
			rgsz_impact[2] = "debris/tile_impact_bullet3.wav";
			rgsz_impact[3] = "debris/tile_impact_bullet4.wav";
			impacts = 4;
			fDoPuffs = true;
		break;
		case CHAR_TEX_SLOSH: 
			fvol = 0.9; fvolbar = 0.0;
			rgsz[0] = "player/pl_slosh1.wav";
			rgsz[1] = "player/pl_slosh3.wav";
			rgsz[2] = "player/pl_slosh2.wav";
			rgsz[3] = "player/pl_slosh4.wav";
			cnt = 4;
		break;
		case CHAR_TEX_GRASS: 
			fvol = 0.9; fvolbar = 0.2;
			rgsz[0] = "player/pl_grass1.wav";
			rgsz[1] = "player/pl_grass3.wav";
			rgsz[2] = "player/pl_grass2.wav";
			rgsz[3] = "player/pl_grass4.wav";
			cnt = 4;

			//Extended Impact-Bullet
			fDoPuffs = true;
		break;
		case CHAR_TEX_WOOD: 
			fvol = 0.9; fvolbar = 0.2;
			rgsz[0] = "debris/wood1.wav";
			rgsz[1] = "debris/wood2.wav";
			rgsz[2] = "debris/wood3.wav";
			cnt = 3;

			//Extended Impact-Bullet
			rgsz_impact[0] = "debris/wood_impact_bullet1.wav";
			rgsz_impact[1] = "debris/wood_impact_bullet2.wav";
			rgsz_impact[2] = "debris/wood_impact_bullet3.wav";
			rgsz_impact[3] = "debris/wood_impact_bullet4.wav";
			impacts = 4;
			fDoPuffs = true;
		break;
		case CHAR_TEX_GLASS:
			fvol = 0.8; fvolbar = 0.2;
			rgsz[0] = "debris/glass1.wav";
			rgsz[1] = "debris/glass2.wav";
			rgsz[2] = "debris/glass3.wav";
			cnt = 3;

			//Extended Impact-Bullet
			rgsz_impact[0] = "debris/glass_impact_bullet1.wav";
			rgsz_impact[1] = "debris/glass_impact_bullet2.wav";
			rgsz_impact[2] = "debris/glass_impact_bullet3.wav";
			rgsz_impact[3] = "debris/glass_impact_bullet4.wav";
			impacts = 4;
		break;
		case CHAR_TEX_COMPUTER:
			fvol = 0.8; fvolbar = 0.2;
			rgsz[0] = "buttons/spark1.wav";
			rgsz[1] = "buttons/spark2.wav";
			rgsz[2] = "buttons/spark3.wav";
			cnt = 3;

			//Extended Impact-Bullet
			rgsz_impact[0] = "debris/metal_impact_bullet1.wav";
			rgsz_impact[1] = "debris/metal_impact_bullet2.wav";
			rgsz_impact[2] = "debris/metal_impact_bullet3.wav";
			rgsz_impact[3] = "debris/metal_impact_bullet4.wav";
			impacts = 4;
			fDoStreakSplash = true;
		break;
		case CHAR_TEX_FLESH:
			if (iBulletType == BULLET_PLAYER_CROWBAR)
				return 0.0; // crowbar already makes this sound

			fvol = 1.0;	fvolbar = 0.2;
			rgsz[0] = "weapons/bullet_hit1.wav";
			rgsz[1] = "weapons/bullet_hit2.wav";
			fattn = 1.0;
			cnt = 2;
		break;
	}

	//add metal impact effect
	if (fDoStreakSplash) {
		int m_iMetalGlow = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/metal_glow.spr");
		gEngfuncs.pEfxAPI->R_TempSprite(ptr->endpos, vec3_origin, 0.1, m_iMetalGlow, kRenderGlow, kRenderFxNoDissipation, 150.0 / 200.0, 0.01, FTENT_FADEOUT );

		// spawn light
		EV_Dynamic_MuzzleFlash(ptr->endpos, 1 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.01);

		// spawn some sparks
		gEngfuncs.pEfxAPI->R_SparkShower(ptr->endpos);

		// Show Sparks
		gEngfuncs.pEfxAPI->R_SparkEffect(ptr->endpos, 2, -200, 200);

		gEngfuncs.pEfxAPI->R_StreakSplash(ptr->endpos, forward * gEngfuncs.pfnRandomFloat(-10, 10) + right * gEngfuncs.pfnRandomFloat(-6, 6) + up * gEngfuncs.pfnRandomFloat(0, 6), 0, 5, 20, 150, 100);
		gEngfuncs.pEfxAPI->R_StreakSplash(ptr->endpos, forward * gEngfuncs.pfnRandomFloat(-10, 10) + right * gEngfuncs.pfnRandomFloat(-6, 6) + up * gEngfuncs.pfnRandomFloat(0, 6), 9, 5, 20, 50, 50);
		gEngfuncs.pEfxAPI->R_StreakSplash(ptr->endpos, forward * gEngfuncs.pfnRandomFloat(-10, 10) + right * gEngfuncs.pfnRandomFloat(-6, 6) + up * gEngfuncs.pfnRandomFloat(0, 6), 0, 5, 20, 150, 100);
	}

	//add wallsmoke
	if (fDoPuffs) {
		int modelindex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/wallsmoke.spr");
		Vector position = ptr->endpos + ptr->plane.normal * 2;
		Vector velocity = ptr->plane.normal * gEngfuncs.pfnRandomFloat(8, 10);

		TEMPENTITY* pSmoke = gEngfuncs.pEfxAPI->R_TempSprite(
			position,												// position
			velocity,												// velocity
			gEngfuncs.pfnRandomFloat(30, 35) / 100, 				// scale
			modelindex, 											// model index
			kRenderNormal, 											// rendermode
			kRenderFxNoDissipation, 								// renderfx
			gEngfuncs.pfnRandomFloat(0.25, 0.5), 					// alpha
			0.01, 													// life
			FTENT_SPRCYCLE | FTENT_FADEOUT);						// flags

		if (pSmoke) {
			Vector angles;
			VectorAngles(velocity, angles);
			pSmoke->flags |= FTENT_CLIENTCUSTOM;
			pSmoke->entity.curstate.framerate = 4;
			pSmoke->entity.baseline.vuser1 = angles;
			pSmoke->entity.curstate.renderamt = alpha;
			pSmoke->entity.curstate.rendercolor.r = red;
			pSmoke->entity.curstate.rendercolor.g = green;
			pSmoke->entity.curstate.rendercolor.b = blue;
			pSmoke->callback = EV_WallPuffCallback;
		}
	}

	//play extended impact-bullet sound
	if (impacts >= 1) {
		gEngfuncs.pEventAPI->EV_PlaySound(-1, ptr->endpos, CHAN_STATIC, rgsz_impact[gEngfuncs.pfnRandomLong(0, impacts - 1)], 0.5, ATTN_NORM, 0, 96 + gEngfuncs.pfnRandomLong(0, 0xf));
	}

	// play material hit sound
	if (cnt >= 1) {
		gEngfuncs.pEventAPI->EV_PlaySound(0, ptr->endpos, CHAN_STATIC, rgsz[gEngfuncs.pfnRandomLong(0, cnt - 1)], fvol, fattn, 0, 96 + gEngfuncs.pfnRandomLong(0, 0xf));
	}
	
	return fvolbar;
}

//======================
//	    MIRROR UTILS
//======================
vec3_t EV_GetMirrorOrigin(int mirror_index, vec3_t pos) {
	vec3_t result;
	VectorCopy(pos, result);

	switch (gHUD.Mirrors[mirror_index].type)
	{
	case 0:
		result[0] = gHUD.Mirrors[mirror_index].origin[0]*2 - pos[0];
		break;
	case 1:
		result[1] = gHUD.Mirrors[mirror_index].origin[1]*2 - pos[1];
		break;
	case 2:
	default:
		result[2] = gHUD.Mirrors[mirror_index].origin[2]*2 - pos[2];
		break;
	}
	return result;
}

vec3_t EV_GetMirrorAngles (int mirror_index, vec3_t angles ) {
	vec3_t result;
	VectorCopy(angles, result);

	switch (gHUD.Mirrors[mirror_index].type)
	{
	case 0:
		result[0] = -result[0]; 
		break;
	case 1:
		result[1] = -result[1]; 
		break;
	case 2:
	default:
		result[2] = -result[2]; 
		break;
	}
	return result;
}

vec3_t EV_MirrorVector( vec3_t angles ) {
	vec3_t result;
	VectorCopy(angles, result);

	if (gHUD.numMirrors) {
		for (int imc=0; imc < gHUD.numMirrors; imc++) {
			if (!gHUD.Mirrors[imc].enabled)
				continue;
			VectorCopy(EV_GetMirrorAngles(imc, angles), result);
		}
	}
	return result;
}

vec3_t EV_MirrorPos( vec3_t endpos ) {
	vec3_t mirpos(0, 0, 0);
	if (gHUD.numMirrors) {
		for (int imc=0; imc < gHUD.numMirrors; imc++) {
			if (!gHUD.Mirrors[imc].enabled)
				continue;
			
			vec3_t delta;
			float dist;
			
			VectorSubtract(gHUD.Mirrors[imc].origin, endpos, delta);
			dist = Length(delta);

			if (gHUD.Mirrors[imc].radius < dist)
				continue;
		
			VectorCopy(EV_GetMirrorOrigin(imc, endpos), mirpos);
		}
	}

	return mirpos;
}

//======================
//	 END MIRROR UTILS
//======================

//======================
//	START DECALS
//======================
void EV_HLDM_FindHullIntersection( int idx, vec3_t vecSrc, pmtrace_t pTrace, float *mins, float *maxs )
{
	int			i, j, k;
	float		distance;
	float		*minmaxs[2] = {mins, maxs};
	pmtrace_t tmpTrace;
	vec3_t		vecHullEnd = pTrace.endpos;
	vec3_t		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();
	
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecHullEnd, PM_STUDIO_BOX, -1, &tmpTrace );
	gEngfuncs.pEventAPI->EV_PopPMStates();
	
	if ( tmpTrace.fraction < 1.0 ) {
		pTrace = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ ) {
		for ( j = 0; j < 2; j++ ) {
			for ( k = 0; k < 2; k++ ) {
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];
                                        
                gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

                // Store off the old count
				gEngfuncs.pEventAPI->EV_PushPMStates();
	
				// Now add in all of the players.
				gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

				gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
				gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tmpTrace );
				gEngfuncs.pEventAPI->EV_PopPMStates();
				
				if ( tmpTrace.fraction < 1.0 ) {
					float thisDistance = (tmpTrace.endpos - vecSrc).Length();
					if ( thisDistance < distance ) {
						pTrace = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

char *EV_HLDM_DamageDecal(pmtrace_t *ptr, physent_t *pe, float *vecSrc, float *vecEnd)
{
	// hit the world, try to play sound based on texture material type
	char chTextureType = 0;
	int entity;
	char *pTextureName;
	char texname[64];
	char szbuffer[64];
	static char decalname[32];
	int idx;

	entity = gEngfuncs.pEventAPI->EV_IndexFromTrace(ptr);

	if (pe && pe->solid == SOLID_BSP) {
		// Nothing
		if (vecSrc == 0 && vecEnd == 0) {
			// hit body
			chTextureType = 0;
		} else {
			// get texture from entity or world (world is ent(0))
			pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture(ptr->ent, vecSrc, vecEnd);

			if (pTextureName) {
				strcpy(texname, pTextureName);
				pTextureName = texname;

				if (*pTextureName == '-' || *pTextureName == '+') {
					pTextureName += 2;
				}

				if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ') {
					pTextureName++;
				}

				strcpy(szbuffer, pTextureName);
				szbuffer[CBTEXTURENAMEMAX - 1] = 0;

				// get texture type
				chTextureType = PM_FindTextureType(szbuffer);
			}
		}
	}

	if (pe->rendermode == kRenderTransAlpha)
		return FALSE;

	if ( pe->classnumber == 1 ) {
		idx = gEngfuncs.pfnRandomLong( 0, 2 );
		snprintf( decalname, 32, "{break%i", idx + 1 );
	} else if (pe->classnumber == 2) {
		snprintf(decalname, 32, "{knife");
	} else if ( pe->rendermode != kRenderNormal ) {
		snprintf( decalname, 32, "{bproof1" );
	} else {
		switch (chTextureType) {
			default:
				idx = gEngfuncs.pfnRandomLong(0, 4);
				snprintf(decalname, 32, "{shot%i", idx + 1);
			break;
			case CHAR_TEX_CONCRETE:
				idx = gEngfuncs.pfnRandomLong(0, 2);
				snprintf(decalname, 32, "{hole_conc%i", idx + 1);
			break;
			case CHAR_TEX_METAL:
			case CHAR_TEX_GRATE:
				idx = gEngfuncs.pfnRandomLong(0, 2);
				snprintf(decalname, 32, "{hole_metal%i", idx + 1);
			break;
			case CHAR_TEX_DIRT:
				idx = gEngfuncs.pfnRandomLong(0, 2);
				snprintf(decalname, 32, "{hole_dirt%i", idx + 1);
			break;
			case CHAR_TEX_WOOD:
				idx = gEngfuncs.pfnRandomLong(0, 2);
				snprintf(decalname, 32, "{hole_wood%i", idx + 1);
			break;
			case CHAR_TEX_COMPUTER:
				idx = gEngfuncs.pfnRandomLong(0, 2);
				snprintf(decalname, 32, "{hole_comp%i", idx + 1);
			break;
			case CHAR_TEX_GLASS:
				idx = gEngfuncs.pfnRandomLong(0, 2);
				snprintf(decalname, 32, "{break%i", idx + 1);
			break;
		}
	}

	return decalname;
}

void EV_HLDM_CrowbarDecalTrace(pmtrace_t *pTrace, char *decalName )
{
	physent_t *pe;

	pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );
	if(!pe) return; //g-cont. fix old bug that crashes Spirit
          
	struct model_s *mdl = pe->model;

	// Only decal brush models such as the world etc.
	if (  decalName && decalName[0] && pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
	{
		if ( CVAR_GET_FLOAT( "r_decals" ) )
		{
			int decal_tex = gEngfuncs.pEfxAPI->Draw_DecalIndex( gEngfuncs.pEfxAPI->Draw_DecalIndexFromName( decalName ) );
			gEngfuncs.pEfxAPI->R_DecalShoot( decal_tex, gEngfuncs.pEventAPI->EV_IndexFromTrace( pTrace ), 0, pTrace->endpos, 0 );
			
			vec3_t mirpos = EV_MirrorPos(pTrace->endpos); 
			if(mirpos != vec3_t(0,0,0))
				gEngfuncs.pEfxAPI->R_DecalShoot(decal_tex,0, 0, mirpos, 0 );
		}
	}
}

void EV_HLDM_GunshotDecalTrace(pmtrace_t *pTrace, char *decalName )
{
	physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );
	if (gEngfuncs.PM_PointContents(pTrace->endpos, NULL) != CONTENT_SKY) {
		if (gEngfuncs.pfnGetCvarFloat("r_particle") == 0) {
			gEngfuncs.pEfxAPI->R_BulletImpactParticles(pTrace->endpos);
		}

		// Only decal brush models such as the world etc.
		if (  decalName && decalName[0] && pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
		{
			if ( CVAR_GET_FLOAT( "r_decals" ) )
			{
				int decal_tex = gEngfuncs.pEfxAPI->Draw_DecalIndex( gEngfuncs.pEfxAPI->Draw_DecalIndexFromName( decalName ) );
				gEngfuncs.pEfxAPI->R_DecalShoot( 
				decal_tex, 
				gEngfuncs.pEventAPI->EV_IndexFromTrace( pTrace ), 0, pTrace->endpos, 0 );
				gEngfuncs.pEfxAPI->R_SparkStreaks(pTrace->endpos, gEngfuncs.pfnRandomLong(10,25),-120,120);
			
				vec3_t mirpos = EV_MirrorPos(pTrace->endpos); 
				if(mirpos != vec3_t(0,0,0)) {
					//Mirror decal!
					gEngfuncs.pEfxAPI->R_DecalShoot(decal_tex,0, 0, mirpos, 0 );
					gEngfuncs.pEfxAPI->R_SparkStreaks(mirpos,gEngfuncs.pfnRandomLong(10,25),-120,120);
				}
			}

			int iRand = gEngfuncs.pfnRandomLong(0, 0x7FFF);
			if (iRand < (0x7fff / 2)) {
				switch (iRand % 5) {
					case 0:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, CHAN_STATIC, "weapons/ric1.wav", VOL_LOW, ATTN_NORM, 0, PITCH_NORM); break;
					case 1:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, CHAN_STATIC, "weapons/ric2.wav", VOL_LOW, ATTN_NORM, 0, PITCH_NORM); break;
					case 2:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, CHAN_STATIC, "weapons/ric3.wav", VOL_LOW, ATTN_NORM, 0, PITCH_NORM); break;
					case 3:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, CHAN_STATIC, "weapons/ric4.wav", VOL_LOW, ATTN_NORM, 0, PITCH_NORM); break;
					case 4:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, CHAN_STATIC, "weapons/ric5.wav", VOL_LOW, ATTN_NORM, 0, PITCH_NORM); break;
				}
			}
		}
	}
}

void EV_HLDM_DecalGunshot(pmtrace_t *pTrace, int iBulletType, float *vecSrc, float *vecEnd)
{
	physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );
	if ( pe && pe->solid == SOLID_BSP )
	{
		switch( iBulletType )
		{
			case BULLET_PLAYER_CROWBAR:
				EV_HLDM_CrowbarDecalTrace( pTrace, EV_HLDM_DamageDecal(pTrace, pe, vecSrc, vecEnd));
				break;
			case BULLET_PLAYER_9MM:
			case BULLET_MONSTER_9MM:
			case BULLET_PLAYER_MP5:
			case BULLET_MONSTER_MP5:
			case BULLET_PLAYER_556:
			case BULLET_MONSTER_556:
			case BULLET_PLAYER_BUCKSHOT:
			case BULLET_PLAYER_357:
			case BULLET_PLAYER_762:
			case BULLET_MONSTER_357:
			default:
				EV_HLDM_GunshotDecalTrace(pTrace, EV_HLDM_DamageDecal(pTrace, pe, vecSrc, vecEnd));
			break;
		}
	}
}

int EV_HLDM_CheckTracer( int idx, float *vecSrc, float *end, float *forward, float *right, int iBulletType, int iTracerFreq, int *tracerCount )
{
	int tracer = 0;
	int i;
	qboolean player = idx >= 1 && idx <= gEngfuncs.GetMaxClients() ? true : false;

	if ( iTracerFreq != 0 && ( (*tracerCount)++ % iTracerFreq) == 0 ) {
		vec3_t vecTracerSrc;
		if ( player ) {
			vec3_t offset( 0, 0, -4 );
			// adjust tracer position for player
			for ( i = 0; i < 3; i++ ) {
				vecTracerSrc[ i ] = vecSrc[ i ] + offset[ i ] + right[ i ] * 2 + forward[ i ] * 16;
			}
		} else {
			VectorCopy( vecSrc, vecTracerSrc );
		}
		
		if ( iTracerFreq != 1 )		// guns that always trace also always decal
			tracer = 1;

		switch( iBulletType ) {
			case BULLET_PLAYER_MP5:
			case BULLET_MONSTER_MP5:
			case BULLET_PLAYER_556:
			case BULLET_MONSTER_357:
			case BULLET_MONSTER_556:
			case BULLET_MONSTER_9MM:
			case BULLET_MONSTER_12MM:
			default:
				EV_CreateTracer( vecTracerSrc, end );
			break;
		}
	}

	return tracer;
}

//======================
//	END DECALS
//======================

//======================
//	Play Empty Sound
//======================
void EV_PlayEmptySound( event_args_s *args ) {
	if (args->iparam2 == 0) 
		args->iparam2 = PITCH_NORM;

	switch (args->iparam1) {
		default: gEngfuncs.pEventAPI->EV_PlaySound(args->entindex, args->origin, CHAN_AUTO, "weapons/cock1.wav", 0.8, ATTN_NORM, 0, args->iparam2); break;
		case 1: gEngfuncs.pEventAPI->EV_PlaySound(args->entindex, args->origin, CHAN_AUTO, "buttons/button10.wav", 0.8, ATTN_NORM, 0, args->iparam2); break;
		case 2: gEngfuncs.pEventAPI->EV_PlaySound(args->entindex, args->origin, CHAN_AUTO, "buttons/button11.wav", 0.8, ATTN_NORM, 0, args->iparam2); break;
		case 3: gEngfuncs.pEventAPI->EV_PlaySound(args->entindex, args->origin, CHAN_AUTO, "buttons/lightswitch2.wav", 0.8, ATTN_NORM, 0, args->iparam2); break;
		case 4: gEngfuncs.pEventAPI->EV_PlaySound(args->entindex, args->origin, CHAN_AUTO, "weapons/shotgun_empty.wav", 0.8, ATTN_NORM, 0, args->iparam2); break;
		case 5: gEngfuncs.pEventAPI->EV_PlaySound(args->entindex, args->origin, CHAN_AUTO, "shockroach/shock_angry.wav", 0.8, ATTN_NORM, 0, args->iparam2); break;
	}
}

/*
================
FireBullets

Go to the trouble of combining multiple pellets into a single damage call.
================
*/
void EV_HLDM_FireBullets( int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, float flSpreadX, float flSpreadY )
{
	int i;
	pmtrace_t tr;
	int iShot;
	int tracer;
	
	for ( iShot = 1; iShot <= cShots; iShot++ )	
	{
		vec3_t vecDir, vecEnd;
			
		float x, y, z;
		//We randomize for the Shotgun.
		if ( iBulletType == BULLET_PLAYER_BUCKSHOT )
		{
			do {
				x = gEngfuncs.pfnRandomFloat(-0.5,0.5) + gEngfuncs.pfnRandomFloat(-0.5,0.5);
				y = gEngfuncs.pfnRandomFloat(-0.5,0.5) + gEngfuncs.pfnRandomFloat(-0.5,0.5);
				z = x*x+y*y;
			} while (z > 1);

			for ( i = 0 ; i < 3; i++ )
			{
				vecDir[i] = vecDirShooting[i] + x * flSpreadX * right[ i ] + y * flSpreadY * up [ i ];
				vecEnd[i] = vecSrc[ i ] + flDistance * vecDir[ i ];
			}
		}//But other guns already have their spread randomized in the synched spread.
		else
		{
			for ( i = 0 ; i < 3; i++ )
			{
				vecDir[i] = vecDirShooting[i] + flSpreadX * right[ i ] + flSpreadY * up [ i ];
				vecEnd[i] = vecSrc[ i ] + flDistance * vecDir[ i ];
			}
		}

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
	
		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();
	
		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

		tracer = EV_HLDM_CheckTracer( idx, vecSrc, tr.endpos, forward, right, iBulletType, iTracerFreq, tracerCount );

		// do damage, paint decals
		if ( tr.fraction != 1.0 ) {
			switch(iBulletType) {
			default:
			case BULLET_PLAYER_9MM:		
				EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
				EV_HLDM_DecalGunshot(&tr, iBulletType, vecSrc, vecEnd);
			break;
			case BULLET_PLAYER_556:
			case BULLET_PLAYER_MP5:		
				if ( !tracer ) {
					EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
					EV_HLDM_DecalGunshot(&tr, iBulletType, vecSrc, vecEnd);
				}
				break;
			case BULLET_PLAYER_BUCKSHOT:
				EV_HLDM_PlayTextureSound(idx, &tr, vecSrc, vecEnd, iBulletType);
				EV_HLDM_DecalGunshot(&tr, iBulletType, vecSrc, vecEnd);
				break;
			case BULLET_PLAYER_357:
				EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
				EV_HLDM_DecalGunshot(&tr, iBulletType, vecSrc, vecEnd);
				break;
			case BULLET_PLAYER_762:
				EV_HLDM_PlayTextureSound(idx, &tr, vecSrc, vecEnd, iBulletType);
				EV_HLDM_DecalGunshot(&tr, iBulletType, vecSrc, vecEnd);
				break;
			}
		}

		gEngfuncs.pEventAPI->EV_PopPMStates();
	}
}

//======================
//	   CROWBAR START
//======================
void EV_FireCrowbar(event_args_t *args) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
    vec3_t up, right, forward;
	physent_t *pHit;
	pmtrace_t tr;
	
	idx = args->entindex;
	VectorCopy( args->angles, angles );
	VectorCopy( args->origin, origin );
	vec3_t vecSrc, vecEnd;
          
    AngleVectors( angles, forward, right, up );
	
	EV_GetGunPosition( args, vecSrc, origin );
	vecEnd = vecSrc + forward * 32;	
          
	//make trace 
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();

	if ( tr.fraction >= 1.0 ) {
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	
		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );
		gEngfuncs.pEventAPI->EV_PopPMStates();

		if ( tr.fraction < 1.0 ) {
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			pHit = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );

			if ( !pHit || pHit->solid == SOLID_BSP )
				EV_HLDM_FindHullIntersection( idx, vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX );
				vecEnd = tr.endpos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}

	if ( tr.fraction >= 1.0 ) {
		if(args->iparam2) {
			if ( EV_IsLocal( idx ) ) {
				// miss
				switch(args->bparam2) {
					case 0: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)CROWBAR_ATTACK1MISS::sequence, args->iparam1 ); break;
					case 1: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)CROWBAR_ATTACK2MISS::sequence, args->iparam1 ); break;
					case 2: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)CROWBAR_ATTACK3MISS::sequence, args->iparam1 ); break;
				}
			}

			// play wiff or swish sound
			gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/cbar_miss1.wav", 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xF ));
		}
	} else {
		if ( EV_IsLocal( idx ) ) {
			switch(args->bparam2) {
				case 0: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)CROWBAR_ATTACK1::sequence, args->iparam1 ); break;
				case 1: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)CROWBAR_ATTACK2::sequence, args->iparam1 ); break;
				case 2: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)CROWBAR_ATTACK3::sequence, args->iparam1 ); break;
			}
		}

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;
  		pHit = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );
  		
		if (pHit && args->bparam1) {
			switch(gEngfuncs.pfnRandomLong(0,2)) {
				case 0: gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/cbar_hitbod1.wav", 1, ATTN_NORM, 0, PITCH_NORM); break;
				case 1: gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/cbar_hitbod2.wav", 1, ATTN_NORM, 0, PITCH_NORM); break;
				case 2: gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/cbar_hitbod3.wav", 1, ATTN_NORM, 0, PITCH_NORM); break;
			}
			fHitWorld = FALSE;
		}

		if (fHitWorld) {
			float fvolbar = EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecSrc + (vecEnd-vecSrc)*2, BULLET_PLAYER_CROWBAR );

			// also play crowbar strike
			switch( gEngfuncs.pfnRandomLong(0,1) ) {
				case 0: gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/cbar_hit1.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0,3)); break;
				case 1: gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/cbar_hit2.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0,3)); break;
			}

			// delay the decal a bit
			EV_HLDM_DecalGunshot( &tr, BULLET_PLAYER_CROWBAR, NULL, NULL );
		}
	}
}
//======================
//	   CROWBAR END 
//======================

//======================
//	   PIPE_WRENCH START
//======================
void EV_FireWrenchSmall(struct event_args_s *args) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t up, right, forward;
	physent_t *pHit;
	pmtrace_t tr;

	idx = args->entindex;
	VectorCopy(args->angles, angles);
	VectorCopy(args->origin, origin);
	vec3_t vecSrc, vecEnd;

	AngleVectors(angles, forward, right, up);

	EV_GetGunPosition(args, vecSrc, origin);
	vecEnd = vecSrc + forward * 32;

	//make trace 
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr);
	gEngfuncs.pEventAPI->EV_PopPMStates();

	if (tr.fraction >= 1.0) {
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr);
		gEngfuncs.pEventAPI->EV_PopPMStates();

		if (tr.fraction < 1.0) {
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			pHit = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

			if (!pHit || pHit->solid == SOLID_BSP)
				EV_HLDM_FindHullIntersection(idx, vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
			vecEnd = tr.endpos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}

	if (tr.fraction >= 1.0) {
		if (args->iparam2) {
			if (EV_IsLocal(idx)) {
				// miss
				switch (args->bparam2) {
					case 0: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)PIPE_WRENCH_ATTACK1MISS::sequence, args->iparam1); break;
					case 1: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)PIPE_WRENCH_ATTACK2MISS::sequence, args->iparam1); break;
					case 2: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)PIPE_WRENCH_ATTACK3MISS::sequence, args->iparam1); break;
				}
			}

			// play wiff or swish sound
			switch (gEngfuncs.pfnRandomLong(0, 1)) {
				case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_miss1.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xF)); break;
				case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_miss2.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xF)); break;
			}
		}
	}
	else {
		if (EV_IsLocal(idx)) {
			switch (args->bparam2) {
				case 0: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)PIPE_WRENCH_ATTACK1::sequence, args->iparam1); break;
				case 1: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)PIPE_WRENCH_ATTACK2::sequence, args->iparam1); break;
				case 2: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)PIPE_WRENCH_ATTACK3::sequence, args->iparam1); break;
			}
		}

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;
		pHit = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

		if (pHit && args->bparam1) {
			switch (gEngfuncs.pfnRandomLong(0, 1)) {
				case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_hitbod1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
				case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_hitbod2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
			}
			fHitWorld = FALSE;
		}

		if (fHitWorld) {
			float fvolbar = EV_HLDM_PlayTextureSound(idx, &tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);

			// also play crowbar strike
			switch (gEngfuncs.pfnRandomLong(0, 1)) {
				case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_hit1.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3)); break;
				case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_hit2.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3)); break;
			}

			// delay the decal a bit
			EV_HLDM_DecalGunshot(&tr, BULLET_PLAYER_CROWBAR, NULL, NULL);
		}
	}
}

void EV_FireWrenchLarge(struct event_args_s *args) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t up, right, forward;
	physent_t *pHit;
	pmtrace_t tr;

	idx = args->entindex;
	VectorCopy(args->angles, angles);
	VectorCopy(args->origin, origin);
	vec3_t vecSrc, vecEnd;

	AngleVectors(angles, forward, right, up);

	EV_GetGunPosition(args, vecSrc, origin);
	vecEnd = vecSrc + forward * 32;

	//make trace 
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr);
	gEngfuncs.pEventAPI->EV_PopPMStates();

	if (tr.fraction >= 1.0) {
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr);
		gEngfuncs.pEventAPI->EV_PopPMStates();

		if (tr.fraction < 1.0) {
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			pHit = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

			if (!pHit || pHit->solid == SOLID_BSP)
				EV_HLDM_FindHullIntersection(idx, vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
			vecEnd = tr.endpos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}

	if (tr.fraction >= 1.0) {
		if (EV_IsLocal(idx)) {
			// miss
			gEngfuncs.pEventAPI->EV_WeaponAnimation((int)PIPE_WRENCH_ATTACKBIGMISS::sequence, args->iparam1);
		}

		// play wiff or swish sound
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_big_miss.wav", 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xF));
	} else {
		if (EV_IsLocal(idx)) {
			gEngfuncs.pEventAPI->EV_WeaponAnimation((int)PIPE_WRENCH_ATTACKBIGHIT::sequence, args->iparam1);
		}

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;
		pHit = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);
		if (pHit && args->bparam1) {
			switch (gEngfuncs.pfnRandomLong(0, 1)) {
				case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_big_hitbod1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
				case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_big_hitbod2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
			}
			fHitWorld = FALSE;
		}

		if (fHitWorld) {
			float fvolbar = EV_HLDM_PlayTextureSound(idx, &tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);
			// also play crowbar strike
			switch (gEngfuncs.pfnRandomLong(0, 1)) {
				case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_hit1.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3)); break;
				case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/pwrench_hit2.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3)); break;
			}

			// delay the decal a bit
			EV_HLDM_DecalGunshot(&tr, BULLET_PLAYER_CROWBAR, NULL, NULL);
		}
	}
}
//======================
//	   PIPE_WRENCH END 
//======================

//======================
//	   KNIFE START
//======================
void EV_FireKnife(event_args_t *args) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t up, right, forward;
	physent_t *pHit;
	pmtrace_t tr;

	idx = args->entindex;
	VectorCopy(args->angles, angles);
	VectorCopy(args->origin, origin);
	vec3_t vecSrc, vecEnd;

	AngleVectors(angles, forward, right, up);

	EV_GetGunPosition(args, vecSrc, origin);
	vecEnd = vecSrc + forward * 32;

	//make trace 
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr);
	gEngfuncs.pEventAPI->EV_PopPMStates();

	if (tr.fraction >= 1.0) {
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr);
		gEngfuncs.pEventAPI->EV_PopPMStates();

		if (tr.fraction < 1.0) {
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			pHit = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

			if (!pHit || pHit->solid == SOLID_BSP)
				EV_HLDM_FindHullIntersection(idx, vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
			vecEnd = tr.endpos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}

	if (!args->fparam1) { //PrimaryAttack
		if (tr.fraction >= 1.0) {
			if (args->iparam2) {
				if (EV_IsLocal(idx)) {
					// miss
					switch (args->bparam2) {
						case 0: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)KNIFE_ATTACK1MISS::sequence, args->iparam1); break;
						case 1: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)KNIFE_ATTACK2MISS::sequence, args->iparam1); break;
						case 2: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)KNIFE_ATTACK3MISS::sequence, args->iparam1); break;
					}
				}

				// play wiff or swish sound
				switch (gEngfuncs.pfnRandomLong(0, 2)) {
					case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
					case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
					case 2: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
				}
			}
		} else {
			if (EV_IsLocal(idx)) {
				switch (args->bparam2) {
					case 0: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)KNIFE_ATTACK1::sequence, args->iparam1); break;
					case 1: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)KNIFE_ATTACK2::sequence, args->iparam1); break;
					case 2: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)KNIFE_ATTACK3::sequence, args->iparam1); break;
				}
			}

			// play thwack, smack, or dong sound
			float flVol = 1.0;
			int fHitWorld = TRUE;
			pHit = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

			if (pHit && args->bparam1) {
				switch (gEngfuncs.pfnRandomLong(0, 1)) {
					case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife_hit_flesh1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
					case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife_hit_flesh2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
				}
				fHitWorld = FALSE;
			}

			if (fHitWorld) {
				float fvolbar = EV_HLDM_PlayTextureSound(idx, &tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);

				// also play crowbar strike
				switch (gEngfuncs.pfnRandomLong(0, 1)) {
					case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife_hit_wall1.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3)); break;
					case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife_hit_wall2.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3)); break;
				}

				// delay the decal a bit
				EV_HLDM_DecalGunshot(&tr, BULLET_PLAYER_CROWBAR, NULL, NULL);
			}
		}
	} else { //SecondaryAttack
		if (!args->iparam2) { //Charge
			gEngfuncs.pEventAPI->EV_WeaponAnimation((int)KNIFE_CHARGE::sequence, args->iparam1);
		} else { //Stab
			if (tr.fraction >= 1.0) {
				if (args->iparam2) {
					if (EV_IsLocal(idx)) {
						gEngfuncs.pEventAPI->EV_WeaponAnimation((int)KNIFE_STAB::sequence, args->iparam1);
					}

					// play wiff or swish sound
					switch (gEngfuncs.pfnRandomLong(0, 2)) {
						case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
						case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
						case 2: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
					}
				}
			} else {
				if (EV_IsLocal(idx)) {
					gEngfuncs.pEventAPI->EV_WeaponAnimation((int)KNIFE_STAB::sequence, args->iparam1);
				}

				// play thwack, smack, or dong sound
				float flVol = 1.0;
				int fHitWorld = TRUE;
				pHit = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

				if (pHit && args->bparam1) {
					switch (gEngfuncs.pfnRandomLong(0, 1)) {
						case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife_hit_flesh1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
						case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife_hit_flesh2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM); break;
					}
					fHitWorld = FALSE;
				}

				if (fHitWorld) {
					float fvolbar = EV_HLDM_PlayTextureSound(idx, &tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);

					// also play crowbar strike
					switch (gEngfuncs.pfnRandomLong(0, 1)) {
						case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife_hit_wall1.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3)); break;
						case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/knife_hit_wall2.wav", fvolbar, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3)); break;
					}

					// delay the decal a bit
					EV_HLDM_DecalGunshot(&tr, BULLET_PLAYER_CROWBAR, NULL, NULL);
				}
			}
		}
	}
}
//======================
//	   KNIFE END 
//======================

//======================
//	    GLOCK START
//======================
void EV_FireGlock( event_args_t *args ) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;

	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	empty = args->bparam1;
	AngleVectors( angles, forward, right, up );

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/shell.mdl");// brass shell

	if ( EV_IsLocal( idx ) ) {
		cl_entity_s *view = gEngfuncs.GetViewModel();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( empty ? (int)GLOCK_SHOOT_EMPTY::sequence : (int)GLOCK_SHOOT::sequence, args->iparam1 );
		EV_MuzzleFlash();

		gEngfuncs.pEfxAPI->R_MuzzleFlash( (float *)&view->attachment[0], 11 );
		V_PunchAxis( 0, -2.0 );
	}

	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/pl_gun3.wav", gEngfuncs.pfnRandomFloat(0.92, 1.0), ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong( 0, 3 ) );

	EV_GetGunPosition( args, vecSrc, origin );
	
	VectorCopy( forward, vecAiming );

	EV_Dynamic_MuzzleFlash(vecSrc, 1.5 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.01);
	EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_9MM, 0, 0, args->fparam1, args->fparam2 );
}
//======================
//	   GLOCK END
//======================

//======================
//	    MP5 START
//======================
void EV_FireMP5( event_args_t *args ) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int body;
	
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	float flSpread = 0.01;

	idx = args->entindex;
	body = args->iparam1;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	AngleVectors( angles, forward, right, up );

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/shell.mdl");// brass shell
	
	if ( EV_IsLocal( idx ) ) {
		switch (gEngfuncs.pfnRandomLong(0, 1)) {
			case 0: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)MP5_SHOOT_1::sequence, body); break;
			case 1: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)MP5_SHOOT_2::sequence, body); break;
		}

		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -2, 2 ) );
	}

	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

	switch( gEngfuncs.pfnRandomLong( 0, 1 ) ) {
		case 0: gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/hks1.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ) ); break;
		case 1: gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/hks2.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ) ); break;
	}

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_Dynamic_MuzzleFlash(vecSrc, 1.5 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.01);
	if ( gEngfuncs.GetMaxClients() > 1 ) {
		EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_MP5, 2, &tracerCount[idx-1], args->fparam1, args->fparam2 );
	} else {
		EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_MP5, 2, &tracerCount[idx-1], args->fparam1, args->fparam2 );
	}
}
//======================
//		 MP5 END
//======================

//======================
//	    SAW START
//======================
int shell_link_count = 0;
void EV_FireM249(event_args_t *args) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int body;

	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	int shell_link;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	float flSpread = 0.01;

	idx = args->entindex;
	body = args->iparam1;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	AngleVectors(angles, forward, right, up);

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/saw_shell.mdl");// brass shell
	shell_link = gEngfuncs.pEventAPI->EV_FindModelIndex("models/saw_link.mdl");// brass shell link

	if (EV_IsLocal(idx)) {
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		switch (gEngfuncs.pfnRandomLong(0, 2)) {
			case 0: gEngfuncs.pEventAPI->EV_WeaponAnimation((int)SAW_SHOOT1::sequence, body); break;
			case 1:	gEngfuncs.pEventAPI->EV_WeaponAnimation((int)SAW_SHOOT2::sequence, body); break;
			case 2:	gEngfuncs.pEventAPI->EV_WeaponAnimation((int)SAW_SHOOT3::sequence, body); break;
		}

		V_PunchAxis(0, gEngfuncs.pfnRandomFloat(-2, 2 + gEngfuncs.pfnRandomLong(0, 0.3)));
		V_PunchAxis(1, gEngfuncs.pfnRandomFloat(2 + gEngfuncs.pfnRandomLong(0, 0.1), -2));
	}

	EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 12, -14, 5);
	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);

	if (shell_link_count >= 2) {
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 12, -14, 5);
		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell_link, TE_BOUNCE_SHELL);
		shell_link_count = 0;
	}
	shell_link_count++;

	switch (gEngfuncs.pfnRandomLong(0, 2)) {
		case 0: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/saw_fire1.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf)); break;
		case 1: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/saw_fire2.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf)); break;
		case 2: gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/saw_fire3.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf)); break;
	}

	EV_GetGunPosition(args, vecSrc, origin);
	VectorCopy(forward, vecAiming);
	EV_Dynamic_MuzzleFlash(vecSrc, 1.5 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.01);

	if (gEngfuncs.GetMaxClients() > 1)
		EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_556, 2, &tracerCount[idx - 1], args->fparam1, args->fparam2);
	else 	
		EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_556, 2, &tracerCount[idx - 1], args->fparam1, args->fparam2);
}
//======================
//		 SAW END
//======================

//======================
//	  SHOTGUN START
//======================
void EV_FireShotGun( event_args_t *args ) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t vecSpread;
	vec3_t up, right, forward;
	float flSpread = 0.01;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
	AngleVectors( angles, forward, right, up );

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/shotgunshell.mdl");// brass shell

	if ( EV_IsLocal( idx ) ) {
		EV_MuzzleFlash();
		if (!args->iparam2) {
			gEngfuncs.pEventAPI->EV_WeaponAnimation((int)SHOTGUN_SHOOT::sequence, args->iparam1);
			V_PunchAxis(0, -5.0);
		} else {
			gEngfuncs.pEventAPI->EV_WeaponAnimation((int)SHOTGUN_SHOOT_BIG::sequence, args->iparam1);
			V_PunchAxis(0, -10.0);
		}
	}

	if (!args->iparam2) {
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 32, -12, 6);
		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHOTSHELL);
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sbarrel1.wav", gEngfuncs.pfnRandomFloat(0.95, 1.0), ATTN_NORM, 0, 93 + gEngfuncs.pfnRandomLong(0, 0x1f));
	}
	else {
		int j;
		for (j = 0; j < 2; j++) {
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 32, -12, 6);
			EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHOTSHELL);
		}

		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/dbarrel1.wav", gEngfuncs.pfnRandomFloat(0.98, 1.0), ATTN_NORM, 0, 85 + gEngfuncs.pfnRandomLong(0, 0x1f));
	}

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_Dynamic_MuzzleFlash(vecSrc, 1.5 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.01);
	if (!args->iparam2) {
		if (gEngfuncs.GetMaxClients() > 1) {
			EV_HLDM_FireBullets(idx, forward, right, up, 4, vecSrc, vecAiming, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx - 1], 0.08716, 0.04362);
		} else {
			EV_HLDM_FireBullets(idx, forward, right, up, 6, vecSrc, vecAiming, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx - 1], 0.08716, 0.08716);
		}
	} else {
		if (gEngfuncs.GetMaxClients() > 1) {
			EV_HLDM_FireBullets(idx, forward, right, up, 8, vecSrc, vecAiming, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx - 1], 0.17365, 0.04362);
		} else {
			EV_HLDM_FireBullets(idx, forward, right, up, 12, vecSrc, vecAiming, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx - 1], 0.08716, 0.08716);
		}
	}
}
//======================
//	   SHOTGUN END
//======================

//======================
// SPORELAUNCHER START
//======================
void EV_FireSpore(event_args_t *args) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t up, right, forward;

	idx = args->entindex;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	AngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx)) {
		gEngfuncs.pEventAPI->EV_WeaponAnimation((int)SPLAUNCHER_SHOOT::sequence, 0);
		V_PunchAxis(0, -5.0);
	}

	if (args->bparam2)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/splauncher_fire.wav", VOL_NORM, ATTN_NORM, 0, 100);
	else
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/splauncher_altfire.wav", VOL_NORM, ATTN_NORM, 0, 100);

	Vector	vecSpitOffset;
	Vector	vecSpitDir;

	vecSpitDir.x = forward.x;
	vecSpitDir.y = forward.y;
	vecSpitDir.z = forward.z;

	vecSpitOffset = origin;

	vecSpitOffset = vecSpitOffset + forward * 16;
	vecSpitOffset = vecSpitOffset + right * 8;
	vecSpitOffset = vecSpitOffset + up * 4;

	int iSpitModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/tinyspit.spr");
	gEngfuncs.pEfxAPI->R_Sprite_Spray((float*)&vecSpitOffset,(float*)&vecSpitDir,iSpitModelIndex,8,210,25);
}
//======================
//  SPORELAUNCHER END
//======================

//======================
//	   PHYTON START 
//	     ( .357 )
//======================
void EV_FirePython( event_args_t *args ) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;

	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	float flSpread = 0.01;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) ) {
		// Python uses different body in multiplayer versus single player
		int multiplayer = gEngfuncs.GetMaxClients() == 1 ? 0 : 1;

		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation((int)PYTHON_SHOOT::sequence, args->iparam1 );

		V_PunchAxis( 0, -10.0 );
	}

	switch( gEngfuncs.pfnRandomLong( 0, 1 ) ) {
		case 0: gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/357_shot1.wav", gEngfuncs.pfnRandomFloat(0.8, 0.9), ATTN_NORM, 0, PITCH_NORM ); break;
		case 1: gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/357_shot2.wav", gEngfuncs.pfnRandomFloat(0.8, 0.9), ATTN_NORM, 0, PITCH_NORM ); break;
	}

	EV_GetGunPosition( args, vecSrc, origin );
	
	VectorCopy( forward, vecAiming );

	EV_Dynamic_MuzzleFlash(vecSrc, 1.5 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.01);
	EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_357, 0, 0, args->fparam1, args->fparam2 );
}
//======================
//	    PHYTON END 
//	     ( .357 )
//======================

//======================
//	DESERT EAGLE START 
//======================
void EV_FireEagle(event_args_t *args) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;

	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;

	idx = args->entindex;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	empty = args->bparam1;
	AngleVectors(angles, forward, right, up);

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell.mdl");

	if (EV_IsLocal(idx)) {
		cl_entity_s *view = gEngfuncs.GetViewModel();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(empty ? (int)EAGLE_SHOOT_EMPTY::sequence : (int)EAGLE_SHOOT::sequence, args->iparam1);
		EV_MuzzleFlash();
		V_PunchAxis(0, -6.0);
	}

	EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4);
	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL);

	// Play fire sound.
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/desert_eagle_fire.wav", gEngfuncs.pfnRandomFloat(0.8, 0.9), ATTN_NORM, 0, PITCH_NORM);

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	EV_Dynamic_MuzzleFlash(vecSrc, 1.5 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.01);
	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_357, 0, 0, args->fparam1, args->fparam2);
}
//======================
//	 DESERT EAGLE END 
//======================

//======================
//	   GAUSS START 
//======================
#define SND_CHANGE_PITCH	(1<<7)		// duplicated in protocol.h change sound pitch

void EV_SpinGauss( event_args_t *args )
{
	int pitch = args->iparam1;
	int iSoundState = args->bparam1 ? SND_CHANGE_PITCH : 0;

	gEngfuncs.pEventAPI->EV_PlaySound( args->entindex, args->origin, CHAN_WEAPON, "ambience/pulsemachine.wav", VOL_NORM, ATTN_NORM, iSoundState, pitch );
}

/*
==============================
EV_StopPreviousGauss

==============================
*/
void EV_StopPreviousGauss( int idx )
{
	// Make sure we don't have a gauss spin event in the queue for this guy
	gEngfuncs.pEventAPI->EV_KillEvents( idx, "events/gaussspin.sc" );
	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_WEAPON, "ambience/pulsemachine.wav" );
}

void EV_FireGauss( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles, mangles;
	vec3_t velocity;
	float flDamage = args->fparam1;

	int m_fPrimaryFire = args->bparam1;
	int m_iWeaponVolume = PRIMARY_FIRE_VOLUME;
	vec3_t vecSrc, vecDest;
	vec3_t vecMirrorSrc, vecMirrorDest; //env_mirror use this
  
	edict_t		*pentIgnore;
	pmtrace_t tr, mtr, beam_tr;
	float flMaxFrac = 1.0;
	int	nTotal = 0;                             
	int fHasPunched = 0;
	int fFirstBeam = 1;
	int	nMaxHits = 10;
	physent_t *pEntity, *pMEntity;
	int m_iBeam, m_iGlow, m_iBalls;
	vec3_t up, right, forward;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	if ( args->bparam2 )
	{
		EV_StopPreviousGauss( idx );
		return;
	}

//	Con_Printf( "Firing gauss with %f\n", flDamage );
	EV_Dynamic_MuzzleFlash(vecSrc, 1.5 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.01);
	EV_GetGunPosition( args, vecSrc, origin );

	m_iBeam = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/smoke.spr" );
	m_iBalls = m_iGlow = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/hotglow.spr" );
	
	AngleVectors( angles, forward, NULL, NULL );
          
	mangles = EV_MirrorVector(angles);
	AngleVectors( mangles, right, NULL, NULL );
	
	vecMirrorSrc = EV_MirrorPos( vecSrc );
	
	VectorMA( vecSrc, 8192, forward, vecDest );
          VectorMA( vecMirrorSrc, 8192, right, vecMirrorDest );
	
	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -2.0 );
		gEngfuncs.pEventAPI->EV_WeaponAnimation((int)GAUSS_FIRE2::sequence, args->iparam1 );
	}

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/gauss2.wav", 0.5 + flDamage * (1.0 / 400.0), ATTN_NORM, 0, 85 + gEngfuncs.pfnRandomLong( 0, 0x1f ) );

	while (flDamage > 10 && nMaxHits > 0)
	{
		nMaxHits--;

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
		
		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();
	
		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecDest, PM_STUDIO_BOX, -1, &tr );
                    
		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecMirrorSrc, vecMirrorDest, PM_STUDIO_BOX, -1, &mtr );

		gEngfuncs.pEventAPI->EV_PopPMStates();
                    
		if ( tr.allsolid ) break;

		if (fFirstBeam)
		{
			if ( EV_IsLocal( idx ) )
			{
				// Add muzzle flash to current weapon model
				EV_MuzzleFlash();
			}
			fFirstBeam = 0;

			gEngfuncs.pEfxAPI->R_BeamEntPoint( idx | 0x1000,tr.endpos,m_iBeam,0.1,m_fPrimaryFire ? 1.0 : 2.5,0.0,m_fPrimaryFire ? 128.0 : flDamage, 0,0,0,m_fPrimaryFire ? 255 : 255, m_fPrimaryFire ? 128 : 255, m_fPrimaryFire ? 0 : 255 );
			if((vecMirrorDest != vec3_t(0,0,0)) && (vecMirrorSrc != vec3_t(0,0,0)))
				gEngfuncs.pEfxAPI->R_BeamPoints( vecMirrorSrc,mtr.endpos,m_iBeam,0.1,m_fPrimaryFire ? 1.0 : 2.5,0.0,m_fPrimaryFire ? 128.0 : flDamage,0,0,0,m_fPrimaryFire ? 255 : 255,m_fPrimaryFire ? 128 : 255,m_fPrimaryFire ? 0 : 255);
		}
		else
		{
			gEngfuncs.pEfxAPI->R_BeamPoints( vecSrc, tr.endpos, m_iBeam, 0.1, m_fPrimaryFire ? 1.0 : 2.5,0.0,m_fPrimaryFire ? 128.0 : flDamage,0,0,0,m_fPrimaryFire ? 255 : 255,m_fPrimaryFire ? 128 : 255,m_fPrimaryFire ? 0 : 255);
		}

		pEntity = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );
		pMEntity = gEngfuncs.pEventAPI->EV_GetPhysent( mtr.ent );

		if ( pEntity == NULL )
			break;
                    
		if ( pEntity->solid == SOLID_BSP )
		{
			float n;

			pentIgnore = NULL;

			n = -DotProduct( tr.plane.normal, forward );

			if (n < 0.5) // 60 degrees	
			{
				// ALERT( at_console, "reflect %f\n", n );
				// reflect
				vec3_t r;
			
				VectorMA( forward, 2.0 * n, tr.plane.normal, r );

				flMaxFrac = flMaxFrac - tr.fraction;
				
				VectorCopy( r, forward );

				VectorMA( tr.endpos, 8.0, forward, vecSrc );
				VectorMA( vecSrc, 8192.0, forward, vecDest );

				gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 0.2, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage * n / 255.0, flDamage * n * 0.5 * 0.1, FTENT_FADEOUT );
                                        if((vecMirrorDest != vec3_t(0,0,0)) && (vecMirrorSrc != vec3_t(0,0,0))) 
                                                 gEngfuncs.pEfxAPI->R_TempSprite( mtr.endpos, vec3_origin, 0.2, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage * n / 255.0, flDamage * n * 0.5 * 0.1, FTENT_FADEOUT );
                                        
				vec3_t fwd;
				VectorAdd( tr.endpos, tr.plane.normal, fwd );

				gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd, m_iBalls, 3, 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 100, 255, 100 );
                                        
				// lose energy
				if ( n == 0 )
				{
					n = 0.1;
				}
				
				flDamage = flDamage * (1 - n);

			}
			else
			{
				// tunnel
				EV_HLDM_DecalGunshot( &tr, BULLET_MONSTER_12MM, NULL, NULL);

				gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 1.0, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage / 255.0, 6.0, FTENT_FADEOUT );
                                        if((vecMirrorDest != vec3_t(0,0,0)) && (vecMirrorSrc != vec3_t(0,0,0)))
                                        	gEngfuncs.pEfxAPI->R_TempSprite( mtr.endpos, vec3_origin, 1.0, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage / 255.0, 6.0, FTENT_FADEOUT );

				// limit it to one hole punch
				if (fHasPunched) break;
				fHasPunched = 1;
				
				// try punching through wall if secondary attack (primary is incapable of breaking through)
				if ( !m_fPrimaryFire )
				{
					vec3_t start;
					
					VectorMA( tr.endpos, 8.0, forward, start );
					
					// Store off the old count
					gEngfuncs.pEventAPI->EV_PushPMStates();
						
					// Now add in all of the players.
					gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );

					gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
					gEngfuncs.pEventAPI->EV_PlayerTrace( start, vecDest, PM_STUDIO_BOX, -1, &beam_tr );

					if ( !beam_tr.allsolid )
					{
						vec3_t delta;
						float n;

						// trace backwards to find exit point

						gEngfuncs.pEventAPI->EV_PlayerTrace( beam_tr.endpos, tr.endpos, PM_STUDIO_BOX, -1, &beam_tr );

						VectorSubtract( beam_tr.endpos, tr.endpos, delta );
						
						n = Length( delta );

						if (n < flDamage)
						{
							if (n == 0)
								n = 1;
							flDamage -= n;

							// absorption balls
							{
								vec3_t fwd;
								VectorSubtract( tr.endpos, forward, fwd );
								gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd, m_iBalls, 3, 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 100, 255, 100 );
							}

							EV_HLDM_DecalGunshot( &beam_tr, BULLET_MONSTER_12MM, NULL, NULL);
							
							gEngfuncs.pEfxAPI->R_TempSprite( beam_tr.endpos, vec3_origin, 0.1, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage / 255.0, 6.0, FTENT_FADEOUT );
			
							// balls
							{
								vec3_t fwd;
								VectorSubtract( beam_tr.endpos, forward, fwd );
								gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, beam_tr.endpos, fwd, m_iBalls, (int)(flDamage * 0.3), 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 200, 255, 40 );
							}
							
							VectorAdd( beam_tr.endpos, forward, vecSrc );
						}
					}
					else
					{
						flDamage = 0;
					}

					gEngfuncs.pEventAPI->EV_PopPMStates();
				}
				else
				{
					if ( m_fPrimaryFire )
					{
						// slug doesn't punch through ever with primary 
						// fire, so leave a little glowy bit and make some balls
						gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 0.2, m_iGlow, kRenderGlow, kRenderFxNoDissipation, 200.0 / 255.0, 0.3, FTENT_FADEOUT );
			                              if((vecMirrorDest != vec3_t(0,0,0)) && (vecMirrorSrc != vec3_t(0,0,0))) 
			                              	gEngfuncs.pEfxAPI->R_TempSprite( mtr.endpos, vec3_origin, 0.2, m_iGlow, kRenderGlow, kRenderFxNoDissipation, 200.0 / 255.0, 0.3, FTENT_FADEOUT );
						
						// balls
						{
							vec3_t fwd;
							int num_balls = gEngfuncs.pfnRandomFloat( 10, 20 );
							VectorAdd( tr.endpos, tr.plane.normal, fwd );
							gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd, m_iBalls, 8, 0.6, num_balls / 100.0, 100, 255, 200 );
							VectorAdd( mtr.endpos, mtr.plane.normal, fwd );
							if((vecMirrorDest != vec3_t(0,0,0)) && (vecMirrorSrc != vec3_t(0,0,0)))
								gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, mtr.endpos, fwd, m_iBalls, 8, 0.6, num_balls / 100.0, 100, 255, 200 );
						}
					}

					flDamage = 0;
				}
			}
		}
		else
		{
			VectorAdd( tr.endpos, forward, vecSrc );
		}
	}
}
//======================
//	   GAUSS END 
//======================

//======================
//	    EGON START 
//======================

enum EGON_FIRESTATE { FIRE_OFF = 0, FIRE_CHARGE };
enum EGON_FIREMODE { FIRE_NARROW = 0, FIRE_WIDE };

#define EGON_PRIMARY_VOLUME		450
#define EGON_BEAM_SPRITE		"sprites/xbeam1.spr"
#define EGON_FLARE_SPRITE		"sprites/XSpark1.spr"
#define EGON_SOUND_OFF			"weapons/egon_off1.wav"
#define EGON_SOUND_RUN			"weapons/egon_run3.wav"
#define EGON_SOUND_STARTUP		"weapons/egon_windup2.wav"

BEAM *pBeam;
BEAM *pBeam2;

BEAM *pMirBeam;
BEAM *pMirBeam2;
TEMPENTITY *EndFlare;
TEMPENTITY *EndMirFlare;

//UNDONE : mirror beams don't update, end sprite don't drawing
bool b_mir = 0;

void EV_EgonFire( event_args_t *args ) {
	int idx, iFireState, iFireMode;
	vec3_t origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	iFireMode = args->iparam2;
	int iStartup = args->bparam1;
	iFireState = args->bparam2;
	int m_iFlare = gEngfuncs.pEventAPI->EV_FindModelIndex( EGON_FLARE_SPRITE );

	if ( iStartup ) {
		if ( iFireMode == FIRE_WIDE )
			gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, EGON_SOUND_STARTUP, 0.98, ATTN_NORM, 0, 125 );
		else	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, EGON_SOUND_STARTUP, 0.9, ATTN_NORM, 0, 100 );
	}  else {
		if ( iFireMode == FIRE_WIDE )
			gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_STATIC, EGON_SOUND_RUN, 0.98, ATTN_NORM, 0, 125 );
		else	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_STATIC, EGON_SOUND_RUN, 0.9, ATTN_NORM, 0, 100 );
	}

	if ( iStartup == 1 && EV_IsLocal( idx ) && !pBeam && !pBeam2 && cl_lw->value )
	{
		vec3_t vecSrc, vecEnd, origin, angles, forward, right, mangles;
		vec3_t vecMirrorSrc, vecMirrorDest; //env_mirror use this
		pmtrace_t tr, mtr;

		cl_entity_t *pl = gEngfuncs.GetEntityByIndex( idx );

		if ( pl )
		{
			VectorCopy( gHUD.m_vecAngles, angles );
			AngleVectors( angles, forward, NULL, NULL );
                              EV_GetGunPosition( args, vecSrc, pl->origin );
                              
			mangles = EV_MirrorVector(angles);
			AngleVectors( mangles, right, NULL, NULL );
			vecMirrorSrc = EV_MirrorPos( vecSrc );
			
			VectorMA( vecSrc, 2048, forward, vecEnd );
			VectorMA( vecMirrorSrc, 2048, right, vecMirrorDest );
			gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );	
				
			// Store off the old count
			gEngfuncs.pEventAPI->EV_PushPMStates();
			// Now add in all of the players.
			gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

			gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
			gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );
                              //mirror trace
			gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
			gEngfuncs.pEventAPI->EV_PlayerTrace( vecMirrorSrc, vecMirrorDest, PM_STUDIO_BOX, -1, &mtr );

			gEngfuncs.pEventAPI->EV_PopPMStates();

			int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex( EGON_BEAM_SPRITE );

			float r = 50.0f;
			float g = 50.0f;
			float b = 125.0f;

			if ( IEngineStudio.IsHardware() )
			{
				r /= 100.0f;
				g /= 100.0f;
			}
				
		
			if ( iFireMode == FIRE_WIDE )
			{
				pBeam = gEngfuncs.pEfxAPI->R_BeamEntPoint ( idx | 0x1000, tr.endpos, iBeamModelIndex, 99999, 3.5, 0.2, 0.7, 55, 0, 0, r, g, b );
				if ( pBeam )pBeam->flags |= ( FBEAM_SINENOISE );
				pBeam2 = gEngfuncs.pEfxAPI->R_BeamEntPoint ( idx | 0x1000, tr.endpos, iBeamModelIndex, 99999, 5.0, 0.08, 0.7, 25, 0, 0, r, g, b );
				if((vecMirrorDest != vec3_t(0,0,0)) && (vecMirrorSrc != vec3_t(0,0,0)) && b_mir)
				{
					pMirBeam = gEngfuncs.pEfxAPI->R_BeamPoints ( vecMirrorSrc, mtr.endpos, iBeamModelIndex, 99999, 3.5, 0.2, 0.7, 55, 0, 0, r, g, b );
					if ( pMirBeam )pBeam->flags |= ( FBEAM_SINENOISE );
					pMirBeam2 = gEngfuncs.pEfxAPI->R_BeamPoints ( vecMirrorSrc, mtr.endpos, iBeamModelIndex, 99999, 5.0, 0.08, 0.7, 25, 0, 0, r, g, b );
				}
			}
			else
			{
				pBeam = gEngfuncs.pEfxAPI->R_BeamEntPoint ( idx | 0x1000, tr.endpos, iBeamModelIndex, 99999, 1.5, 0.1, 0.7, 55, 0, 0, r, g, b );
				if ( pBeam )pBeam->flags |= ( FBEAM_SINENOISE );
				pBeam2 = gEngfuncs.pEfxAPI->R_BeamEntPoint ( idx | 0x1000, tr.endpos, iBeamModelIndex, 99999, 5.0, 0.08, 0.7, 25, 0, 0, r, g, b );
				if((vecMirrorDest != vec3_t(0,0,0)) && (vecMirrorSrc != vec3_t(0,0,0)) && b_mir)
				{
					pMirBeam = gEngfuncs.pEfxAPI->R_BeamPoints ( vecMirrorSrc, mtr.endpos, iBeamModelIndex, 99999, 1.5, 0.1, 0.7, 55, 0, 0, r, g, b );
					if ( pMirBeam )pBeam->flags |= ( FBEAM_SINENOISE );
					pMirBeam2 = gEngfuncs.pEfxAPI->R_BeamPoints ( vecMirrorSrc, mtr.endpos, iBeamModelIndex, 99999, 5.0, 0.08, 0.7, 25, 0, 0, r, g, b );
				}
			}

            EndFlare = gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 1.0, m_iFlare, kRenderGlow, kRenderFxNoDissipation, 255, 9999, FTENT_SPRANIMATE );
			if((vecMirrorDest != vec3_t(0,0,0)) && (vecMirrorSrc != vec3_t(0,0,0)) && b_mir)
				EndMirFlare = gEngfuncs.pEfxAPI->R_TempSprite( mtr.endpos, vec3_origin, 1.0, m_iFlare, kRenderGlow, kRenderFxNoDissipation, 255, 9999, FTENT_SPRANIMATE );
		}
	}
}

void EV_EgonStop( event_args_t *args )
{
	int idx;
	vec3_t origin;

	idx = args->entindex;
	VectorCopy ( args->origin, origin );

	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, EGON_SOUND_RUN );
	
	if ( args->iparam1 )
		 gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, EGON_SOUND_OFF, 0.98, ATTN_NORM, 0, 100 );

	if ( EV_IsLocal( idx ) ) 
	{
		if ( pBeam )
		{
			pBeam->die = 0.0;
			pBeam = NULL;
		}
		if ( pBeam2 )
		{
			pBeam2->die = 0.0;
			pBeam2 = NULL;
		}
		if ( pMirBeam )
		{
			pMirBeam->die = 0.0;
			pMirBeam = NULL;
		}
		if ( pMirBeam2 )
		{
			pMirBeam2->die = 0.0;
			pMirBeam2 = NULL;
		}
		if( EndFlare )
		{
			EndFlare->die = 0.0;
			EndFlare = NULL;
		}
		if( EndMirFlare )
		{
			EndMirFlare->die = 0.0;
			EndMirFlare = NULL;
		}
		gEngfuncs.pEventAPI->EV_WeaponAnimation((int)EGON_FIRESTOP::sequence, args->iparam2 );
	}
}

void EV_UpdateBeams ( void )
{
	if ( !pBeam && !pBeam2 ) return;
	
	vec3_t forward, vecSrc, vecEnd, origin, angles, right, mangles;
	vec3_t vecMirrorSrc, vecMirrorDest; //env_mirror use this
	pmtrace_t tr, mtr;

	cl_entity_t *pthisplayer = gEngfuncs.GetLocalPlayer();
	int idx = pthisplayer->index;
	
	// Get our exact viewangles from engine
	gEngfuncs.GetViewAngles( (float *)angles );

	VectorCopy(previousorigin, origin);

	AngleVectors( angles, forward, NULL, NULL );
	VectorCopy( origin, vecSrc );
	VectorMA( vecSrc, 2048, forward, vecEnd );

	mangles = EV_MirrorVector(angles);
	AngleVectors( mangles, right, NULL, NULL );
	vecMirrorSrc = EV_MirrorPos( vecSrc );
   VectorMA( vecMirrorSrc, 2048, right, vecMirrorDest );
	
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );	
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

 	//mirror trace
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecMirrorSrc, vecMirrorDest, PM_STUDIO_BOX, -1, &mtr );

	gEngfuncs.pEventAPI->EV_PopPMStates();

	if ( pBeam )
	{
		pBeam->target = tr.endpos;
		pBeam->die = gEngfuncs.GetClientTime() + 0.1; // We keep it alive just a little bit forward in the future, just in case.
	}
	if ( pBeam2 )
	{
		pBeam2->target = tr.endpos;
		pBeam2->die = gEngfuncs.GetClientTime() + 0.1; // We keep it alive just a little bit forward in the future, just in case.
	}
	if ( pMirBeam )
	{
		pMirBeam->target = mtr.endpos;
		pMirBeam->die = gEngfuncs.GetClientTime() + 0.1; // We keep it alive just a little bit forward in the future, just in case.
	}
	if ( pMirBeam2 )
	{
		pMirBeam2->target = mtr.endpos;
		pMirBeam2->die = gEngfuncs.GetClientTime() + 0.1; // We keep it alive just a little bit forward in the future, just in case.
	}
	if( EndFlare )
	{
		EndFlare->tentOffset = tr.endpos;
		EndFlare->die = gEngfuncs.GetClientTime() + 0.1;
	}
	if( EndMirFlare )
	{
		EndMirFlare->tentOffset = mtr.endpos;
		EndMirFlare->die = gEngfuncs.GetClientTime() + 0.1;
	}
}
//======================
//	    EGON END 
//======================

//======================
//	   SQUEAK START
//======================
enum squeak_e {
	SQUEAK_IDLE1 = 0,
	SQUEAK_FIDGETFIT,
	SQUEAK_FIDGETNIP,
	SQUEAK_DOWN,
	SQUEAK_UP,
	SQUEAK_THROW
};

void EV_SnarkFire(event_args_t *args) {
	int idx;
	vec3_t vecSrc, angles, view_ofs, forward;
	pmtrace_t tr;

	idx = args->entindex;
	VectorCopy(args->origin, vecSrc);
	VectorCopy(args->angles, angles);

	AngleVectors(angles, forward, NULL, NULL);

	if (!EV_IsLocal(idx))
		return;

	if (args->ducking)
		vecSrc = vecSrc - (VEC_HULL_MIN - VEC_DUCK_HULL_MIN);

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc + forward * 20, vecSrc + forward * 64, PM_NORMAL, -1, &tr);

	//Find space to drop the thing.
	if (tr.allsolid == 0 && tr.startsolid == 0 && tr.fraction > 0.25)
		gEngfuncs.pEventAPI->EV_WeaponAnimation(SQUEAK_THROW, args->iparam1);

	gEngfuncs.pEventAPI->EV_PopPMStates();
}
//======================
//	   SQUEAK END
//======================

//======================
//	 SHOCKRIFLE START
//======================
void EV_ShockFire(event_args_t *args) {
	int idx;
	vec3_t origin;
	idx = args->entindex;
	VectorCopy(args->origin, origin);

	// Primary attack.
	if (!(int)args->iparam1) {
		//Only play the weapon anims if I shot it.
		if (EV_IsLocal(idx)) {
			V_PunchAxis(0, gEngfuncs.pfnRandomLong(0, 2));
			gEngfuncs.pEventAPI->EV_WeaponAnimation((int)SHOCKRIFLE_SHOOT::sequence, 1);
		}

		// Play fire sound.
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/shock_fire.wav", 1, ATTN_NORM, 0, 100);
	}
	else // Play weapon effects.
	{
		int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/lgtning.spr");
		cl_entity_t * vm = gEngfuncs.GetViewModel();
		if (vm)
		{
			// Valid viewmodel.

			gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);

			// Store off the old count
			gEngfuncs.pEventAPI->EV_PushPMStates();

			for (int i = 1; i < 4; i++) {
				BEAM* pBeam = gEngfuncs.pEfxAPI->R_BeamPoints(
					(float*)&vm->attachment[0],
					(float*)&vm->attachment[i],
					iBeamModelIndex,
					0.01f,
					1.1f,
					0.3f,
					230 + gEngfuncs.pfnRandomFloat(20, 30),
					10,
					0,
					10,
					0.0f,
					1.0f,
					1.0f);

				if (pBeam) {
					pBeam->flags |= (FBEAM_SHADEIN | FBEAM_SHADEOUT);
					pBeam->startEntity = vm->index;
					pBeam->endEntity = vm->index;
				}
			}

			EV_Dynamic_MuzzleFlash(args->origin, 1.5 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 0, 255, 255, 0.01);
			gEngfuncs.pEventAPI->EV_PopPMStates();
		}
	}
}
//======================
//   SHOCKRIFLE END
//======================

//======================
//  SNIPERRIFLE START 
//======================
void EV_FireSniper(event_args_t *args) {
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;

	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;

	idx = args->entindex;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	empty = args->bparam1;
	AngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx)) {
		cl_entity_s *view = gEngfuncs.GetViewModel();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam1 == 1 ? (int)M40A1_FIRELASTROUND::sequence : (int)M40A1_FIRE::sequence, 0);
		EV_MuzzleFlash();

		gEngfuncs.pEfxAPI->R_MuzzleFlash((float *)&view->attachment[0], 11);
		V_PunchAxis(0, -5.0);
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sniper_fire.wav", gEngfuncs.pfnRandomFloat(0.92, 1.0), ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	// Play fire sound.
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sniper_fire.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);

	EV_Dynamic_MuzzleFlash(vecSrc, 1.5 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.02);
	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_762, 0, 0, args->fparam1, args->fparam2);
}
//======================
//	 SNIPERRIFLE END 
//======================

//======================
//	 DISPLACER START 
//======================
void EV_Displacer(event_args_t *args) {
	int idx;
	vec3_t origin;
	idx = args->entindex;
	VectorCopy(args->origin, origin);

	if (EV_IsLocal(idx)) {
		// Used to play weapon animations.
		// Fire state
		switch (args->iparam1) {
			case (int)DISPLACER_SPINUP::sequence:
				if (EV_IsLocal(args->entindex)) {
					gEngfuncs.pEventAPI->EV_WeaponAnimation((int)DISPLACER_SPINUP::sequence, 2);
					cl_entity_t *view = gEngfuncs.GetViewModel();
					if (view != NULL) {
						float life = 1.14;
						//	int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/plasma.spr");
						//	gEngfuncs.pEfxAPI->R_BeamEnts(view->index | 0x2000, view->index | 0x1000, iBeamModelIndex, life, 0.5, 0.5, 0.5, 0.6, 0, 10, 2, 10, 0);
						//	gEngfuncs.pEfxAPI->R_BeamEnts(view->index | 0x3000, view->index | 0x1000, iBeamModelIndex, life, 0.5, 0.5, 0.5, 0.6, 0, 10, 2, 10, 0);
						//	gEngfuncs.pEfxAPI->R_BeamEnts(view->index | 0x4000, view->index | 0x1000, iBeamModelIndex, life, 0.5, 0.5, 0.5, 0.6, 0, 10, 2, 10, 0);
					}
				}
			break;
			case (int)DISPLACER_SPIN::sequence:
				gEngfuncs.pEventAPI->EV_WeaponAnimation((int)DISPLACER_SPIN::sequence, 0);
			break;
			case (int)DISPLACER_FIRE::sequence:
				gEngfuncs.pEventAPI->EV_WeaponAnimation((int)DISPLACER_FIRE::sequence, 0);
			break;
		}
	}

	// Used to play weapon sounds.
	// Fire state.
	switch (args->iparam1) {
		case (int)DISPLACER_SPINUP::sequence:
			// Fire mode
			switch (args->iparam2) {
				case 1: // FIRESTATE_FORWARD (primary attack)
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/displacer_spin.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				break;
				case 2: // FIRESTATE_BACKWARD (secondary attack)
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/displacer_spin2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				break;
			}
		break;
		case (int)DISPLACER_FIRE::sequence:
			// Fire mode
			switch (args->iparam2) {
				case 1: // FIRESTATE_FORWARD (primary attack)
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/displacer_fire.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				break;
				case 2: // FIRESTATE_BACKWARD (secondary attack)
					if (!args->bparam1)
						gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/displacer_self.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				break;
			}
		break;
	}
}
//======================
//	    DISPLACER END 
//======================

//======================
//	   DECALS START
//======================
void EV_Decals( struct event_args_s *args )
{
	int idx;
	pmtrace_t tr;
	pmtrace_t *pTrace = &tr;
    physent_t *pe;
          
	idx = args->entindex;
	VectorCopy( args->origin, pTrace->endpos );
	pTrace->ent = args->iparam1; 
	pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );
	
	if(args->iparam2 == 0)//explode decals
	{
		switch( gEngfuncs.pfnRandomLong( 0, 2 ) ) {
			case 0: EV_HLDM_CrowbarDecalTrace( pTrace, "{scorch1" ); break;
			case 1: EV_HLDM_CrowbarDecalTrace( pTrace, "{scorch2" ); break;
			case 2: EV_HLDM_CrowbarDecalTrace( pTrace, "{scorch3" ); break;
		}
	}
	else if(args->iparam2 == 1)//red blood decals
	{
		switch( gEngfuncs.pfnRandomLong( 0, 7 ) ) {
			case 0: EV_HLDM_CrowbarDecalTrace( pTrace, "{blood1" ); break;
			case 1: EV_HLDM_CrowbarDecalTrace( pTrace, "{blood2" ); break;
			case 2: EV_HLDM_CrowbarDecalTrace( pTrace, "{blood3" ); break;
			case 3: EV_HLDM_CrowbarDecalTrace( pTrace, "{blood4" ); break;
			case 4: EV_HLDM_CrowbarDecalTrace( pTrace, "{blood5" ); break;
			case 5: EV_HLDM_CrowbarDecalTrace( pTrace, "{blood6" ); break;
			case 6: EV_HLDM_CrowbarDecalTrace( pTrace, "{blood7" ); break;
			case 7: EV_HLDM_CrowbarDecalTrace( pTrace, "{blood8" ); break;
		}
	}
	else if(args->iparam2 == 2)//yellow blood decals
	{
		switch( gEngfuncs.pfnRandomLong( 0, 5 ) ) {
			case 0: EV_HLDM_CrowbarDecalTrace( pTrace, "{yblood1" ); break;
			case 1: EV_HLDM_CrowbarDecalTrace( pTrace, "{yblood2" ); break;
			case 2: EV_HLDM_CrowbarDecalTrace( pTrace, "{yblood3" ); break;
			case 3: EV_HLDM_CrowbarDecalTrace( pTrace, "{yblood4" ); break;
			case 4: EV_HLDM_CrowbarDecalTrace( pTrace, "{yblood5" ); break;
			case 5: EV_HLDM_CrowbarDecalTrace( pTrace, "{yblood6" ); break;
		}
	}
	else if(args->iparam2 == 4)//bulsquid decals
	{
		switch( gEngfuncs.pfnRandomLong( 0, 1 ) ) {
			case 0: EV_HLDM_CrowbarDecalTrace( pTrace, "{spit1" ); break;
			case 1: EV_HLDM_CrowbarDecalTrace( pTrace, "{spit2" ); break;
		}
	}
	else if(args->iparam2 == 5)//garg flames
	{
		switch( gEngfuncs.pfnRandomLong( 0, 2 ) ) {
			case 0: EV_HLDM_CrowbarDecalTrace( pTrace, "{smscorch1" ); break;
			case 1: EV_HLDM_CrowbarDecalTrace( pTrace, "{smscorch2" ); break;
			case 2: EV_HLDM_CrowbarDecalTrace( pTrace, "{smscorch3" ); break;
		}
	}
	else if(args->iparam2 == 6)//monsters shoot
	{
        if ( pe && pe->solid == SOLID_BSP )
			EV_HLDM_GunshotDecalTrace( pTrace, EV_HLDM_DamageDecal(pTrace, pe, null, null));
	}
}
//======================
//	   DECALS END
//======================

//======================
//     EFX START
//======================
void EV_Explode( struct event_args_s *args ) {
	int m_iExplodeSprite;
	vec3_t mirpos = EV_MirrorPos(args->origin); 	

	if(args->bparam1)
		m_iExplodeSprite = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/WXplo1.spr" );
	else {
		switch (gEngfuncs.pfnRandomLong(0, 3)) {
			case 0: m_iExplodeSprite = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/explosion_0.spr"); break;
			case 1: m_iExplodeSprite = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/explosion_1.spr"); break;
			case 2: m_iExplodeSprite = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/explosion_2.spr"); break;
			case 3: m_iExplodeSprite = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/explosion_3.spr"); break;
		}
	}

	EV_Dynamic_MuzzleFlash(args->origin, (args->fparam1 - 50) * 0.06 + gEngfuncs.pfnRandomFloat(-0.2, 0.2), 255, 255, 128, 0.3);
	gEngfuncs.pEfxAPI->R_Explosion( args->origin, m_iExplodeSprite, (args->fparam1 - 50) * 0.06, 15, TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES );
	
	if(mirpos != vec3_t(0,0,0))
		gEngfuncs.pEfxAPI->R_Explosion( mirpos, m_iExplodeSprite, (args->fparam1 - 50) * 0.06, 15, TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NOPARTICLES );
}
//======================
//           EFX END
//======================

//======================
//	   NULL START
//======================
void EV_FireNull(event_args_t *args) {
	CONPRINT("Fire Null!\n");
}

//======================
//	   NULL END
//======================
void EV_TrainPitchAdjust( event_args_t *args ) {
	int idx;
	vec3_t origin;

	unsigned short us_params;
	int noise;
	float m_flVolume;
	int pitch;
	int stop;
	
	char sz[ 256 ];

	idx = args->entindex;
	
	VectorCopy( args->origin, origin );

	us_params = (unsigned short)args->iparam1;
	stop	  = args->bparam1;

	m_flVolume	= (float)(us_params & 0x003f)/40.0;
	noise		= (int)(((us_params) >> 12 ) & 0x0007);
	pitch		= (int)( 10.0 * (float)( ( us_params >> 6 ) & 0x003f ) );

	switch ( noise ) {
		case 1: strcpy( sz, "plats/ttrain1.wav"); break;
		case 2: strcpy( sz, "plats/ttrain2.wav"); break;
		case 3: strcpy( sz, "plats/ttrain3.wav"); break; 
		case 4: strcpy( sz, "plats/ttrain4.wav"); break;
		case 5: strcpy( sz, "plats/ttrain6.wav"); break;
		case 6: strcpy( sz, "plats/ttrain7.wav"); break;
		default:
			// no sound
			strcpy( sz, "" );
		return;
	}

	if ( stop ) {
		gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, sz );
	} else {
		gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_STATIC, sz, m_flVolume, ATTN_NORM, SND_CHANGE_PITCH, pitch );
	}
}

/*
=====================
HUD_PostRunCmd

Client calls this during prediction, after it has moved the player and updated any info changed into to->
time is the current client clock based on prediction
cmd is the command that caused the movement, etc
runfuncs is 1 if this is the first time we've predicted this command.  If so, sounds and effects should play, otherwise, they should
be ignored
=====================
*/
void CL_DLLEXPORT HUD_PostRunCmd( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed )
{
	// Offset final origin by view_offset
	if(cl_lw->value)
	{
		for (int i = 0; i < 3; i++ ) previousorigin[i] = to->playerstate.origin[i] + to->client.view_ofs[ i ];
		to = NULL;
	}
}