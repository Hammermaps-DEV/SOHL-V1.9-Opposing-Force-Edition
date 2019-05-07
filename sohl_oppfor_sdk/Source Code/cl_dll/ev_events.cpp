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

#include "hud.h"
#include "cl_util.h"
#include "event_api.h"

extern "C" {
	void EV_FireNull(event_args_t *args);
	void EV_FireCrowbar(event_args_t *args);
	void EV_FireWrenchSmall(struct event_args_s *args);
	void EV_FireWrenchLarge(struct event_args_s *args);
	void EV_FireKnife(event_args_t *args);
	void EV_PlayEmptySound( struct event_args_s *args );
	void EV_FireGlock( struct event_args_s *args  );
	void EV_FireMP5( struct event_args_s *args  );
	void EV_FirePython( struct event_args_s *args  );
	void EV_FireGauss( struct event_args_s *args  );
	void EV_SpinGauss( struct event_args_s *args  );
	void EV_EgonFire( struct event_args_s *args );
	void EV_EgonStop( struct event_args_s *args );
	void EV_FireShotGun( struct event_args_s *args  );
	void EV_SnarkFire( struct event_args_s *args );
	void EV_TrainPitchAdjust( struct event_args_s *args );
	void EV_Decals( struct event_args_s *args );
	void EV_Explode( struct event_args_s *args );
	void EV_FireM249(struct event_args_s *args);
	void EV_ShockFire(struct event_args_s *args);
	void EV_FireEagle(struct event_args_s *args);
	void EV_FireSpore(struct event_args_s *args);
	void EV_FireSniper(struct event_args_s *args);
	void EV_Displacer(struct event_args_s *args);
}

/*
======================
Game_HookEvents

Associate script file name with callback functions.  Callback's must be extern "C" so
 the engine doesn't get confused about name mangling stuff.  Note that the format is
 always the same.  Of course, a clever mod team could actually embed parameters, behavior
 into the actual .sc files and create a .sc file parser and hook their functionality through
 that.. i.e., a scripting system.

That was what we were going to do, but we ran out of time...oh well.
======================
*/

void EV_HookEvents( void ) {
	gEngfuncs.pfnHookEvent( "events/null.sc",				EV_FireNull);
	gEngfuncs.pfnHookEvent( "events/crowbar.sc",			EV_FireCrowbar);
	gEngfuncs.pfnHookEvent( "events/wrench_small.sc",		EV_FireWrenchSmall);
	gEngfuncs.pfnHookEvent( "events/wrench_large.sc",		EV_FireWrenchLarge);
	gEngfuncs.pfnHookEvent( "events/knife.sc",				EV_FireKnife);
	gEngfuncs.pfnHookEvent( "events/tripfire.sc",			EV_PlayEmptySound );
	gEngfuncs.pfnHookEvent( "events/glock.sc",				EV_FireGlock );
	gEngfuncs.pfnHookEvent( "events/shotgun.sc",			EV_FireShotGun);
	gEngfuncs.pfnHookEvent( "events/mp5.sc",				EV_FireMP5 );
	gEngfuncs.pfnHookEvent( "events/explode.sc",			EV_Explode );
	gEngfuncs.pfnHookEvent( "events/python.sc",				EV_FirePython );
	gEngfuncs.pfnHookEvent( "events/gauss.sc",				EV_FireGauss );
	gEngfuncs.pfnHookEvent( "events/gaussspin.sc",			EV_SpinGauss );
	gEngfuncs.pfnHookEvent( "events/egon_fire.sc",			EV_EgonFire );
	gEngfuncs.pfnHookEvent( "events/egon_stop.sc",			EV_EgonStop );
	gEngfuncs.pfnHookEvent( "events/train.sc",				EV_TrainPitchAdjust );
	gEngfuncs.pfnHookEvent( "events/snarkfire.sc",			EV_SnarkFire );
	gEngfuncs.pfnHookEvent( "events/decals.sc",				EV_Decals );
	gEngfuncs.pfnHookEvent( "events/m249.sc",				EV_FireM249 );
	gEngfuncs.pfnHookEvent( "events/shock.sc",				EV_ShockFire );
	gEngfuncs.pfnHookEvent( "events/eagle.sc",				EV_FireEagle);
	gEngfuncs.pfnHookEvent( "events/spore.sc",				EV_FireSpore);
	gEngfuncs.pfnHookEvent( "events/sniper.sc",				EV_FireSniper);
	gEngfuncs.pfnHookEvent( "events/displacer.sc",			EV_Displacer);
}