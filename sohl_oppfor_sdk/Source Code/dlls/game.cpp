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

#include "extdll.h"
#include "eiface.h"
#include "util.h"
#include "game.h"

//Macros to make skill cvars easier to define
#define DECLARE_SKILL_CVARS( name )					\
cvar_t	sk_##name##1 = { "sk_" #name "1", "0" };	\
cvar_t	sk_##name##2 = { "sk_" #name "2", "0" };	\
cvar_t	sk_##name##3 = { "sk_" #name "3", "0" }

#define REGISTER_SKILL_CVARS( name )	\
CVAR_REGISTER( &sk_##name##1 );			\
CVAR_REGISTER( &sk_##name##2 );			\
CVAR_REGISTER( &sk_##name##3 )

cvar_t	displaysoundlist = { "displaysoundlist","0" };

// multiplayer server rules
cvar_t	fragsleft = { "mp_fragsleft","0", FCVAR_SERVER | FCVAR_UNLOGGED };	  // Don't spam console/log files/users with this changing
cvar_t	timeleft = { "mp_timeleft","0" , FCVAR_SERVER | FCVAR_UNLOGGED };	  // "      "

// multiplayer server rules
cvar_t	teamplay = { "mp_teamplay","0", FCVAR_SERVER };
cvar_t	fraglimit = { "mp_fraglimit","0", FCVAR_SERVER };
cvar_t	timelimit = { "mp_timelimit","0", FCVAR_SERVER };
cvar_t	friendlyfire = { "mp_friendlyfire","0", FCVAR_SERVER };
cvar_t	falldamage = { "mp_falldamage","0", FCVAR_SERVER };
cvar_t	weaponstay = { "mp_weaponstay","0", FCVAR_SERVER };
cvar_t	forcerespawn = { "mp_forcerespawn","1", FCVAR_SERVER };
cvar_t	flashlight = { "mp_flashlight","0", FCVAR_SERVER };
cvar_t	aimcrosshair = { "mp_autocrosshair","1", FCVAR_SERVER };
cvar_t	decalfrequency = { "decalfrequency","30", FCVAR_SERVER };
cvar_t	teamlist = { "mp_teamlist","hgrunt;scientist", FCVAR_SERVER };
cvar_t	teamoverride = { "mp_teamoverride","1" };
cvar_t	defaultteam = { "mp_defaultteam","0" };
cvar_t	allowmonsters = { "mp_allowmonsters","0", FCVAR_SERVER };

cvar_t	impulsetarget = { "sohl_impulsetarget","0", FCVAR_SERVER }; //LRC - trigger ents manually
cvar_t	mw_debug = { "sohl_mwdebug","0", FCVAR_SERVER }; //LRC - debug info. for MoveWith. (probably not useful for most people.)

cvar_t  mp_chattime = { "mp_chattime","10", FCVAR_SERVER };

// Engine Cvars
cvar_t 	*g_psv_gravity = NULL;
cvar_t	*g_psv_aim = NULL;
cvar_t	*g_footsteps = NULL;

//CVARS FOR SKILL LEVEL SETTINGS

// Agrunt
DECLARE_SKILL_CVARS(agrunt_health);
DECLARE_SKILL_CVARS(agrunt_dmg_punch);

// Apache
DECLARE_SKILL_CVARS(apache_health);

// Osprey
DECLARE_SKILL_CVARS(osprey_health);

// Apache BlackOP
DECLARE_SKILL_CVARS(apache_blkop_health);

// Osprey BlackOP
DECLARE_SKILL_CVARS(osprey_blkop_health);

// Barney
DECLARE_SKILL_CVARS(barney_health);
DECLARE_SKILL_CVARS(barney_head);
DECLARE_SKILL_CVARS(barney_chest);
DECLARE_SKILL_CVARS(barney_stomach);
DECLARE_SKILL_CVARS(barney_leg);
DECLARE_SKILL_CVARS(barney_arm);

// Barniel
DECLARE_SKILL_CVARS(barniel_health);
DECLARE_SKILL_CVARS(barniel_head);
DECLARE_SKILL_CVARS(barniel_chest);
DECLARE_SKILL_CVARS(barniel_stomach);
DECLARE_SKILL_CVARS(barniel_leg);
DECLARE_SKILL_CVARS(barniel_arm);

// Otis Laurey
DECLARE_SKILL_CVARS(otis_health);
DECLARE_SKILL_CVARS(otis_head);
DECLARE_SKILL_CVARS(otis_chest);
DECLARE_SKILL_CVARS(otis_stomach);
DECLARE_SKILL_CVARS(otis_leg);
DECLARE_SKILL_CVARS(otis_arm);

// Bullsquid
DECLARE_SKILL_CVARS(bullsquid_health);
DECLARE_SKILL_CVARS(bullsquid_dmg_bite);
DECLARE_SKILL_CVARS(bullsquid_dmg_whip);
DECLARE_SKILL_CVARS(bullsquid_dmg_spit);

// Big Momma
DECLARE_SKILL_CVARS(bigmomma_health_factor);
DECLARE_SKILL_CVARS(bigmomma_dmg_slash);
DECLARE_SKILL_CVARS(bigmomma_dmg_blast);
DECLARE_SKILL_CVARS(bigmomma_radius_blast);

// Gargantua
DECLARE_SKILL_CVARS(gargantua_health);
DECLARE_SKILL_CVARS(gargantua_dmg_slash);
DECLARE_SKILL_CVARS(gargantua_dmg_fire);
DECLARE_SKILL_CVARS(gargantua_dmg_stomp);

// Hassassin
DECLARE_SKILL_CVARS(hassassin_health);

// Headcrab
DECLARE_SKILL_CVARS(headcrab_health);
DECLARE_SKILL_CVARS(headcrab_dmg_bite);

// Hgrunt 
DECLARE_SKILL_CVARS(hgrunt_health);
DECLARE_SKILL_CVARS(hgrunt_kick);
DECLARE_SKILL_CVARS(hgrunt_pellets);
DECLARE_SKILL_CVARS(hgrunt_gspeed);

// Houndeye
DECLARE_SKILL_CVARS(houndeye_health);
DECLARE_SKILL_CVARS(houndeye_dmg_blast);

// ISlave
DECLARE_SKILL_CVARS(islave_health);
DECLARE_SKILL_CVARS(islave_dmg_claw);
DECLARE_SKILL_CVARS(islave_dmg_clawrake);
DECLARE_SKILL_CVARS(islave_dmg_zap);

// Icthyosaur
DECLARE_SKILL_CVARS(ichthyosaur_health);
DECLARE_SKILL_CVARS(ichthyosaur_shake);

// Leech
DECLARE_SKILL_CVARS(leech_health);
DECLARE_SKILL_CVARS(leech_dmg_bite);

// Controller
DECLARE_SKILL_CVARS(controller_health);
DECLARE_SKILL_CVARS(controller_dmgzap);
DECLARE_SKILL_CVARS(controller_speedball);
DECLARE_SKILL_CVARS(controller_dmgball);

// Nihilanth
DECLARE_SKILL_CVARS(nihilanth_health);
DECLARE_SKILL_CVARS(nihilanth_zap);

// Scientist
DECLARE_SKILL_CVARS(scientist_health);

//=========================================================
// NPCs: Construction
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(construction_health);
DECLARE_SKILL_CVARS(construction_head);
DECLARE_SKILL_CVARS(construction_chest);
DECLARE_SKILL_CVARS(construction_stomach);
DECLARE_SKILL_CVARS(construction_leg);
DECLARE_SKILL_CVARS(construction_arm);

// Snark
DECLARE_SKILL_CVARS(snark_health);
DECLARE_SKILL_CVARS(snark_dmg_bite);
DECLARE_SKILL_CVARS(snark_dmg_pop);

//=========================================================
// NPCs: Diablo
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(diablo_health);
DECLARE_SKILL_CVARS(diablo_dmg_one_slash);
DECLARE_SKILL_CVARS(diablo_dmg_both_slash);
DECLARE_SKILL_CVARS(diablo_head);
DECLARE_SKILL_CVARS(diablo_chest);
DECLARE_SKILL_CVARS(diablo_stomach);
DECLARE_SKILL_CVARS(diablo_leg);

//=========================================================
// NPCs: Zombie,Zombie Soldier,Zombie Barney
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================

//=========================================================
// NPCs: Zombie Scientist
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(zombie_health);
DECLARE_SKILL_CVARS(zombie_dmg_one_slash);
DECLARE_SKILL_CVARS(zombie_dmg_both_slash);
DECLARE_SKILL_CVARS(zombie_head);
DECLARE_SKILL_CVARS(zombie_chest);
DECLARE_SKILL_CVARS(zombie_stomach);
DECLARE_SKILL_CVARS(zombie_leg);
DECLARE_SKILL_CVARS(zombie_arm);

//=========================================================
// NPCs: Zombie Soldier
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(zombie_soldier_health);
DECLARE_SKILL_CVARS(zombie_soldier_dmg_one_slash);
DECLARE_SKILL_CVARS(zombie_soldier_dmg_both_slash);
DECLARE_SKILL_CVARS(zombie_soldier_head);
DECLARE_SKILL_CVARS(zombie_soldier_chest);
DECLARE_SKILL_CVARS(zombie_soldier_stomach);
DECLARE_SKILL_CVARS(zombie_soldier_leg);
DECLARE_SKILL_CVARS(zombie_soldier_arm);

//=========================================================
// NPCs: Zombie Barney
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(zombie_barney_health);
DECLARE_SKILL_CVARS(zombie_barney_dmg_one_slash);
DECLARE_SKILL_CVARS(zombie_barney_dmg_both_slash);
DECLARE_SKILL_CVARS(zombie_barney_head);
DECLARE_SKILL_CVARS(zombie_barney_chest);
DECLARE_SKILL_CVARS(zombie_barney_stomach);
DECLARE_SKILL_CVARS(zombie_barney_leg);
DECLARE_SKILL_CVARS(zombie_barney_arm);

//=========================================================
// NPCs: Zombie Construction
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(zombie_construction_health);
DECLARE_SKILL_CVARS(zombie_construction_dmg_one_slash);
DECLARE_SKILL_CVARS(zombie_construction_dmg_both_slash);
DECLARE_SKILL_CVARS(zombie_construction_head);
DECLARE_SKILL_CVARS(zombie_construction_chest);
DECLARE_SKILL_CVARS(zombie_construction_stomach);
DECLARE_SKILL_CVARS(zombie_construction_leg);
DECLARE_SKILL_CVARS(zombie_construction_arm);

//=========================================================
// NPCs: Pit Drone
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(pitdrone_health);
DECLARE_SKILL_CVARS(pitdrone_dmg_bite);
DECLARE_SKILL_CVARS(pitdrone_dmg_whip);
DECLARE_SKILL_CVARS(pitdrone_dmg_spit);
DECLARE_SKILL_CVARS(pitdrone_head);
DECLARE_SKILL_CVARS(pitdrone_chest);
DECLARE_SKILL_CVARS(pitdrone_stomach);
DECLARE_SKILL_CVARS(pitdrone_leg);
DECLARE_SKILL_CVARS(pitdrone_arm);

//=========================================================
// NPCs: Voltigore
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(voltigore_health);
DECLARE_SKILL_CVARS(voltigore_dmg_punch);
DECLARE_SKILL_CVARS(voltigore_dmg_beam);
DECLARE_SKILL_CVARS(voltigore_head);
DECLARE_SKILL_CVARS(voltigore_chest);
DECLARE_SKILL_CVARS(voltigore_stomach);
DECLARE_SKILL_CVARS(voltigore_leg);
DECLARE_SKILL_CVARS(voltigore_arm);

//=========================================================
// NPCs: Ally Grunt 
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(hgrunt_ally_health);
DECLARE_SKILL_CVARS(hgrunt_ally_kick);
DECLARE_SKILL_CVARS(hgrunt_ally_pellets);
DECLARE_SKILL_CVARS(hgrunt_ally_gspeed);
DECLARE_SKILL_CVARS(hgrunt_ally_head);
DECLARE_SKILL_CVARS(hgrunt_ally_chest);
DECLARE_SKILL_CVARS(hgrunt_ally_stomach);
DECLARE_SKILL_CVARS(hgrunt_ally_leg);
DECLARE_SKILL_CVARS(hgrunt_ally_arm);

//=========================================================
// NPCs: Ally Grunt Medic
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(medic_ally_health);
DECLARE_SKILL_CVARS(medic_ally_kick);
DECLARE_SKILL_CVARS(medic_ally_heal);

//=========================================================
// NPCs: Ally Grunt Torch
// For Spirit of Half-Life v1.9: Opposing-Force Edition
//=========================================================
DECLARE_SKILL_CVARS(torch_ally_health);
DECLARE_SKILL_CVARS(torch_ally_kick);

// Pitworm
DECLARE_SKILL_CVARS(pitworm_health);
DECLARE_SKILL_CVARS(pitworm_dmg_swipe);
DECLARE_SKILL_CVARS(pitworm_dmg_beam);

// Shock Roach
DECLARE_SKILL_CVARS(shockroach_health);
DECLARE_SKILL_CVARS(shockroach_dmg_bite);
DECLARE_SKILL_CVARS(shockroach_lifespan);

// Gonome
DECLARE_SKILL_CVARS(gonome_health);
DECLARE_SKILL_CVARS(gonome_dmg_one_slash);
DECLARE_SKILL_CVARS(gonome_dmg_guts);
DECLARE_SKILL_CVARS(gonome_dmg_one_bite);

// ShockTrooper 
DECLARE_SKILL_CVARS(shocktrooper_health);
DECLARE_SKILL_CVARS(shocktrooper_kick);
DECLARE_SKILL_CVARS(shocktrooper_gspeed);
DECLARE_SKILL_CVARS(shocktrooper_maxcharge);
DECLARE_SKILL_CVARS(shocktrooper_rchgspeed);

//Turret
DECLARE_SKILL_CVARS(turret_health);

// MiniTurret
DECLARE_SKILL_CVARS(miniturret_health);

// Sentry Turret
DECLARE_SKILL_CVARS(sentry_health);

// PLAYER WEAPONS

// Crowbar whack
DECLARE_SKILL_CVARS(plr_crowbar);

// Pipe Wrench
DECLARE_SKILL_CVARS(plr_pipewrench);

// Knife whack
DECLARE_SKILL_CVARS(plr_knife);
DECLARE_SKILL_CVARS(plr_knife_charge);

// Glock Round
DECLARE_SKILL_CVARS(plr_9mm_bullet);

// 357 Round
DECLARE_SKILL_CVARS(plr_357_bullet);

// 762 Round
DECLARE_SKILL_CVARS(plr_762_bullet);

// MP5 Round
DECLARE_SKILL_CVARS(plr_9mmAR_bullet);

// M203 grenade
DECLARE_SKILL_CVARS(plr_9mmAR_grenade);

// Shotgun buckshot
DECLARE_SKILL_CVARS(plr_buckshot);

// Crossbow
DECLARE_SKILL_CVARS(plr_xbow_bolt_client);
DECLARE_SKILL_CVARS(plr_xbow_bolt_monster);

// RPG
DECLARE_SKILL_CVARS(plr_rpg);

// Zero Point Generator
DECLARE_SKILL_CVARS(plr_gauss);

// Tau Cannon
DECLARE_SKILL_CVARS(plr_egon_narrow);
DECLARE_SKILL_CVARS(plr_egon_wide);

// Hand Grendade
DECLARE_SKILL_CVARS(plr_hand_grenade);

// Satchel Charge
DECLARE_SKILL_CVARS(plr_satchel);

// Tripmine
DECLARE_SKILL_CVARS(plr_tripmine);

// 556 Player
DECLARE_SKILL_CVARS(plr_556_bullet);

// WORLD WEAPONS
DECLARE_SKILL_CVARS(12mm_bullet);
DECLARE_SKILL_CVARS(9mmAR_bullet);
DECLARE_SKILL_CVARS(9mm_bullet);

// 556
DECLARE_SKILL_CVARS(556_bullet);

// HORNET
DECLARE_SKILL_CVARS(hornet_dmg);

// SHOCKROACH
DECLARE_SKILL_CVARS(plr_shockroachs);
DECLARE_SKILL_CVARS(plr_shockroachm);

// SPOREGRENADE
DECLARE_SKILL_CVARS(plr_spore);

// HEALTH/CHARGE
DECLARE_SKILL_CVARS(suitcharger);
DECLARE_SKILL_CVARS(battery);
DECLARE_SKILL_CVARS(healthcharger);
DECLARE_SKILL_CVARS(healthkit);
DECLARE_SKILL_CVARS(scientist_heal);

// FLASHLIGHT CHARGE VALUE
DECLARE_SKILL_CVARS(flashcharge);

// monster damage adjusters
DECLARE_SKILL_CVARS(monster_head);
DECLARE_SKILL_CVARS(monster_chest);
DECLARE_SKILL_CVARS(monster_stomach);
DECLARE_SKILL_CVARS(monster_arm);
DECLARE_SKILL_CVARS(monster_leg);

// player damage adjusters
DECLARE_SKILL_CVARS(player_head);
DECLARE_SKILL_CVARS(player_chest);
DECLARE_SKILL_CVARS(player_stomach);
DECLARE_SKILL_CVARS(player_arm);
DECLARE_SKILL_CVARS(player_leg);

// END Cvars for Skill Level settings

// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit()
{
	// Register cvars here:
	g_psv_gravity = CVAR_GET_POINTER("sv_gravity");
	g_psv_aim = CVAR_GET_POINTER("sv_aim");
	g_footsteps = CVAR_GET_POINTER("mp_footsteps");

	CVAR_REGISTER(&displaysoundlist);

	CVAR_REGISTER(&teamplay);
	CVAR_REGISTER(&fraglimit);
	CVAR_REGISTER(&timelimit);

	CVAR_REGISTER(&fragsleft);
	CVAR_REGISTER(&timeleft);

	CVAR_REGISTER(&friendlyfire);
	CVAR_REGISTER(&falldamage);
	CVAR_REGISTER(&weaponstay);
	CVAR_REGISTER(&forcerespawn);
	CVAR_REGISTER(&flashlight);
	CVAR_REGISTER(&aimcrosshair);
	CVAR_REGISTER(&decalfrequency);
	CVAR_REGISTER(&teamlist);
	CVAR_REGISTER(&teamoverride);
	CVAR_REGISTER(&defaultteam);
	CVAR_REGISTER(&allowmonsters);
	CVAR_REGISTER(&impulsetarget); //LRC
	CVAR_REGISTER(&mw_debug); //LRC
	CVAR_REGISTER(&mp_chattime);

	// REGISTER CVARS FOR SKILL LEVEL STUFF
	// Agrunt
	REGISTER_SKILL_CVARS(agrunt_health);
	REGISTER_SKILL_CVARS(agrunt_dmg_punch);

	// Apache
	REGISTER_SKILL_CVARS(apache_health);

	// Osprey
	CVAR_REGISTER(&sk_osprey_health1);
	CVAR_REGISTER(&sk_osprey_health2);
	CVAR_REGISTER(&sk_osprey_health3);

	// Apache BlackOP
	CVAR_REGISTER(&sk_apache_blkop_health1);
	CVAR_REGISTER(&sk_apache_blkop_health2);
	CVAR_REGISTER(&sk_apache_blkop_health3);

	// Osprey BlackOP
	CVAR_REGISTER(&sk_osprey_blkop_health1);
	CVAR_REGISTER(&sk_osprey_blkop_health2);
	CVAR_REGISTER(&sk_osprey_blkop_health3);

	// Barney
	REGISTER_SKILL_CVARS(barney_health);

	CVAR_REGISTER(&sk_barney_head1);
	CVAR_REGISTER(&sk_barney_head2);
	CVAR_REGISTER(&sk_barney_head3);

	CVAR_REGISTER(&sk_barney_chest1);
	CVAR_REGISTER(&sk_barney_chest2);
	CVAR_REGISTER(&sk_barney_chest3);

	CVAR_REGISTER(&sk_barney_stomach1);
	CVAR_REGISTER(&sk_barney_stomach2);
	CVAR_REGISTER(&sk_barney_stomach3);

	CVAR_REGISTER(&sk_barney_leg1);
	CVAR_REGISTER(&sk_barney_leg2);
	CVAR_REGISTER(&sk_barney_leg3);

	CVAR_REGISTER(&sk_barney_arm1);
	CVAR_REGISTER(&sk_barney_arm2);
	CVAR_REGISTER(&sk_barney_arm3);

	// Barniel
	CVAR_REGISTER(&sk_barniel_health1);
	CVAR_REGISTER(&sk_barniel_health2);
	CVAR_REGISTER(&sk_barniel_health3);

	CVAR_REGISTER(&sk_barniel_head1);
	CVAR_REGISTER(&sk_barniel_head2);
	CVAR_REGISTER(&sk_barniel_head3);

	CVAR_REGISTER(&sk_barniel_chest1);
	CVAR_REGISTER(&sk_barniel_chest2);
	CVAR_REGISTER(&sk_barniel_chest3);

	CVAR_REGISTER(&sk_barniel_stomach1);
	CVAR_REGISTER(&sk_barniel_stomach2);
	CVAR_REGISTER(&sk_barniel_stomach3);

	CVAR_REGISTER(&sk_barniel_leg1);
	CVAR_REGISTER(&sk_barniel_leg2);
	CVAR_REGISTER(&sk_barniel_leg3);

	CVAR_REGISTER(&sk_barniel_arm1);
	CVAR_REGISTER(&sk_barniel_arm2);
	CVAR_REGISTER(&sk_barniel_arm3);

	// Otis Laurey
	CVAR_REGISTER(&sk_otis_health1);
	CVAR_REGISTER(&sk_otis_health2);
	CVAR_REGISTER(&sk_otis_health3);

	CVAR_REGISTER(&sk_otis_head1);
	CVAR_REGISTER(&sk_otis_head2);
	CVAR_REGISTER(&sk_otis_head3);

	CVAR_REGISTER(&sk_otis_chest1);
	CVAR_REGISTER(&sk_otis_chest2);
	CVAR_REGISTER(&sk_otis_chest3);

	CVAR_REGISTER(&sk_otis_stomach1);
	CVAR_REGISTER(&sk_otis_stomach2);
	CVAR_REGISTER(&sk_otis_stomach3);

	CVAR_REGISTER(&sk_otis_leg1);
	CVAR_REGISTER(&sk_otis_leg2);
	CVAR_REGISTER(&sk_otis_leg3);

	CVAR_REGISTER(&sk_otis_arm1);
	CVAR_REGISTER(&sk_otis_arm2);
	CVAR_REGISTER(&sk_otis_arm3);

	// Bullsquid
	REGISTER_SKILL_CVARS(bullsquid_health);
	REGISTER_SKILL_CVARS(bullsquid_dmg_bite);
	REGISTER_SKILL_CVARS(bullsquid_dmg_whip);
	REGISTER_SKILL_CVARS(bullsquid_dmg_spit);

	// Bigmomma
	REGISTER_SKILL_CVARS(bigmomma_health_factor);
	REGISTER_SKILL_CVARS(bigmomma_dmg_slash);
	REGISTER_SKILL_CVARS(bigmomma_dmg_blast);
	REGISTER_SKILL_CVARS(bigmomma_radius_blast);

	// Gargantua
	REGISTER_SKILL_CVARS(gargantua_health);
	REGISTER_SKILL_CVARS(gargantua_dmg_slash);
	REGISTER_SKILL_CVARS(gargantua_dmg_fire);
	REGISTER_SKILL_CVARS(gargantua_dmg_stomp);

	// Hassassin
	REGISTER_SKILL_CVARS(hassassin_health);

	// Headcrab
	REGISTER_SKILL_CVARS(headcrab_health);
	REGISTER_SKILL_CVARS(headcrab_dmg_bite);

	// Hgrunt
	REGISTER_SKILL_CVARS(hgrunt_health);
	REGISTER_SKILL_CVARS(hgrunt_kick);
	REGISTER_SKILL_CVARS(hgrunt_pellets);
	REGISTER_SKILL_CVARS(hgrunt_gspeed);

	// Houndeye
	REGISTER_SKILL_CVARS(houndeye_health);
	REGISTER_SKILL_CVARS(houndeye_dmg_blast);

	// ISlave
	REGISTER_SKILL_CVARS(islave_health);
	REGISTER_SKILL_CVARS(islave_dmg_claw);
	REGISTER_SKILL_CVARS(islave_dmg_clawrake);
	REGISTER_SKILL_CVARS(islave_dmg_zap);

	// Icthyosaur
	REGISTER_SKILL_CVARS(ichthyosaur_health);
	REGISTER_SKILL_CVARS(ichthyosaur_shake);

	// Leech
	REGISTER_SKILL_CVARS(leech_health);
	REGISTER_SKILL_CVARS(leech_dmg_bite);

	// Controller
	REGISTER_SKILL_CVARS(controller_health);
	REGISTER_SKILL_CVARS(controller_dmgzap);
	REGISTER_SKILL_CVARS(controller_speedball);
	REGISTER_SKILL_CVARS(controller_dmgball);

	// Nihilanth
	REGISTER_SKILL_CVARS(nihilanth_health);
	REGISTER_SKILL_CVARS(nihilanth_zap);

	// Scientist
	REGISTER_SKILL_CVARS(scientist_health);

	// Construction
	REGISTER_SKILL_CVARS(construction_health);
	REGISTER_SKILL_CVARS(construction_head);
	REGISTER_SKILL_CVARS(construction_chest);
	REGISTER_SKILL_CVARS(construction_stomach);
	REGISTER_SKILL_CVARS(construction_leg);
	REGISTER_SKILL_CVARS(construction_arm);

	// Snark
	REGISTER_SKILL_CVARS(snark_health);
	REGISTER_SKILL_CVARS(snark_dmg_bite);
	REGISTER_SKILL_CVARS(snark_dmg_pop);

	//=========================================================
	// NPCs: Diablo
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	REGISTER_SKILL_CVARS(diablo_health);
	REGISTER_SKILL_CVARS(diablo_dmg_one_slash);
	REGISTER_SKILL_CVARS(diablo_dmg_both_slash);
	REGISTER_SKILL_CVARS(diablo_head);
	REGISTER_SKILL_CVARS(diablo_chest);
	REGISTER_SKILL_CVARS(diablo_stomach);
	REGISTER_SKILL_CVARS(diablo_leg);

	//=========================================================
	// NPCs: Zombie,Zombie Soldier,Zombie Barney
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================

	// Zombie Scientist
	REGISTER_SKILL_CVARS(zombie_health);
	REGISTER_SKILL_CVARS(zombie_dmg_one_slash);
	REGISTER_SKILL_CVARS(zombie_dmg_both_slash);
	REGISTER_SKILL_CVARS(zombie_head);
	REGISTER_SKILL_CVARS(zombie_chest);
	REGISTER_SKILL_CVARS(zombie_stomach);
	REGISTER_SKILL_CVARS(zombie_leg);
	REGISTER_SKILL_CVARS(zombie_arm);

	// Zombie Soldier
	REGISTER_SKILL_CVARS(zombie_soldier_health);
	REGISTER_SKILL_CVARS(zombie_soldier_dmg_one_slash);
	REGISTER_SKILL_CVARS(zombie_soldier_dmg_both_slash);
	REGISTER_SKILL_CVARS(zombie_soldier_head);
	REGISTER_SKILL_CVARS(zombie_soldier_chest);
	REGISTER_SKILL_CVARS(zombie_soldier_stomach);
	REGISTER_SKILL_CVARS(zombie_soldier_leg);
	REGISTER_SKILL_CVARS(zombie_soldier_arm);

	// Zombie Construction
	REGISTER_SKILL_CVARS(zombie_construction_health);
	REGISTER_SKILL_CVARS(zombie_construction_dmg_one_slash);
	REGISTER_SKILL_CVARS(zombie_construction_dmg_both_slash);
	REGISTER_SKILL_CVARS(zombie_construction_head);
	REGISTER_SKILL_CVARS(zombie_construction_chest);
	REGISTER_SKILL_CVARS(zombie_construction_stomach);
	REGISTER_SKILL_CVARS(zombie_construction_leg);
	REGISTER_SKILL_CVARS(zombie_construction_arm);

	// Zombie Barney
	REGISTER_SKILL_CVARS(zombie_barney_health);
	REGISTER_SKILL_CVARS(zombie_barney_dmg_one_slash);
	REGISTER_SKILL_CVARS(zombie_barney_dmg_both_slash);
	REGISTER_SKILL_CVARS(zombie_barney_head);
	REGISTER_SKILL_CVARS(zombie_barney_chest);
	REGISTER_SKILL_CVARS(zombie_barney_stomach);
	REGISTER_SKILL_CVARS(zombie_barney_leg);
	REGISTER_SKILL_CVARS(zombie_barney_arm);

	//=========================================================
	// NPCs: Pit Drone
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	REGISTER_SKILL_CVARS(pitdrone_health);
	REGISTER_SKILL_CVARS(pitdrone_dmg_bite);
	REGISTER_SKILL_CVARS(pitdrone_dmg_whip);
	REGISTER_SKILL_CVARS(pitdrone_dmg_spit);
	REGISTER_SKILL_CVARS(pitdrone_head);
	REGISTER_SKILL_CVARS(pitdrone_chest);
	REGISTER_SKILL_CVARS(pitdrone_stomach);
	REGISTER_SKILL_CVARS(pitdrone_leg);
	REGISTER_SKILL_CVARS(pitdrone_arm);

	//=========================================================
	// NPCs: Voltigore
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	REGISTER_SKILL_CVARS(voltigore_health);
	REGISTER_SKILL_CVARS(voltigore_dmg_punch);
	REGISTER_SKILL_CVARS(voltigore_dmg_beam);
	REGISTER_SKILL_CVARS(voltigore_head);
	REGISTER_SKILL_CVARS(voltigore_chest);
	REGISTER_SKILL_CVARS(voltigore_stomach);
	REGISTER_SKILL_CVARS(voltigore_leg);
	REGISTER_SKILL_CVARS(voltigore_arm);

	//=========================================================
	// NPCs: Ally Grunt
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	REGISTER_SKILL_CVARS(hgrunt_ally_health);
	REGISTER_SKILL_CVARS(hgrunt_ally_kick);
	REGISTER_SKILL_CVARS(hgrunt_ally_pellets);
	REGISTER_SKILL_CVARS(hgrunt_ally_gspeed);
	REGISTER_SKILL_CVARS(hgrunt_ally_head);
	REGISTER_SKILL_CVARS(hgrunt_ally_chest);
	REGISTER_SKILL_CVARS(hgrunt_ally_stomach);
	REGISTER_SKILL_CVARS(hgrunt_ally_leg);
	REGISTER_SKILL_CVARS(hgrunt_ally_arm);

	//=========================================================
	// NPCs: Ally Grunt Medic
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	REGISTER_SKILL_CVARS(medic_ally_health);
	REGISTER_SKILL_CVARS(medic_ally_kick);
	REGISTER_SKILL_CVARS(medic_ally_heal);

	//=========================================================
	// NPCs: Ally Grunt Torch
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	REGISTER_SKILL_CVARS(torch_ally_health);
	REGISTER_SKILL_CVARS(torch_ally_kick);

	// Pitworm
	REGISTER_SKILL_CVARS(pitworm_health);
	REGISTER_SKILL_CVARS(pitworm_dmg_swipe);
	REGISTER_SKILL_CVARS(pitworm_dmg_beam);

	// Shock Roach
	REGISTER_SKILL_CVARS(shockroach_health);
	REGISTER_SKILL_CVARS(shockroach_dmg_bite);
	REGISTER_SKILL_CVARS(shockroach_lifespan);

	// Gonome
	REGISTER_SKILL_CVARS(gonome_health);
	REGISTER_SKILL_CVARS(gonome_dmg_one_slash);
	REGISTER_SKILL_CVARS(gonome_dmg_guts);
	REGISTER_SKILL_CVARS(gonome_dmg_one_bite);

	// ShockTrooper 
	REGISTER_SKILL_CVARS(shocktrooper_health);
	REGISTER_SKILL_CVARS(shocktrooper_kick);
	REGISTER_SKILL_CVARS(shocktrooper_gspeed);
	REGISTER_SKILL_CVARS(shocktrooper_maxcharge);
	REGISTER_SKILL_CVARS(shocktrooper_rchgspeed);

	//Turret
	REGISTER_SKILL_CVARS(turret_health);

	// MiniTurret
	REGISTER_SKILL_CVARS(miniturret_health);

	// Sentry Turret
	REGISTER_SKILL_CVARS(sentry_health);

	// PLAYER WEAPONS

	// Crowbar whack
	REGISTER_SKILL_CVARS(plr_crowbar);

	// Pipe Wrench
	REGISTER_SKILL_CVARS(plr_pipewrench);

	// Knife whack
	REGISTER_SKILL_CVARS(plr_knife);
	REGISTER_SKILL_CVARS(plr_knife_charge);

	// Glock Round
	REGISTER_SKILL_CVARS(plr_9mm_bullet);

	// 357 Round
	REGISTER_SKILL_CVARS(plr_357_bullet);

	// 762 Round
	REGISTER_SKILL_CVARS(plr_762_bullet);

	// MP5 Round
	REGISTER_SKILL_CVARS(plr_9mmAR_bullet);

	// M203 grenade
	REGISTER_SKILL_CVARS(plr_9mmAR_grenade);

	// Shotgun buckshot
	REGISTER_SKILL_CVARS(plr_buckshot);

	// Crossbow
	REGISTER_SKILL_CVARS(plr_xbow_bolt_monster);
	REGISTER_SKILL_CVARS(plr_xbow_bolt_client);

	// RPG
	REGISTER_SKILL_CVARS(plr_rpg);

	// Gauss Gun
	REGISTER_SKILL_CVARS(plr_gauss);

	// Egon Gun
	REGISTER_SKILL_CVARS(plr_egon_narrow);
	REGISTER_SKILL_CVARS(plr_egon_wide);

	// Hand Grendade
	REGISTER_SKILL_CVARS(plr_hand_grenade);

	// Satchel Charge
	REGISTER_SKILL_CVARS(plr_satchel);

	// Tripmine
	REGISTER_SKILL_CVARS(plr_tripmine);

	// 556 Player
	REGISTER_SKILL_CVARS(plr_556_bullet);

	// WORLD WEAPONS
	REGISTER_SKILL_CVARS(12mm_bullet);
	REGISTER_SKILL_CVARS(9mmAR_bullet);
	REGISTER_SKILL_CVARS(9mm_bullet);

	// 556
	REGISTER_SKILL_CVARS(556_bullet);

	// HORNET
	REGISTER_SKILL_CVARS(hornet_dmg);

	// SHOCKROACH
	REGISTER_SKILL_CVARS(plr_shockroachs);
	REGISTER_SKILL_CVARS(plr_shockroachm);

	// SPOREGRENADE
	REGISTER_SKILL_CVARS(plr_spore);

	// HEALTH/SUIT CHARGE DISTRIBUTION
	REGISTER_SKILL_CVARS(suitcharger);
	REGISTER_SKILL_CVARS(battery);
	REGISTER_SKILL_CVARS(healthcharger);
	REGISTER_SKILL_CVARS(healthkit);
	REGISTER_SKILL_CVARS(scientist_heal);
	REGISTER_SKILL_CVARS(flashcharge);

	// monster damage adjusters
	REGISTER_SKILL_CVARS(monster_head);
	REGISTER_SKILL_CVARS(monster_chest);
	REGISTER_SKILL_CVARS(monster_stomach);
	REGISTER_SKILL_CVARS(monster_arm);
	REGISTER_SKILL_CVARS(monster_leg);

	// player damage adjusters
	REGISTER_SKILL_CVARS(player_head);
	REGISTER_SKILL_CVARS(player_chest);
	REGISTER_SKILL_CVARS(player_stomach);
	REGISTER_SKILL_CVARS(player_arm);
	REGISTER_SKILL_CVARS(player_leg);

	// END REGISTER CVARS FOR SKILL LEVEL STUFF
	SERVER_COMMAND("exec skill.cfg\n");
	SERVER_COMMAND("exec skill_opfor.cfg\n"); // Opposing-Force
	SERVER_COMMAND("exec skill_hitgroups.cfg\n"); // Hitgroups
}