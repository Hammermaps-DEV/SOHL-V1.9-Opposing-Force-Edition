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
#ifndef EXTDLL_H
#define EXTDLL_H


//
// Global header file for extension DLLs
//

// Allow "DEBUG" in addition to default "_DEBUG"
#ifdef _DEBUG
#define DEBUG 1
#endif

// Silence certain warnings
#pragma warning(disable : 4244)		// int or float down-conversion
#pragma warning(disable : 4305)		// int or float data truncation
#pragma warning(disable : 4201)		// nameless struct/union
#pragma warning(disable : 4514)		// unreferenced inline function removed
#pragma warning(disable : 4100)		// unreferenced formal parameter

#include "archtypes.h"

// Prevent tons of unused windows definitions
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOSERVICE
#define NOMCX
#define NOIME
#include "windows.h"
#else // _WIN32
#define FALSE 0
#define TRUE (!FALSE)
typedef uint32 ULONG;
typedef unsigned char BYTE;
typedef int BOOL;
#define MAX_PATH PATH_MAX
#include <limits.h>
#include <stdarg.h>
#include <string.h> // memset 
#endif //_WIN32

// Misc C-runtime library headers
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "platform.h"

// Header file containing definition of globalvars_t and entvars_t
typedef unsigned int func_t;					//
typedef unsigned int string_t;				// from engine's pr_comp.h;
typedef float vec_t;				// needed before including progdefs.h

// Vector class
#include "vector.h"

// Defining it as a (bogus) struct helps enforce type-checking
#define vec3_t Vector

// Shared engine/DLL constants
#include "const.h"
#include "progdefs.h"
#include "edict.h"

// Shared header describing protocol between engine and DLLs
#include "eiface.h"

// Shared header between the client DLL and the game DLLs
#include "cdll_dll.h"

#endif //EXTDLL_H
