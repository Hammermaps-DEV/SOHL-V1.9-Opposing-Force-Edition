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
#ifndef PARTICLE_TEXTURE_H
#define PARTICLE_TEXTURE_H

#ifdef CLIENT_DLL
#include <windows.h>
#include "gl/gl.h"
#include "gl/glaux.h"
#include "r_studioint.h"

// our texture
struct particle_texture_s								
{
	particle_texture_s() { iID = NULL; imageData = NULL; }

	GLubyte	*imageData;
	unsigned int	iBpp;
	unsigned int	iWidth;
	unsigned int	iHeight;
	int				iID;
	unsigned int	iType;
	int	m_iTexture;
};	

// our tga file's header
struct tga_header
{
	unsigned char	cHeader[12];
};

// our tga file definition
struct tga
{
	unsigned char	cHeader[6];
	unsigned int	iImageSize;
	unsigned int	iTemp;
	unsigned int	iType;
	unsigned int	iWidth;
	unsigned int	iHeight;
	unsigned int	iBpp;
	unsigned int	iBytespp;
};

extern bool LoadUncompressedTGA(particle_texture_s *, FILE *);
extern bool LoadCompressedTGA(particle_texture_s *, FILE *);


// Global engine <-> studio model rendering code interface
extern engine_studio_api_t IEngineStudio;		

#endif
#endif

  