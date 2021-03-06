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
#ifndef PARTICLE_SYSTEM_MANAGER_H
#define PARTICLE_SYSTEM_MANAGER_H

// in an ideal world this would be a true singleton,
// but I don't have a singleton template class atm, so this will do
class CParticleSystemManager {
private:
	// our list of particle systems.
	vector <CParticleSystem*> m_pParticleSystems;
	// our cached textures
	vector <particle_texture_cache*> m_pTextures;
	// our lists of particles
	vector <CParticle*> m_pParticles;
	vector <CParticle*> m_pUnsortedParticles;

	// time of last sort
	float m_flLastSort;
	// time of last draw
	float m_flLastDraw;
	// time of last debug message
	float m_flLastDebug;

	// adds in a new system
	void AddSystem(CParticleSystem* pSystem);
	// determines whether we're gonna allow drawing atm
	bool CheckDrawSystem( void );
	// returns the length of time between this draw and the last one
	inline float TimeSinceLastDraw( void ) const { return (gEngfuncs.GetClientTime() - m_flLastDraw); }
public:
	// creates and destroys a particle system
	CParticleSystemManager() { 
		m_flLastSort = 0.0; m_flLastDebug = 0.0;
		m_pParticleSystems.clear(); m_pParticles.clear(); m_pUnsortedParticles.clear();
	}
	~CParticleSystemManager() { RemoveParticles(); RemoveSystems(); }

	// allows us to remove all our systems
	void RemoveSystems( void );
	// remove a single system
	void RemoveSystem( unsigned int iSystem);
	// clears the textures map
//	void RemoveTextures( void );
	// deletes all particles
	void RemoveParticles( void );

	// updates all systems
	void UpdateSystems( void );

	// wrappers to create particle systems
	void CreateFlintPS(vec3_t vPosition);
	void CreateBarrelPS(vec3_t vPosition, vec3_t vDirection);
	void CreateSparkPS(vec3_t vPosition, vec3_t vDirection);
	void CreateWhitePS(vec3_t vPosition, vec3_t vDirection);
	void CreateBrownPS(vec3_t vPosition, vec3_t vDirection);
	void CreateGrassPS(char* sFile, particle_system_management* pSystem );
	void CreateMappedPS(char* sFile, particle_system_management* pSystem );
	void CreatePresetPS(unsigned int iPreset, particle_system_management* pSystem);
	void CreateMuzzleFlash(vec3_t vPosition, vec3_t vDirection, int iType);

	// Adds Particles so we can do the actual drawing
	void AddParticle(CParticle* pParticle);
	// deletes a particle
	void RemoveParticle( CParticle* pParticle );

	// Allows us to maintain a lookup table of loaded textures
	void AddTexture(char* sName, particle_texture_s* pTexture);
	particle_texture_s* HasTexture(char* sName);

	// Precaches commonly used textures on map load so that we don't have to have pauses while
	// they are loaded
	void PrecacheTextures( void );
};

// TODO : Singleton the particle system manager
#endif
