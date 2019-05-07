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
#ifndef CDLL_DLL_H
#define CDLL_DLL_H

#define MAX_WEAPONS			45
#define WEAPON_CYCLER		31

#define MAX_WEAPON_SLOTS	7	// hud item selection slots
#define MAX_ITEM_TYPES		8	// hud item selection slots

#define MAX_ITEMS				10	//AJH (increased from 5 for expanded inventory system) hard coded item types

#define HIDEHUD_WEAPONS		( 1<<0 )
#define HIDEHUD_FLASHLIGHT	( 1<<1 )
#define HIDEHUD_ALL			( 1<<2 )
#define HIDEHUD_HEALTH		( 1<<3 )
#define HIDEHUD_CROSSHAIR	( 1<<4 ) //LRC - probably not the right way to do this, but it's just an experiment.
#define ITEM_SUIT			( 1<<5 ) //g-cont. move suit flag here

#define MAX_AMMO_TYPES		32		// ???
#define MAX_AMMO_SLOTS		32		// not really slots

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

#define WEAPON_SUIT			31

#endif
