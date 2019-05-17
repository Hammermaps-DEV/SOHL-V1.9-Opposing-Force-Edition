/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
*	This product contains software technology licensed from:
*	The Battle Grounds Team and Contributors.
*
****/
#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER

// needed for all list work
#include <algorithm>
#include <vector>

using std::vector;

// lets do some forward declaring so we don't get into too much trouble
class CParticle;
class CParticleSystem;
class CParticleSystemManager;

// now the real headers
#include "particle_texture.h"
#include "particle_defs.h"
#include "system_manager.h"
#include "particle_systems.h"
#include "particle.h"

// our particle related externs

// percentage of particles we want
extern cvar_t* g_ParticleCount;
// are we debugging the particle system
extern cvar_t* g_ParticleDebug;
// how many sorts we are going to have a second
extern cvar_t* g_ParticleSorts;
// this player's position from pm_shared
extern Vector flPlayerOrigin;
// sets up ogl for all particles
extern inline void Particle_InitOpenGL( void );
// "closes" opengl again when drawing is done so we don't screw up settings for HL
extern inline void Particle_FinishOpenGL( void );
// loads a tga to a pointer
extern particle_texture_s * LoadTGA(particle_texture_s *pTexture, char* filename);
// this is the pointer that all particle systems add themselves to.
// it must be the only one as it deletes the particle systems
// and we don't want dangling pointers
extern CParticleSystemManager *pParticleManager;

#endif
