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
void GameDLLInit(void)
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
	CVAR_REGISTER(&sk_agrunt_health1);
	CVAR_REGISTER(&sk_agrunt_health2);
	CVAR_REGISTER(&sk_agrunt_health3);

	CVAR_REGISTER(&sk_agrunt_dmg_punch1);
	CVAR_REGISTER(&sk_agrunt_dmg_punch2);
	CVAR_REGISTER(&sk_agrunt_dmg_punch3);

	// Apache
	CVAR_REGISTER(&sk_apache_health1);
	CVAR_REGISTER(&sk_apache_health2);
	CVAR_REGISTER(&sk_apache_health3);

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
	CVAR_REGISTER(&sk_barney_health1);
	CVAR_REGISTER(&sk_barney_health2);
	CVAR_REGISTER(&sk_barney_health3);

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
	CVAR_REGISTER(&sk_bullsquid_health1);
	CVAR_REGISTER(&sk_bullsquid_health2);
	CVAR_REGISTER(&sk_bullsquid_health3);

	CVAR_REGISTER(&sk_bullsquid_dmg_bite1);
	CVAR_REGISTER(&sk_bullsquid_dmg_bite2);
	CVAR_REGISTER(&sk_bullsquid_dmg_bite3);

	CVAR_REGISTER(&sk_bullsquid_dmg_whip1);
	CVAR_REGISTER(&sk_bullsquid_dmg_whip2);
	CVAR_REGISTER(&sk_bullsquid_dmg_whip3);

	CVAR_REGISTER(&sk_bullsquid_dmg_spit1);
	CVAR_REGISTER(&sk_bullsquid_dmg_spit2);
	CVAR_REGISTER(&sk_bullsquid_dmg_spit3);

	// Bigmomma
	CVAR_REGISTER(&sk_bigmomma_health_factor1);
	CVAR_REGISTER(&sk_bigmomma_health_factor2);
	CVAR_REGISTER(&sk_bigmomma_health_factor3);

	CVAR_REGISTER(&sk_bigmomma_dmg_slash1);
	CVAR_REGISTER(&sk_bigmomma_dmg_slash2);
	CVAR_REGISTER(&sk_bigmomma_dmg_slash3);

	CVAR_REGISTER(&sk_bigmomma_dmg_blast1);
	CVAR_REGISTER(&sk_bigmomma_dmg_blast2);
	CVAR_REGISTER(&sk_bigmomma_dmg_blast3);

	CVAR_REGISTER(&sk_bigmomma_radius_blast1);
	CVAR_REGISTER(&sk_bigmomma_radius_blast2);
	CVAR_REGISTER(&sk_bigmomma_radius_blast3);

	// Gargantua
	CVAR_REGISTER(&sk_gargantua_health1);
	CVAR_REGISTER(&sk_gargantua_health2);
	CVAR_REGISTER(&sk_gargantua_health3);

	CVAR_REGISTER(&sk_gargantua_dmg_slash1);
	CVAR_REGISTER(&sk_gargantua_dmg_slash2);
	CVAR_REGISTER(&sk_gargantua_dmg_slash3);

	CVAR_REGISTER(&sk_gargantua_dmg_fire1);
	CVAR_REGISTER(&sk_gargantua_dmg_fire2);
	CVAR_REGISTER(&sk_gargantua_dmg_fire3);

	CVAR_REGISTER(&sk_gargantua_dmg_stomp1);
	CVAR_REGISTER(&sk_gargantua_dmg_stomp2);
	CVAR_REGISTER(&sk_gargantua_dmg_stomp3);

	// Hassassin
	CVAR_REGISTER(&sk_hassassin_health1);
	CVAR_REGISTER(&sk_hassassin_health2);
	CVAR_REGISTER(&sk_hassassin_health3);

	// Headcrab
	CVAR_REGISTER(&sk_headcrab_health1);
	CVAR_REGISTER(&sk_headcrab_health2);
	CVAR_REGISTER(&sk_headcrab_health3);

	CVAR_REGISTER(&sk_headcrab_dmg_bite1);
	CVAR_REGISTER(&sk_headcrab_dmg_bite2);
	CVAR_REGISTER(&sk_headcrab_dmg_bite3);

	// Hgrunt 
	CVAR_REGISTER(&sk_hgrunt_health1);
	CVAR_REGISTER(&sk_hgrunt_health2);
	CVAR_REGISTER(&sk_hgrunt_health3);

	CVAR_REGISTER(&sk_hgrunt_kick1);
	CVAR_REGISTER(&sk_hgrunt_kick2);
	CVAR_REGISTER(&sk_hgrunt_kick3);

	CVAR_REGISTER(&sk_hgrunt_pellets1);
	CVAR_REGISTER(&sk_hgrunt_pellets2);
	CVAR_REGISTER(&sk_hgrunt_pellets3);

	CVAR_REGISTER(&sk_hgrunt_gspeed1);
	CVAR_REGISTER(&sk_hgrunt_gspeed2);
	CVAR_REGISTER(&sk_hgrunt_gspeed3);

	// Houndeye
	CVAR_REGISTER(&sk_houndeye_health1);
	CVAR_REGISTER(&sk_houndeye_health2);
	CVAR_REGISTER(&sk_houndeye_health3);

	CVAR_REGISTER(&sk_houndeye_dmg_blast1);
	CVAR_REGISTER(&sk_houndeye_dmg_blast2);
	CVAR_REGISTER(&sk_houndeye_dmg_blast3);

	// ISlave
	CVAR_REGISTER(&sk_islave_health1);
	CVAR_REGISTER(&sk_islave_health2);
	CVAR_REGISTER(&sk_islave_health3);

	CVAR_REGISTER(&sk_islave_dmg_claw1);
	CVAR_REGISTER(&sk_islave_dmg_claw2);
	CVAR_REGISTER(&sk_islave_dmg_claw3);

	CVAR_REGISTER(&sk_islave_dmg_clawrake1);
	CVAR_REGISTER(&sk_islave_dmg_clawrake2);
	CVAR_REGISTER(&sk_islave_dmg_clawrake3);

	CVAR_REGISTER(&sk_islave_dmg_zap1);
	CVAR_REGISTER(&sk_islave_dmg_zap2);
	CVAR_REGISTER(&sk_islave_dmg_zap3);

	// Icthyosaur
	CVAR_REGISTER(&sk_ichthyosaur_health1);
	CVAR_REGISTER(&sk_ichthyosaur_health2);
	CVAR_REGISTER(&sk_ichthyosaur_health3);

	CVAR_REGISTER(&sk_ichthyosaur_shake1);
	CVAR_REGISTER(&sk_ichthyosaur_shake2);
	CVAR_REGISTER(&sk_ichthyosaur_shake3);

	// Leech
	CVAR_REGISTER(&sk_leech_health1);
	CVAR_REGISTER(&sk_leech_health2);
	CVAR_REGISTER(&sk_leech_health3);

	CVAR_REGISTER(&sk_leech_dmg_bite1);
	CVAR_REGISTER(&sk_leech_dmg_bite2);
	CVAR_REGISTER(&sk_leech_dmg_bite3);

	// Controller
	CVAR_REGISTER(&sk_controller_health1);
	CVAR_REGISTER(&sk_controller_health2);
	CVAR_REGISTER(&sk_controller_health3);

	CVAR_REGISTER(&sk_controller_dmgzap1);
	CVAR_REGISTER(&sk_controller_dmgzap2);
	CVAR_REGISTER(&sk_controller_dmgzap3);

	CVAR_REGISTER(&sk_controller_speedball1);
	CVAR_REGISTER(&sk_controller_speedball2);
	CVAR_REGISTER(&sk_controller_speedball3);

	CVAR_REGISTER(&sk_controller_dmgball1);
	CVAR_REGISTER(&sk_controller_dmgball2);
	CVAR_REGISTER(&sk_controller_dmgball3);

	// Nihilanth
	CVAR_REGISTER(&sk_nihilanth_health1);
	CVAR_REGISTER(&sk_nihilanth_health2);
	CVAR_REGISTER(&sk_nihilanth_health3);

	CVAR_REGISTER(&sk_nihilanth_zap1);
	CVAR_REGISTER(&sk_nihilanth_zap2);
	CVAR_REGISTER(&sk_nihilanth_zap3);

	// Scientist
	CVAR_REGISTER(&sk_scientist_health1);
	CVAR_REGISTER(&sk_scientist_health2);
	CVAR_REGISTER(&sk_scientist_health3);

	// Construction
	CVAR_REGISTER(&sk_construction_health1);
	CVAR_REGISTER(&sk_construction_health2);
	CVAR_REGISTER(&sk_construction_health3);


	CVAR_REGISTER(&sk_construction_head1);
	CVAR_REGISTER(&sk_construction_head2);
	CVAR_REGISTER(&sk_construction_head3);

	CVAR_REGISTER(&sk_construction_chest1);
	CVAR_REGISTER(&sk_construction_chest2);
	CVAR_REGISTER(&sk_construction_chest3);

	CVAR_REGISTER(&sk_construction_stomach1);
	CVAR_REGISTER(&sk_construction_stomach2);
	CVAR_REGISTER(&sk_construction_stomach3);

	CVAR_REGISTER(&sk_construction_leg1);
	CVAR_REGISTER(&sk_construction_leg2);
	CVAR_REGISTER(&sk_construction_leg3);

	CVAR_REGISTER(&sk_construction_arm1);
	CVAR_REGISTER(&sk_construction_arm2);
	CVAR_REGISTER(&sk_construction_arm3);

	// Snark
	CVAR_REGISTER(&sk_snark_health1);
	CVAR_REGISTER(&sk_snark_health2);
	CVAR_REGISTER(&sk_snark_health3);

	CVAR_REGISTER(&sk_snark_dmg_bite1);
	CVAR_REGISTER(&sk_snark_dmg_bite2);
	CVAR_REGISTER(&sk_snark_dmg_bite3);

	CVAR_REGISTER(&sk_snark_dmg_pop1);
	CVAR_REGISTER(&sk_snark_dmg_pop2);
	CVAR_REGISTER(&sk_snark_dmg_pop3);

	//=========================================================
	// NPCs: Diablo
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	CVAR_REGISTER(&sk_diablo_health1);
	CVAR_REGISTER(&sk_diablo_health2);
	CVAR_REGISTER(&sk_diablo_health3);

	CVAR_REGISTER(&sk_diablo_dmg_one_slash1);
	CVAR_REGISTER(&sk_diablo_dmg_one_slash2);
	CVAR_REGISTER(&sk_diablo_dmg_one_slash3);

	CVAR_REGISTER(&sk_diablo_dmg_both_slash1);
	CVAR_REGISTER(&sk_diablo_dmg_both_slash2);
	CVAR_REGISTER(&sk_diablo_dmg_both_slash3);

	CVAR_REGISTER(&sk_diablo_head1);
	CVAR_REGISTER(&sk_diablo_head2);
	CVAR_REGISTER(&sk_diablo_head3);

	CVAR_REGISTER(&sk_diablo_chest1);
	CVAR_REGISTER(&sk_diablo_chest2);
	CVAR_REGISTER(&sk_diablo_chest3);

	CVAR_REGISTER(&sk_diablo_stomach1);
	CVAR_REGISTER(&sk_diablo_stomach2);
	CVAR_REGISTER(&sk_diablo_stomach3);

	CVAR_REGISTER(&sk_diablo_leg1);
	CVAR_REGISTER(&sk_diablo_leg2);
	CVAR_REGISTER(&sk_diablo_leg3);

	//=========================================================
	// NPCs: Zombie,Zombie Soldier,Zombie Barney
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================

	// Zombie Scientist
	CVAR_REGISTER(&sk_zombie_health1);
	CVAR_REGISTER(&sk_zombie_health2);
	CVAR_REGISTER(&sk_zombie_health3);

	CVAR_REGISTER(&sk_zombie_dmg_one_slash1);
	CVAR_REGISTER(&sk_zombie_dmg_one_slash2);
	CVAR_REGISTER(&sk_zombie_dmg_one_slash3);

	CVAR_REGISTER(&sk_zombie_dmg_both_slash1);
	CVAR_REGISTER(&sk_zombie_dmg_both_slash2);
	CVAR_REGISTER(&sk_zombie_dmg_both_slash3);

	CVAR_REGISTER(&sk_zombie_head1);
	CVAR_REGISTER(&sk_zombie_head2);
	CVAR_REGISTER(&sk_zombie_head3);

	CVAR_REGISTER(&sk_zombie_chest1);
	CVAR_REGISTER(&sk_zombie_chest2);
	CVAR_REGISTER(&sk_zombie_chest3);

	CVAR_REGISTER(&sk_zombie_stomach1);
	CVAR_REGISTER(&sk_zombie_stomach2);
	CVAR_REGISTER(&sk_zombie_stomach3);

	CVAR_REGISTER(&sk_zombie_leg1);
	CVAR_REGISTER(&sk_zombie_leg2);
	CVAR_REGISTER(&sk_zombie_leg3);

	CVAR_REGISTER(&sk_zombie_arm1);
	CVAR_REGISTER(&sk_zombie_arm2);
	CVAR_REGISTER(&sk_zombie_arm3);

	// Zombie Soldier
	CVAR_REGISTER(&sk_zombie_soldier_health1);
	CVAR_REGISTER(&sk_zombie_soldier_health2);
	CVAR_REGISTER(&sk_zombie_soldier_health3);

	CVAR_REGISTER(&sk_zombie_soldier_dmg_one_slash1);
	CVAR_REGISTER(&sk_zombie_soldier_dmg_one_slash2);
	CVAR_REGISTER(&sk_zombie_soldier_dmg_one_slash3);

	CVAR_REGISTER(&sk_zombie_soldier_dmg_both_slash1);
	CVAR_REGISTER(&sk_zombie_soldier_dmg_both_slash2);
	CVAR_REGISTER(&sk_zombie_soldier_dmg_both_slash3);

	CVAR_REGISTER(&sk_zombie_soldier_head1);
	CVAR_REGISTER(&sk_zombie_soldier_head2);
	CVAR_REGISTER(&sk_zombie_soldier_head3);

	CVAR_REGISTER(&sk_zombie_soldier_chest1);
	CVAR_REGISTER(&sk_zombie_soldier_chest2);
	CVAR_REGISTER(&sk_zombie_soldier_chest3);

	CVAR_REGISTER(&sk_zombie_soldier_stomach1);
	CVAR_REGISTER(&sk_zombie_soldier_stomach2);
	CVAR_REGISTER(&sk_zombie_soldier_stomach3);

	CVAR_REGISTER(&sk_zombie_soldier_leg1);
	CVAR_REGISTER(&sk_zombie_soldier_leg2);
	CVAR_REGISTER(&sk_zombie_soldier_leg3);

	CVAR_REGISTER(&sk_zombie_soldier_arm1);
	CVAR_REGISTER(&sk_zombie_soldier_arm2);
	CVAR_REGISTER(&sk_zombie_soldier_arm3);

	// Zombie Construction
	CVAR_REGISTER(&sk_zombie_construction_health1);
	CVAR_REGISTER(&sk_zombie_construction_health2);
	CVAR_REGISTER(&sk_zombie_construction_health3);

	CVAR_REGISTER(&sk_zombie_construction_dmg_one_slash1);
	CVAR_REGISTER(&sk_zombie_construction_dmg_one_slash2);
	CVAR_REGISTER(&sk_zombie_construction_dmg_one_slash3);

	CVAR_REGISTER(&sk_zombie_construction_dmg_both_slash1);
	CVAR_REGISTER(&sk_zombie_construction_dmg_both_slash2);
	CVAR_REGISTER(&sk_zombie_construction_dmg_both_slash3);

	CVAR_REGISTER(&sk_zombie_construction_head1);
	CVAR_REGISTER(&sk_zombie_construction_head2);
	CVAR_REGISTER(&sk_zombie_construction_head3);

	CVAR_REGISTER(&sk_zombie_construction_chest1);
	CVAR_REGISTER(&sk_zombie_construction_chest2);
	CVAR_REGISTER(&sk_zombie_construction_chest3);

	CVAR_REGISTER(&sk_zombie_construction_stomach1);
	CVAR_REGISTER(&sk_zombie_construction_stomach2);
	CVAR_REGISTER(&sk_zombie_construction_stomach3);

	CVAR_REGISTER(&sk_zombie_construction_leg1);
	CVAR_REGISTER(&sk_zombie_construction_leg2);
	CVAR_REGISTER(&sk_zombie_construction_leg3);

	CVAR_REGISTER(&sk_zombie_construction_arm1);
	CVAR_REGISTER(&sk_zombie_construction_arm2);
	CVAR_REGISTER(&sk_zombie_construction_arm3);

	// Zombie Barney
	CVAR_REGISTER(&sk_zombie_barney_health1);
	CVAR_REGISTER(&sk_zombie_barney_health2);
	CVAR_REGISTER(&sk_zombie_barney_health3);

	CVAR_REGISTER(&sk_zombie_barney_dmg_one_slash1);
	CVAR_REGISTER(&sk_zombie_barney_dmg_one_slash2);
	CVAR_REGISTER(&sk_zombie_barney_dmg_one_slash3);

	CVAR_REGISTER(&sk_zombie_barney_dmg_both_slash1);
	CVAR_REGISTER(&sk_zombie_barney_dmg_both_slash2);
	CVAR_REGISTER(&sk_zombie_barney_dmg_both_slash3);

	CVAR_REGISTER(&sk_zombie_barney_head1);
	CVAR_REGISTER(&sk_zombie_barney_head2);
	CVAR_REGISTER(&sk_zombie_barney_head3);

	CVAR_REGISTER(&sk_zombie_barney_chest1);
	CVAR_REGISTER(&sk_zombie_barney_chest2);
	CVAR_REGISTER(&sk_zombie_barney_chest3);

	CVAR_REGISTER(&sk_zombie_barney_stomach1);
	CVAR_REGISTER(&sk_zombie_barney_stomach2);
	CVAR_REGISTER(&sk_zombie_barney_stomach3);

	CVAR_REGISTER(&sk_zombie_barney_leg1);
	CVAR_REGISTER(&sk_zombie_barney_leg2);
	CVAR_REGISTER(&sk_zombie_barney_leg3);

	CVAR_REGISTER(&sk_zombie_barney_arm1);
	CVAR_REGISTER(&sk_zombie_barney_arm2);
	CVAR_REGISTER(&sk_zombie_barney_arm3);

	//=========================================================
	// NPCs: Pit Drone
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	CVAR_REGISTER(&sk_pitdrone_health1);
	CVAR_REGISTER(&sk_pitdrone_health2);
	CVAR_REGISTER(&sk_pitdrone_health3);

	CVAR_REGISTER(&sk_pitdrone_dmg_bite1);
	CVAR_REGISTER(&sk_pitdrone_dmg_bite2);
	CVAR_REGISTER(&sk_pitdrone_dmg_bite3);

	CVAR_REGISTER(&sk_pitdrone_dmg_whip1);
	CVAR_REGISTER(&sk_pitdrone_dmg_whip2);
	CVAR_REGISTER(&sk_pitdrone_dmg_whip3);

	CVAR_REGISTER(&sk_pitdrone_dmg_spit1);
	CVAR_REGISTER(&sk_pitdrone_dmg_spit2);
	CVAR_REGISTER(&sk_pitdrone_dmg_spit3);

	CVAR_REGISTER(&sk_pitdrone_head1);
	CVAR_REGISTER(&sk_pitdrone_head2);
	CVAR_REGISTER(&sk_pitdrone_head3);

	CVAR_REGISTER(&sk_pitdrone_chest1);
	CVAR_REGISTER(&sk_pitdrone_chest2);
	CVAR_REGISTER(&sk_pitdrone_chest3);

	CVAR_REGISTER(&sk_pitdrone_stomach1);
	CVAR_REGISTER(&sk_pitdrone_stomach2);
	CVAR_REGISTER(&sk_pitdrone_stomach3);

	CVAR_REGISTER(&sk_pitdrone_leg1);
	CVAR_REGISTER(&sk_pitdrone_leg2);
	CVAR_REGISTER(&sk_pitdrone_leg3);

	CVAR_REGISTER(&sk_pitdrone_arm1);
	CVAR_REGISTER(&sk_pitdrone_arm2);
	CVAR_REGISTER(&sk_pitdrone_arm3);

	//=========================================================
	// NPCs: Voltigore
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	CVAR_REGISTER(&sk_voltigore_health1);
	CVAR_REGISTER(&sk_voltigore_health2);
	CVAR_REGISTER(&sk_voltigore_health3);

	CVAR_REGISTER(&sk_voltigore_dmg_punch1);
	CVAR_REGISTER(&sk_voltigore_dmg_punch2);
	CVAR_REGISTER(&sk_voltigore_dmg_punch3);

	CVAR_REGISTER(&sk_voltigore_dmg_beam1);
	CVAR_REGISTER(&sk_voltigore_dmg_beam2);
	CVAR_REGISTER(&sk_voltigore_dmg_beam3);

	CVAR_REGISTER(&sk_voltigore_head1);
	CVAR_REGISTER(&sk_voltigore_head2);
	CVAR_REGISTER(&sk_voltigore_head3);

	CVAR_REGISTER(&sk_voltigore_chest1);
	CVAR_REGISTER(&sk_voltigore_chest2);
	CVAR_REGISTER(&sk_voltigore_chest3);

	CVAR_REGISTER(&sk_voltigore_stomach1);
	CVAR_REGISTER(&sk_voltigore_stomach2);
	CVAR_REGISTER(&sk_voltigore_stomach3);

	CVAR_REGISTER(&sk_voltigore_leg1);
	CVAR_REGISTER(&sk_voltigore_leg2);
	CVAR_REGISTER(&sk_voltigore_leg3);

	CVAR_REGISTER(&sk_voltigore_arm1);
	CVAR_REGISTER(&sk_voltigore_arm2);
	CVAR_REGISTER(&sk_voltigore_arm3);

	//=========================================================
	// NPCs: #########################
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================

	// Opposing-Force
	// Ally Grunt
	CVAR_REGISTER(&sk_hgrunt_ally_health1);
	CVAR_REGISTER(&sk_hgrunt_ally_health2);
	CVAR_REGISTER(&sk_hgrunt_ally_health3);

	CVAR_REGISTER(&sk_hgrunt_ally_kick1);
	CVAR_REGISTER(&sk_hgrunt_ally_kick2);
	CVAR_REGISTER(&sk_hgrunt_ally_kick3);

	CVAR_REGISTER(&sk_hgrunt_ally_pellets1);
	CVAR_REGISTER(&sk_hgrunt_ally_pellets2);
	CVAR_REGISTER(&sk_hgrunt_ally_pellets3);

	CVAR_REGISTER(&sk_hgrunt_ally_gspeed1);
	CVAR_REGISTER(&sk_hgrunt_ally_gspeed2);
	CVAR_REGISTER(&sk_hgrunt_ally_gspeed3);

	CVAR_REGISTER(&sk_hgrunt_ally_head1);
	CVAR_REGISTER(&sk_hgrunt_ally_head2);
	CVAR_REGISTER(&sk_hgrunt_ally_head3);

	CVAR_REGISTER(&sk_hgrunt_ally_chest1);
	CVAR_REGISTER(&sk_hgrunt_ally_chest2);
	CVAR_REGISTER(&sk_hgrunt_ally_chest3);

	CVAR_REGISTER(&sk_hgrunt_ally_stomach1);
	CVAR_REGISTER(&sk_hgrunt_ally_stomach2);
	CVAR_REGISTER(&sk_hgrunt_ally_stomach3);

	CVAR_REGISTER(&sk_hgrunt_ally_leg1);
	CVAR_REGISTER(&sk_hgrunt_ally_leg2);
	CVAR_REGISTER(&sk_hgrunt_ally_leg3);

	CVAR_REGISTER(&sk_hgrunt_ally_arm1);
	CVAR_REGISTER(&sk_hgrunt_ally_arm2);
	CVAR_REGISTER(&sk_hgrunt_ally_arm3);

	// Ally Grunt Medic
	CVAR_REGISTER(&sk_medic_ally_health1);
	CVAR_REGISTER(&sk_medic_ally_health2);
	CVAR_REGISTER(&sk_medic_ally_health3);

	CVAR_REGISTER(&sk_medic_ally_kick1);
	CVAR_REGISTER(&sk_medic_ally_kick2);
	CVAR_REGISTER(&sk_medic_ally_kick3);

	CVAR_REGISTER(&sk_medic_ally_heal1);
	CVAR_REGISTER(&sk_medic_ally_heal2);
	CVAR_REGISTER(&sk_medic_ally_heal3);

	// Ally Grunt Torch 
	CVAR_REGISTER(&sk_torch_ally_health1);
	CVAR_REGISTER(&sk_torch_ally_health2);
	CVAR_REGISTER(&sk_torch_ally_health3);

	CVAR_REGISTER(&sk_torch_ally_kick1);
	CVAR_REGISTER(&sk_torch_ally_kick2);
	CVAR_REGISTER(&sk_torch_ally_kick3);

	// Pitworm
	CVAR_REGISTER(&sk_pitworm_health1);
	CVAR_REGISTER(&sk_pitworm_health2);
	CVAR_REGISTER(&sk_pitworm_health3);

	CVAR_REGISTER(&sk_pitworm_dmg_swipe1);
	CVAR_REGISTER(&sk_pitworm_dmg_swipe2);
	CVAR_REGISTER(&sk_pitworm_dmg_swipe3);

	CVAR_REGISTER(&sk_pitworm_dmg_beam1);
	CVAR_REGISTER(&sk_pitworm_dmg_beam2);
	CVAR_REGISTER(&sk_pitworm_dmg_beam3);

	// Shock Roach
	CVAR_REGISTER(&sk_shockroach_health1);
	CVAR_REGISTER(&sk_shockroach_health2);
	CVAR_REGISTER(&sk_shockroach_health3);

	CVAR_REGISTER(&sk_shockroach_dmg_bite1);
	CVAR_REGISTER(&sk_shockroach_dmg_bite2);
	CVAR_REGISTER(&sk_shockroach_dmg_bite3);

	CVAR_REGISTER(&sk_shockroach_lifespan1);
	CVAR_REGISTER(&sk_shockroach_lifespan2);
	CVAR_REGISTER(&sk_shockroach_lifespan3);

	// Gonome
	CVAR_REGISTER(&sk_gonome_health1);
	CVAR_REGISTER(&sk_gonome_health2);
	CVAR_REGISTER(&sk_gonome_health3);

	CVAR_REGISTER(&sk_gonome_dmg_one_slash1);
	CVAR_REGISTER(&sk_gonome_dmg_one_slash2);
	CVAR_REGISTER(&sk_gonome_dmg_one_slash3);

	CVAR_REGISTER(&sk_gonome_dmg_guts1);
	CVAR_REGISTER(&sk_gonome_dmg_guts2);
	CVAR_REGISTER(&sk_gonome_dmg_guts3);

	CVAR_REGISTER(&sk_gonome_dmg_one_bite1);
	CVAR_REGISTER(&sk_gonome_dmg_one_bite2);
	CVAR_REGISTER(&sk_gonome_dmg_one_bite3);

	// ShockTrooper 
	CVAR_REGISTER(&sk_shocktrooper_health1);
	CVAR_REGISTER(&sk_shocktrooper_health2);
	CVAR_REGISTER(&sk_shocktrooper_health3);

	CVAR_REGISTER(&sk_shocktrooper_kick1);
	CVAR_REGISTER(&sk_shocktrooper_kick2);
	CVAR_REGISTER(&sk_shocktrooper_kick3);

	CVAR_REGISTER(&sk_shocktrooper_gspeed1);
	CVAR_REGISTER(&sk_shocktrooper_gspeed2);
	CVAR_REGISTER(&sk_shocktrooper_gspeed3);

	CVAR_REGISTER(&sk_shocktrooper_maxcharge1);
	CVAR_REGISTER(&sk_shocktrooper_maxcharge2);
	CVAR_REGISTER(&sk_shocktrooper_maxcharge3);

	CVAR_REGISTER(&sk_shocktrooper_rchgspeed1);
	CVAR_REGISTER(&sk_shocktrooper_rchgspeed2);
	CVAR_REGISTER(&sk_shocktrooper_rchgspeed3);

	//Turret
	CVAR_REGISTER(&sk_turret_health1);
	CVAR_REGISTER(&sk_turret_health2);
	CVAR_REGISTER(&sk_turret_health3);

	// MiniTurret
	CVAR_REGISTER(&sk_miniturret_health1);
	CVAR_REGISTER(&sk_miniturret_health2);
	CVAR_REGISTER(&sk_miniturret_health3);

	// Sentry Turret
	CVAR_REGISTER(&sk_sentry_health1);
	CVAR_REGISTER(&sk_sentry_health2);
	CVAR_REGISTER(&sk_sentry_health3);

	// PLAYER WEAPONS

	// Crowbar whack
	CVAR_REGISTER(&sk_plr_crowbar1);
	CVAR_REGISTER(&sk_plr_crowbar2);
	CVAR_REGISTER(&sk_plr_crowbar3);

	// Pipe Wrench
	CVAR_REGISTER(&sk_plr_pipewrench1);
	CVAR_REGISTER(&sk_plr_pipewrench2);
	CVAR_REGISTER(&sk_plr_pipewrench3);

	// Knife whack
	CVAR_REGISTER(&sk_plr_knife1);
	CVAR_REGISTER(&sk_plr_knife2);
	CVAR_REGISTER(&sk_plr_knife3);

	CVAR_REGISTER(&sk_plr_knife_charge1);
	CVAR_REGISTER(&sk_plr_knife_charge2);
	CVAR_REGISTER(&sk_plr_knife_charge3);

	// Glock Round
	CVAR_REGISTER(&sk_plr_9mm_bullet1);
	CVAR_REGISTER(&sk_plr_9mm_bullet2);
	CVAR_REGISTER(&sk_plr_9mm_bullet3);

	// 357 Round
	CVAR_REGISTER(&sk_plr_357_bullet1);
	CVAR_REGISTER(&sk_plr_357_bullet2);
	CVAR_REGISTER(&sk_plr_357_bullet3);

	// 762 Round
	CVAR_REGISTER(&sk_plr_762_bullet1);
	CVAR_REGISTER(&sk_plr_762_bullet2);
	CVAR_REGISTER(&sk_plr_762_bullet3);

	// MP5 Round
	CVAR_REGISTER(&sk_plr_9mmAR_bullet1);
	CVAR_REGISTER(&sk_plr_9mmAR_bullet2);
	CVAR_REGISTER(&sk_plr_9mmAR_bullet3);

	// M203 grenade
	CVAR_REGISTER(&sk_plr_9mmAR_grenade1);
	CVAR_REGISTER(&sk_plr_9mmAR_grenade2);
	CVAR_REGISTER(&sk_plr_9mmAR_grenade3);

	// Shotgun buckshot
	CVAR_REGISTER(&sk_plr_buckshot1);
	CVAR_REGISTER(&sk_plr_buckshot2);
	CVAR_REGISTER(&sk_plr_buckshot3);

	// Crossbow
	CVAR_REGISTER(&sk_plr_xbow_bolt_monster1);
	CVAR_REGISTER(&sk_plr_xbow_bolt_monster2);
	CVAR_REGISTER(&sk_plr_xbow_bolt_monster3);

	CVAR_REGISTER(&sk_plr_xbow_bolt_client1);
	CVAR_REGISTER(&sk_plr_xbow_bolt_client2);
	CVAR_REGISTER(&sk_plr_xbow_bolt_client3);

	// RPG
	CVAR_REGISTER(&sk_plr_rpg1);
	CVAR_REGISTER(&sk_plr_rpg2);
	CVAR_REGISTER(&sk_plr_rpg3);

	// Gauss Gun
	CVAR_REGISTER(&sk_plr_gauss1);
	CVAR_REGISTER(&sk_plr_gauss2);
	CVAR_REGISTER(&sk_plr_gauss3);

	// Egon Gun
	CVAR_REGISTER(&sk_plr_egon_narrow1);
	CVAR_REGISTER(&sk_plr_egon_narrow2);
	CVAR_REGISTER(&sk_plr_egon_narrow3);

	CVAR_REGISTER(&sk_plr_egon_wide1);
	CVAR_REGISTER(&sk_plr_egon_wide2);
	CVAR_REGISTER(&sk_plr_egon_wide3);

	// Hand Grendade
	CVAR_REGISTER(&sk_plr_hand_grenade1);
	CVAR_REGISTER(&sk_plr_hand_grenade2);
	CVAR_REGISTER(&sk_plr_hand_grenade3);

	// Satchel Charge
	CVAR_REGISTER(&sk_plr_satchel1);
	CVAR_REGISTER(&sk_plr_satchel2);
	CVAR_REGISTER(&sk_plr_satchel3);

	// Tripmine
	CVAR_REGISTER(&sk_plr_tripmine1);
	CVAR_REGISTER(&sk_plr_tripmine2);
	CVAR_REGISTER(&sk_plr_tripmine3);

	// 556
	CVAR_REGISTER(&sk_plr_556_bullet1);
	CVAR_REGISTER(&sk_plr_556_bullet2);
	CVAR_REGISTER(&sk_plr_556_bullet3);

	// WORLD WEAPONS
	CVAR_REGISTER(&sk_12mm_bullet1);
	CVAR_REGISTER(&sk_12mm_bullet2);
	CVAR_REGISTER(&sk_12mm_bullet3);

	CVAR_REGISTER(&sk_9mmAR_bullet1);
	CVAR_REGISTER(&sk_9mmAR_bullet2);
	CVAR_REGISTER(&sk_9mmAR_bullet3);

	CVAR_REGISTER(&sk_9mm_bullet1);
	CVAR_REGISTER(&sk_9mm_bullet2);
	CVAR_REGISTER(&sk_9mm_bullet3);

	// 556
	CVAR_REGISTER(&sk_556_bullet1);
	CVAR_REGISTER(&sk_556_bullet2);
	CVAR_REGISTER(&sk_556_bullet3);

	// HORNET
	CVAR_REGISTER(&sk_hornet_dmg1);
	CVAR_REGISTER(&sk_hornet_dmg2);
	CVAR_REGISTER(&sk_hornet_dmg3);

	// SHOCKROACH
	CVAR_REGISTER(&sk_plr_shockroachs1);
	CVAR_REGISTER(&sk_plr_shockroachs2);
	CVAR_REGISTER(&sk_plr_shockroachs3);

	CVAR_REGISTER(&sk_plr_shockroachm1);
	CVAR_REGISTER(&sk_plr_shockroachm2);
	CVAR_REGISTER(&sk_plr_shockroachm3);

	// SPOREGRENADE
	CVAR_REGISTER(&sk_plr_spore1);
	CVAR_REGISTER(&sk_plr_spore2);
	CVAR_REGISTER(&sk_plr_spore3);

	// HEALTH/SUIT CHARGE DISTRIBUTION
	CVAR_REGISTER(&sk_suitcharger1);
	CVAR_REGISTER(&sk_suitcharger2);
	CVAR_REGISTER(&sk_suitcharger3);

	CVAR_REGISTER(&sk_battery1);
	CVAR_REGISTER(&sk_battery2);
	CVAR_REGISTER(&sk_battery3);

	CVAR_REGISTER(&sk_healthcharger1);
	CVAR_REGISTER(&sk_healthcharger2);
	CVAR_REGISTER(&sk_healthcharger3);

	CVAR_REGISTER(&sk_healthkit1);
	CVAR_REGISTER(&sk_healthkit2);
	CVAR_REGISTER(&sk_healthkit3);

	CVAR_REGISTER(&sk_scientist_heal1);
	CVAR_REGISTER(&sk_scientist_heal2);
	CVAR_REGISTER(&sk_scientist_heal3);

	CVAR_REGISTER(&sk_flashcharge1);
	CVAR_REGISTER(&sk_flashcharge2);
	CVAR_REGISTER(&sk_flashcharge3);

	// monster damage adjusters
	CVAR_REGISTER(&sk_monster_head1);
	CVAR_REGISTER(&sk_monster_head2);
	CVAR_REGISTER(&sk_monster_head3);

	CVAR_REGISTER(&sk_monster_chest1);
	CVAR_REGISTER(&sk_monster_chest2);
	CVAR_REGISTER(&sk_monster_chest3);

	CVAR_REGISTER(&sk_monster_stomach1);
	CVAR_REGISTER(&sk_monster_stomach2);
	CVAR_REGISTER(&sk_monster_stomach3);

	CVAR_REGISTER(&sk_monster_arm1);
	CVAR_REGISTER(&sk_monster_arm2);
	CVAR_REGISTER(&sk_monster_arm3);

	CVAR_REGISTER(&sk_monster_leg1);
	CVAR_REGISTER(&sk_monster_leg2);
	CVAR_REGISTER(&sk_monster_leg3);

	// player damage adjusters
	CVAR_REGISTER(&sk_player_head1);
	CVAR_REGISTER(&sk_player_head2);
	CVAR_REGISTER(&sk_player_head3);

	CVAR_REGISTER(&sk_player_chest1);
	CVAR_REGISTER(&sk_player_chest2);
	CVAR_REGISTER(&sk_player_chest3);

	CVAR_REGISTER(&sk_player_stomach1);
	CVAR_REGISTER(&sk_player_stomach2);
	CVAR_REGISTER(&sk_player_stomach3);

	CVAR_REGISTER(&sk_player_arm1);
	CVAR_REGISTER(&sk_player_arm2);
	CVAR_REGISTER(&sk_player_arm3);

	CVAR_REGISTER(&sk_player_leg1);
	CVAR_REGISTER(&sk_player_leg2);
	CVAR_REGISTER(&sk_player_leg3);

	// END REGISTER CVARS FOR SKILL LEVEL STUFF
	SERVER_COMMAND("exec skill.cfg\n");
	SERVER_COMMAND("exec skill_opfor.cfg\n"); // Opposing-Force
	SERVER_COMMAND("exec skill_hitgroups.cfg\n"); // Hitgroups
}

