//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
// New clipping style camera - original idea by Xwider
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "hud.h"
#include "cl_util.h"
#include "camera.h"
#include "in_defs.h"
#include "exports.h"

int iMouseInUse = 0;

void CL_DLLEXPORT CAM_Think( void )
{
}

void CAM_Init( void )
{
}

int CL_DLLEXPORT CL_IsThirdPerson( void )
{
	return (gHUD.m_iCameraMode ? 1 : 0) || (g_iUser1 && (g_iUser2 == gEngfuncs.GetLocalPlayer()->index) );
}

void CL_DLLEXPORT CL_CameraOffset( float *ofs )
{
	VectorCopy( vec3_origin, ofs );
}
