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
*	This product contains software technology licensed from:
*	The Battle Grounds Team and Contributors.
*
*   Modifications by Hammermaps.de DEV Team (support@hammermaps.de).
*
***/

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>

#include "particle_header.h"
#include "event_api.h"
#include "r_efx.h"
#include "pm_shared.h"

CParticleSystemManager *pParticleManager = NULL;
cvar_t* g_ParticleCount;
cvar_t* g_ParticleDebug;
cvar_t* g_ParticleSorts;

// updates all systems
void CParticleSystemManager::UpdateSystems( void )
{
	CParticleSystem *pSystem = NULL;
	signed int i = 0;
	signed int iSystems = (signed)m_pParticleSystems.size();
	// iterate through all the particle systems, drawing each
	for (; i < iSystems; i++)
	{
		pSystem = m_pParticleSystems[i];
		// remove the system if the system requests it
		if( pSystem && pSystem->DrawSystem() == false)
		{
			delete pSystem;
			pSystem = NULL;
			m_pParticleSystems.erase((m_pParticleSystems.begin() + i));
			i--;
			iSystems--;
		}
	}

	// we couldn't return earlier as we need to have the sorting before the ps updating
	// however no sorting when we can't see the particles
	if(CheckDrawSystem() == false)
		return;

	// prepare opengl
	Particle_InitOpenGL();

	// declated variables we need for both unsorted and sorted
	int iParticles = m_pUnsortedParticles.size();

	float 	flTimeSinceLastDraw = TimeSinceLastDraw();

	int iDrawn = 0;

	// draw all unsorted particles first, so they are at the back of the screen.
	if(iParticles > 0) {
		// loop through all particles drawing them
		CParticle *pParticle = NULL;
		for(i = 0; i < iParticles ; i++) {
			if(m_pUnsortedParticles[i]) {
				pParticle = m_pUnsortedParticles[i];
				if(pParticle && pParticle->Test()) {
					pParticle->Update(flTimeSinceLastDraw);

					// don't draw in certain spec modes
					if(g_iUser1 != OBS_MAP_FREE && g_iUser1 != OBS_MAP_CHASE) {
						// unfortunately we have to prepare every particle now
						// as we can't prepare for a batch of the same type anymore
						pParticle->Prepare(); 
						pParticle->Draw();
						iDrawn++;
					}
				// particle wants to die, so kill it
				} else {
					RemoveParticle(pParticle);
					i--;
					iParticles--;
				}
			}
		}
	}

	iParticles = m_pParticles.size();

	// sort and draw the sorted particles list
	if(iParticles > 0) {
		// calculate the fraction of a second between sorts
		float flTimeSinceLastSort = (gEngfuncs.GetClientTime() - m_flLastSort);
		// 1 / time between sorts will give us a number like 5
		// if it is less than the particlesorts cvar then it is a small value 
		// and therefore a long time since last sort
		if((((int)(1 / flTimeSinceLastSort)) < g_ParticleSorts->value)) {
			m_flLastSort = gEngfuncs.GetClientTime();
			std::sort(m_pParticles.begin(), m_pParticles.end(), less_than);
		}

		// loop through all particles drawing them
		CParticle *pParticle = NULL;
		for(i = 0; i < iParticles ; i++) {
			if(m_pParticles[i]) {
				pParticle = m_pParticles[i];
				if(pParticle && pParticle->Test()) {
					pParticle->Update(flTimeSinceLastDraw);

					// don't draw in certain spec modes
					if(g_iUser1 != OBS_MAP_FREE && g_iUser1 != OBS_MAP_CHASE) {
						// unfortunately we have to prepare every particle now
						// as we can't prepare for a batch of the same type anymore
						pParticle->Prepare(); 
						pParticle->Draw();
						iDrawn++;
					}
				// particle wants to die, so kill it
				} else {
					RemoveParticle(pParticle);
					i--;
					iParticles--;
				}
			}
		}
	}

	// finished particle drawing
	Particle_FinishOpenGL();

	// print out how fast we've been drawing the systems in debug mode
	if (g_ParticleDebug->value != 0 && ((m_flLastDebug + 1) <= gEngfuncs.GetClientTime()))
	{
		CONPRINT("%i Particles Drawn this pass in %i systems %i Textures in Cache\n\0", iDrawn, m_pParticleSystems.size(), m_pTextures.size());
		m_flLastDebug = gEngfuncs.GetClientTime();
	}

	m_flLastDraw = gEngfuncs.GetClientTime();
}

// Handles all the present particle systems
void CParticleSystemManager::CreatePresetPS(unsigned int iPreset, particle_system_management *pSystem)
{
	switch(iPreset)
	{
		case iImpactBloodRed:
			CreateMappedPS("particles/gore/blood_red_animated.txt", pSystem);	// Blood impact animated
			CreateMappedPS("particles/gore/blood_red_impact.txt", pSystem);		// Blood impact-mist
			CreateMappedPS("particles/gore/blood_red_drips.txt", pSystem);		// Blood drips
		break;

		case iImpactBloodRedLOW:
			//CreateMappedPS("particles/gore/blood_red_animated.txt", pSystem);	// Blood impact animated
			//CreateMappedPS("particles/gore/blood_red_impact.txt", pSystem);		// Blood impact-mist
		break;

		case iImpactBloodYellow:
			//CreateMappedPS("particles/gore/blood_yellow_animated.txt", pSystem);	// Blood impact animated
			//CreateMappedPS("particles/gore/blood_yellow_impact.txt", pSystem);		// Blood impact-mist
			//CreateMappedPS("particles/gore/blood_yellow_drips.txt", pSystem);		// Blood drips
		break;

		case iImpactBloodYellowLOW:
			//CreateMappedP	S("particles/gore/blood_yellow_animated.txt", pSystem);	// Blood impact animated
			//CreateMappedPS("particles/gore/blood_yellow_impact.txt", pSystem);		// Blood impact-mist
		break;

		case iImpactWater:
			CreateMappedPS("particles/water/water_impact_core.txt", pSystem);
			CreateMappedPS("particles/water/water_impact_drops.txt", pSystem);
			CreateMappedPS("particles/water/water_impact_wave.txt", pSystem);

			// play a sound as well
			switch (gEngfuncs.pfnRandomLong(0, 2)) {
				case 0:gEngfuncs.pEventAPI->EV_PlaySound(0, pSystem->vPosition, 0, "debris/water_impact_bullet1.wav", 1.0, ATTN_NONE, 0, PITCH_NORM); break;
				case 1:gEngfuncs.pEventAPI->EV_PlaySound(0, pSystem->vPosition, 0, "debris/water_impact_bullet2.wav", 1.0, ATTN_NONE, 0, PITCH_NORM); break;
				case 2:gEngfuncs.pEventAPI->EV_PlaySound(0, pSystem->vPosition, 0, "debris/water_impact_bullet3.wav", 1.0, ATTN_NONE, 0, PITCH_NORM); break;
			}
		break;
		case iExplosionDefault: {
			CreateMappedPS("particles/explosions/explo1_darksmoke.txt", pSystem);
			CreateMappedPS("particles/explosions/explo1_fire.txt", pSystem);
			CreateMappedPS("particles/explosions/explo1_firedust.txt", pSystem);
			CreateMappedPS("particles/explosions/explo1_glow.txt", pSystem);
			CreateMappedPS("particles/explosions/explo1_ground_wave.txt", pSystem);
			CreateMappedPS("particles/explosions/explo1_grounddust.txt", pSystem);
			CreateMappedPS("particles/explosions/explo1_shockwave.txt", pSystem);
			CreateMappedPS("particles/explosions/explo1_sparks.txt", pSystem);
			CreateMappedPS("particles/explosions/explo1_tong.txt", pSystem);

			// play a sound as well
			switch (gEngfuncs.pfnRandomLong(0, 2)) {
				case 0:gEngfuncs.pEventAPI->EV_PlaySound(0, pSystem->vPosition, 0, "explosions/explode_dist1.wav", 1.0, ATTN_NONE, 0, PITCH_NORM); break;
				case 1:gEngfuncs.pEventAPI->EV_PlaySound(0, pSystem->vPosition, 0, "explosions/explode_dist2.wav", 1.0, ATTN_NONE, 0, PITCH_NORM); break;
				case 2:gEngfuncs.pEventAPI->EV_PlaySound(0, pSystem->vPosition, 0, "explosions/explode_dist3.wav", 1.0, ATTN_NONE, 0, PITCH_NORM); break;
			}

			// create dynamic light
			dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
			VectorCopy(pSystem->vPosition, dl->origin);
			dl->radius = 500;
			dl->color.r = 254;
			dl->color.g = 160;
			dl->color.b = 24;
			dl->decay = 0.2;
			dl->die = (gEngfuncs.GetClientTime() + 0.1);
		}
		break;
		case iDefaultSporeExplosion: {
			// explositions are made up of 5 ps
			//CreateMappedPS("particles/explosions/spore_clouds.txt", pSystem);
			//CreateMappedPS("particles/explosions/spore_groundparticles.txt", pSystem);

			// create dynamic light
			dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
			VectorCopy(pSystem->vPosition, dl->origin);
			dl->radius = 200;
			dl->color.r = 119;
			dl->color.g = 179;
			dl->color.b = 0;
			dl->decay = 0.2;
			dl->die = (gEngfuncs.GetClientTime() + 0.1);
		}
		break;

		case iImpactBSPconcrete:
		//	CreateMappedPS("particles/impact/e_impacts_chunks.txt", pSystem);
		//	CreateMappedPS("particles/impact/e_impacts_smoke.txt", pSystem);
		break;

		case iDefaultWaves:
		//	CreateMappedPS("particles/water/water_waves.txt", pSystem);
		break;

		case iWaterIcky:
		//	CreateMappedPS("particles/water/water_icky_splash.txt", pSystem);
		break;

		case iImpactBSPglass:
		//	CreateMappedPS("particles/impact/e_impacts_glass.txt", pSystem);
		break;

		
	}

	if(pSystem == NULL)
		return;
}

// wrappers to create particle system's
// flintlock smoke ps
void CParticleSystemManager::CreateFlintPS(vec3_t vPosition)
{
	if(CheckDrawSystem() == false)
		return;

	AddSystem(new CFlintlockSmokeParticleSystem(vPosition));
}

// barrel smoke ps
void CParticleSystemManager::CreateBarrelPS(vec3_t vPosition, vec3_t vDirection)
{
	if(CheckDrawSystem() == false)
		return;

	AddSystem(new CBarrelSmokeParticleSystem(vPosition, vDirection));
}

// spark ps
void CParticleSystemManager::CreateSparkPS(vec3_t vPosition, vec3_t vDirection)
{
	if(CheckDrawSystem() == false)
		return;

	AddSystem(new CSparkParticleSystem(vPosition, vDirection));
}

// white smoke ps
void CParticleSystemManager::CreateWhitePS(vec3_t vPosition, vec3_t vDirection)
{
	if(CheckDrawSystem() == false)
		return;

	AddSystem(new CWhiteSmokeParticleSystem(vPosition, vDirection));
}

// brown smoke ps
void CParticleSystemManager::CreateBrownPS(vec3_t vPosition, vec3_t vDirection)
{
	if(CheckDrawSystem() == false)
		return;

	AddSystem(new CBrownSmokeParticleSystem(vPosition, vDirection));
}

void CParticleSystemManager::CreateMuzzleFlash(vec3_t vPosition, vec3_t vDirection, int iType)
{
	if(CheckDrawSystem() == false)
		return;

	AddSystem(new CMuzzleFlashParticleSystem(vPosition, vDirection, iType));
}

// grass system
void CParticleSystemManager::CreateGrassPS( char* sFile, particle_system_management* pSystem )
{
	if(pSystem == NULL) {
		return;
	}

	// no d3d/software
	if (IEngineStudio.IsHardware() == false)
		return;

	AddSystem(new CGrassParticleSystem(sFile, pSystem));
}

// mapped ps
void CParticleSystemManager::CreateMappedPS( char* sFile, particle_system_management* pSystem )
{
	if(pSystem == NULL) {
		return;
	}
	// no d3d/software
	if (IEngineStudio.IsHardware() == false)
		return;

	AddSystem(new CMappedParticleSystem(sFile, pSystem));
}

// are we allowed to draw atm
bool CParticleSystemManager::CheckDrawSystem( void )
{
	if (gHUD.m_iHideHUDDisplay & (HIDEHUD_ALL))
		return false;

	// no d3d/software
	if (IEngineStudio.IsHardware() == false)
		return false;

	return true;
}

// adds a new texture to out cache
// using a map would be preferable but you can't snprintf into the index
void CParticleSystemManager::AddTexture(char* sName, particle_texture_s *pTexture) {
	// create a new entry and then fill it with the values
	particle_texture_cache *pCacheEntry = new particle_texture_cache;
	_snprintf(pCacheEntry->sTexture, MAX_PARTICLE_PATH-1, "%s\0", sName);
	pCacheEntry->pTexture = pTexture;

	// add the cache entry
	m_pTextures.push_back(pCacheEntry);
}

// check for a texture with the same path
particle_texture_s* CParticleSystemManager::HasTexture(char* sName) {

	unsigned int i = 0;
	unsigned int iTextures = m_pTextures.size();
	particle_texture_cache *pCacheEntry = NULL;

	// loop through all cache entries, comparing stored path with parameter path
	for (; i < iTextures; i++)
	{
		pCacheEntry = m_pTextures[i];
		if(!stricmp(pCacheEntry->sTexture, sName)) {
			return pCacheEntry->pTexture;
		}
	}
	// return the texture if we've found it, otherwise null
	return NULL;
}

// cache the most used tgas, so we don't get lag on first firing the gun
void CParticleSystemManager::PrecacheTextures( void ) 
{

	ENGINEPRINT("Caching frequently used particles, this may take a few moments\n");

	// Default explosion
	LoadTGA(NULL, const_cast<char*>(spark01));
	LoadTGA(NULL, const_cast<char*>(smoke09));
	LoadTGA(NULL, const_cast<char*>(explo_strip));
	LoadTGA(NULL, const_cast<char*>(exp_tong));
	LoadTGA(NULL, const_cast<char*>(exp_glow));
	LoadTGA(NULL, const_cast<char*>(debris_concrete001a));

	// Blood impact
	LoadTGA(NULL, const_cast<char*>(blood_red_animated));
	LoadTGA(NULL, const_cast<char*>(blood_red_drips));
	LoadTGA(NULL, const_cast<char*>(blood_red_impact));

	ENGINEPRINT("Finished caching frequently used particles, game loading will now continue\n");
}
// adds a particle into the global particle tracker
void CParticleSystemManager::AddParticle(CParticle* pParticle)
{
	if(pParticle->sParticle.bIgnoreSort == true)
		m_pUnsortedParticles.push_back(pParticle);
	else
		m_pParticles.push_back(pParticle);

	pParticle = NULL;
}

// removes a particle from the global tracker and from the system
void CParticleSystemManager::RemoveParticle(CParticle* pParticle)
{
	unsigned int i = 0;
	unsigned int iParticles = m_pParticles.size();

	// remove a particle from the sorted list
	for (; i < iParticles; i++) {
		if(pParticle == m_pParticles[i]) {
			delete m_pParticles[i];
			pParticle = NULL;
			m_pParticles.erase(m_pParticles.begin() + i);
			i--;
			iParticles--;
			return;
		}
	}

	// remove a particle from the unsorted list
	iParticles = m_pUnsortedParticles.size();
	for (i = 0; i < iParticles; i++) {
		if(pParticle == m_pUnsortedParticles[i]) {
			delete m_pUnsortedParticles[i];
			pParticle = NULL;
			m_pUnsortedParticles.erase(m_pUnsortedParticles.begin() + i);
			i--;
			iParticles--;
			return;
		}
	}
}

// remove all trackers in the system
void CParticleSystemManager::RemoveParticles()
{
	unsigned int i = 0;
	unsigned int iParticles = m_pParticles.size();

	// remove the sorted particles
	for (i = 0; i < iParticles; i++) {
		delete m_pParticles[i];
		m_pParticles[i] = NULL;
		m_pParticles.erase(m_pParticles.begin() + i);
		i--;
		iParticles--;
	}
	m_pParticles.clear();

	// remove the unsorted particles
	iParticles = m_pUnsortedParticles.size();
	for (i = 0; i < iParticles; i++) {
		delete m_pUnsortedParticles[i];
		m_pUnsortedParticles[i] = NULL;
		m_pUnsortedParticles.erase(m_pUnsortedParticles.begin() + i);
		i--;
		iParticles--;
	}
	m_pUnsortedParticles.clear();
}


// adds a new system
void CParticleSystemManager::AddSystem(CParticleSystem *pSystem) {
	m_pParticleSystems.push_back(pSystem);
}

// tbh highly inefficent but we shouldn't have any large number of ps's,
// and we won't be force removing very often so this won't be too bad
void CParticleSystemManager::RemoveSystem( unsigned int iSystem )
{
	unsigned int i = 0;
	unsigned int iParticles = m_pParticles.size();
	CParticle *pParticle = NULL;
	// remove the sorted particles
	for (i = 0; i < iParticles; i++) {
		pParticle = m_pParticles[i];
		if(pParticle && pParticle->SystemID() == iSystem) {
			delete pParticle;
			pParticle = NULL;
			m_pParticles.erase(m_pParticles.begin() + i);
			i--;
			iParticles--;
		}
	}

	// remove the unsorted particles
	iParticles = m_pUnsortedParticles.size();
	for (i = 0; i < iParticles; i++) {
		pParticle = m_pUnsortedParticles[i];
		if(pParticle && pParticle->SystemID() == iSystem) {
			delete pParticle;
			pParticle = NULL;
			m_pUnsortedParticles.erase(m_pUnsortedParticles.begin() + i);
			i--;
			iParticles--;
		}
	}

	CParticleSystem *pSystem = NULL;	
	unsigned int iSystems = m_pParticleSystems.size();
	for (; i < iSystems; i++)
	{
		pSystem = m_pParticleSystems[i];
		// i != the system id, as the server or the client can generate these
		if(pSystem && pSystem->SystemID() == iSystem) {
			delete pSystem;
			pSystem = NULL;
			m_pParticleSystems.erase(m_pParticleSystems.begin() + i);
			i--;
			iSystems--;
		}
	}
}


// deletes all systems
void CParticleSystemManager::RemoveSystems( void )
{
	unsigned int i = 0;
	unsigned int iSystems = m_pParticleSystems.size();
	for (; i < iSystems; i++) {
		delete m_pParticleSystems[i];
		m_pParticleSystems[i] = NULL;
		m_pParticleSystems.erase(m_pParticleSystems.begin() + i);
		i--;
		iSystems--;
	}
	m_pParticleSystems.clear();
}