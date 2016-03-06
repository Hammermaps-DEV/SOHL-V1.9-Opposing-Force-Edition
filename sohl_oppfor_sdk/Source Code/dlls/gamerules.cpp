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

//=========================================================
// GameRules.cpp
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"skill.h"
#include	"game.h"

extern edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer );

DLL_GLOBAL CGameRules*	g_pGameRules = NULL;
extern DLL_GLOBAL BOOL	g_fGameOver;
extern int gmsgDeathMsg;	// client dll messages
extern int gmsgMOTD;

int g_teamplay = 0;

//=========================================================
//=========================================================
BOOL CGameRules::CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry )
{
	int iAmmoIndex;

	if ( pszAmmoName )
	{
		iAmmoIndex = pPlayer->GetAmmoIndex( pszAmmoName );

		if ( iAmmoIndex > -1 )
		{
			if ( pPlayer->AmmoInventory( iAmmoIndex ) < iMaxCarry )
			{
				// player has room for more of this type of ammo
				return TRUE;
			}
		}
	}

	return FALSE;
}

//=========================================================
//=========================================================
edict_t *CGameRules :: GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	edict_t *pentSpawnSpot = EntSelectSpawnPoint( pPlayer );

	pPlayer->pev->origin = VARS(pentSpawnSpot)->origin + Vector(0,0,1);
	pPlayer->pev->v_angle  = g_vecZero;
	pPlayer->pev->velocity = g_vecZero;
	pPlayer->pev->angles = VARS(pentSpawnSpot)->angles;
	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->pev->fixangle = TRUE;
	
	//LRC
	if (pentSpawnSpot->v.spawnflags & 1) // the START WITH SUIT flag
	{
		g_startSuit = TRUE;
	}
	
	return pentSpawnSpot;
}

//=========================================================
//=========================================================
BOOL CGameRules::CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
	// only living players can have items
	if ( pPlayer->pev->deadflag != DEAD_NO )
		return FALSE;

	if ( pWeapon->pszAmmo1() )
	{
		if ( !CanHaveAmmo( pPlayer, pWeapon->pszAmmo1(), pWeapon->iMaxAmmo1() ) )
		{
			// we can't carry anymore ammo for this gun. We can only 
			// have the gun if we aren't already carrying one of this type
			if ( pPlayer->HasPlayerItem( pWeapon ) )
			{
				return FALSE;
			}
		}
	}
	else
	{
		// weapon doesn't use ammo, don't take another if you already have it.
		if ( pPlayer->HasPlayerItem( pWeapon ) )
		{
			return FALSE;
		}
	}

	// note: will fall through to here if GetItemInfo doesn't fill the struct!
	return TRUE;
}

//=========================================================
// load the SkillData struct with the proper values based on the skill level.
//=========================================================
void CGameRules::RefreshSkillData ( void )
{
	int	iSkill;

	iSkill = (int)CVAR_GET_FLOAT("skill");
	g_iSkillLevel = iSkill;

	if ( iSkill < 1 )
	{
		iSkill = 1;
	}
	else if ( iSkill > 3 )
	{
		iSkill = 3; 
	}

	gSkillData.iSkillLevel = iSkill;

	ALERT ( at_debug, "\nGAME SKILL LEVEL:%d\n",iSkill );

	//Agrunt		
	gSkillData.agruntHealth = GetSkillCvar( "sk_agrunt_health" );
	gSkillData.agruntDmgPunch = GetSkillCvar( "sk_agrunt_dmg_punch");

	// Apache 
	gSkillData.apacheHealth = GetSkillCvar( "sk_apache_health");

	//=========================================================
	// NPCs: Barney
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.barneyHealth  = GetSkillCvar("sk_barney_health");
	gSkillData.barneyHead    = GetSkillCvar("sk_barney_head"); //Hitgroup
	gSkillData.barneyChest   = GetSkillCvar("sk_barney_chest"); //Hitgroup
	gSkillData.barneyStomach = GetSkillCvar("sk_barney_stomach"); //Hitgroup
	gSkillData.barneyLeg     = GetSkillCvar("sk_barney_leg"); //Hitgroup
	gSkillData.barneyArm     = GetSkillCvar("sk_barney_arm"); //Hitgroup

	//=========================================================
	// NPCs: Barniel
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.barnielHealth  = GetSkillCvar("sk_barniel_health");
	gSkillData.barnielHead    = GetSkillCvar("sk_barniel_head"); //Hitgroup
	gSkillData.barnielChest   = GetSkillCvar("sk_barniel_chest"); //Hitgroup
	gSkillData.barnielStomach = GetSkillCvar("sk_barniel_stomach"); //Hitgroup
	gSkillData.barnielLeg     = GetSkillCvar("sk_barniel_leg"); //Hitgroup
	gSkillData.barnielArm     = GetSkillCvar("sk_barniel_arm"); //Hitgroup

	//=========================================================
	// NPCs: Otis Laurey
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.otisHealth  = GetSkillCvar("sk_otis_health");
	gSkillData.otisHead    = GetSkillCvar("sk_otis_head"); //Hitgroup
	gSkillData.otisChest   = GetSkillCvar("sk_otis_chest"); //Hitgroup
	gSkillData.otisStomach = GetSkillCvar("sk_otis_stomach"); //Hitgroup
	gSkillData.otisLeg     = GetSkillCvar("sk_otis_leg"); //Hitgroup
	gSkillData.otisArm     = GetSkillCvar("sk_otis_arm"); //Hitgroup

	// Big Momma
	gSkillData.bigmommaHealthFactor = GetSkillCvar( "sk_bigmomma_health_factor" );
	gSkillData.bigmommaDmgSlash = GetSkillCvar( "sk_bigmomma_dmg_slash" );
	gSkillData.bigmommaDmgBlast = GetSkillCvar( "sk_bigmomma_dmg_blast" );
	gSkillData.bigmommaRadiusBlast = GetSkillCvar( "sk_bigmomma_radius_blast" );

	// Bullsquid
	gSkillData.bullsquidHealth = GetSkillCvar( "sk_bullsquid_health");
	gSkillData.bullsquidDmgBite = GetSkillCvar( "sk_bullsquid_dmg_bite");
	gSkillData.bullsquidDmgWhip = GetSkillCvar( "sk_bullsquid_dmg_whip");
	gSkillData.bullsquidDmgSpit = GetSkillCvar( "sk_bullsquid_dmg_spit");

	// Gargantua
	gSkillData.gargantuaHealth = GetSkillCvar( "sk_gargantua_health");
	gSkillData.gargantuaDmgSlash = GetSkillCvar( "sk_gargantua_dmg_slash");
	gSkillData.gargantuaDmgFire = GetSkillCvar( "sk_gargantua_dmg_fire");
	gSkillData.gargantuaDmgStomp = GetSkillCvar( "sk_gargantua_dmg_stomp");

	// Hassassin
	gSkillData.hassassinHealth = GetSkillCvar( "sk_hassassin_health");

	// Headcrab
	gSkillData.headcrabHealth = GetSkillCvar( "sk_headcrab_health");
	gSkillData.headcrabDmgBite = GetSkillCvar( "sk_headcrab_dmg_bite");

	// Hgrunt 
	gSkillData.hgruntHealth = GetSkillCvar( "sk_hgrunt_health");
	gSkillData.hgruntDmgKick = GetSkillCvar( "sk_hgrunt_kick");
	gSkillData.hgruntShotgunPellets = GetSkillCvar( "sk_hgrunt_pellets");
	gSkillData.hgruntGrenadeSpeed = GetSkillCvar( "sk_hgrunt_gspeed");

	// Houndeye
	gSkillData.houndeyeHealth = GetSkillCvar( "sk_houndeye_health");
	gSkillData.houndeyeDmgBlast = GetSkillCvar( "sk_houndeye_dmg_blast");

	// ISlave
	gSkillData.slaveHealth = GetSkillCvar( "sk_islave_health");
	gSkillData.slaveDmgClaw = GetSkillCvar( "sk_islave_dmg_claw");
	gSkillData.slaveDmgClawrake = GetSkillCvar( "sk_islave_dmg_clawrake");
	gSkillData.slaveDmgZap = GetSkillCvar( "sk_islave_dmg_zap");

	// Icthyosaur
	gSkillData.ichthyosaurHealth = GetSkillCvar( "sk_ichthyosaur_health");
	gSkillData.ichthyosaurDmgShake = GetSkillCvar( "sk_ichthyosaur_shake");

	// Leech
	gSkillData.leechHealth = GetSkillCvar( "sk_leech_health");

	gSkillData.leechDmgBite = GetSkillCvar( "sk_leech_dmg_bite");

	// Controller
	gSkillData.controllerHealth = GetSkillCvar( "sk_controller_health");
	gSkillData.controllerDmgZap = GetSkillCvar( "sk_controller_dmgzap");
	gSkillData.controllerSpeedBall = GetSkillCvar( "sk_controller_speedball");
	gSkillData.controllerDmgBall = GetSkillCvar( "sk_controller_dmgball");

	// Nihilanth
	gSkillData.nihilanthHealth = GetSkillCvar( "sk_nihilanth_health");
	gSkillData.nihilanthZap = GetSkillCvar( "sk_nihilanth_zap");

	// Scientist
	gSkillData.scientistHealth = GetSkillCvar( "sk_scientist_health");

	//=========================================================
	// NPCs: Construction
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.constructionHealth = GetSkillCvar("sk_construction_health");
	gSkillData.constructionHead = GetSkillCvar("sk_construction_head"); //Hitgroup
	gSkillData.constructionChest = GetSkillCvar("sk_construction_chest"); //Hitgroup
	gSkillData.constructionStomach = GetSkillCvar("sk_construction_stomach"); //Hitgroup
	gSkillData.constructionLeg = GetSkillCvar("sk_construction_leg"); //Hitgroup
	gSkillData.constructionArm = GetSkillCvar("sk_construction_arm"); //Hitgroup

	// Snark
	gSkillData.snarkHealth = GetSkillCvar( "sk_snark_health");
	gSkillData.snarkDmgBite = GetSkillCvar( "sk_snark_dmg_bite");
	gSkillData.snarkDmgPop = GetSkillCvar( "sk_snark_dmg_pop");

	//=========================================================
	// NPCs: Zombie Scientist
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.zombieHealth              = GetSkillCvar("sk_zombie_health");
	gSkillData.zombieDmgOneSlash         = GetSkillCvar("sk_zombie_dmg_one_slash");
	gSkillData.zombieDmgBothSlash        = GetSkillCvar("sk_zombie_dmg_both_slash");
	gSkillData.zombieHead                = GetSkillCvar("sk_zombie_head"); //Hitgroup
	gSkillData.zombieChest               = GetSkillCvar("sk_zombie_chest"); //Hitgroup
	gSkillData.zombieStomach             = GetSkillCvar("sk_zombie_stomach"); //Hitgroup
	gSkillData.zombieLeg                 = GetSkillCvar("sk_zombie_leg"); //Hitgroup
	gSkillData.zombieArm                 = GetSkillCvar("sk_zombie_arm"); //Hitgroup

	//=========================================================
	// NPCs: Zombie Soldier
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.zombieSoldierHealth       = GetSkillCvar("sk_zombie_soldier_health");
	gSkillData.zombieSoldierDmgOneSlash  = GetSkillCvar("sk_zombie_soldier_dmg_one_slash");
	gSkillData.zombieSoldierDmgBothSlash = GetSkillCvar("sk_zombie_soldier_dmg_both_slash");
	gSkillData.zombieSoldierHead         = GetSkillCvar("sk_zombie_soldier_head"); //Hitgroup
	gSkillData.zombieSoldierChest        = GetSkillCvar("sk_zombie_soldier_chest"); //Hitgroup
	gSkillData.zombieSoldierStomach      = GetSkillCvar("sk_zombie_soldier_stomach"); //Hitgroup
	gSkillData.zombieSoldierLeg          = GetSkillCvar("sk_zombie_soldier_leg"); //Hitgroup
	gSkillData.zombieSoldierArm          = GetSkillCvar("sk_zombie_soldier_arm"); //Hitgroup

	//=========================================================
	// NPCs: Zombie Construction
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.zombieConstructionHealth  = GetSkillCvar("sk_zombie_construction_health");
	gSkillData.zombieConstructionDmgOneSlash = GetSkillCvar("sk_zombie_construction_dmg_one_slash");
	gSkillData.zombieConstructionDmgBothSlash = GetSkillCvar("sk_zombie_construction_dmg_both_slash");
	gSkillData.zombieConstructionHead    = GetSkillCvar("sk_zombie_construction_head"); //Hitgroup
	gSkillData.zombieConstructionChest   = GetSkillCvar("sk_zombie_construction_chest"); //Hitgroup
	gSkillData.zombieConstructionStomach = GetSkillCvar("sk_zombie_construction_stomach"); //Hitgroup
	gSkillData.zombieConstructionLeg     = GetSkillCvar("sk_zombie_construction_leg"); //Hitgroup
	gSkillData.zombieConstructionArm     = GetSkillCvar("sk_zombie_construction_arm"); //Hitgroup

	//=========================================================
	// NPCs: Zombie Barney
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.zombieBarneyHealth        = GetSkillCvar("sk_zombie_barney_health");
	gSkillData.zombieBarneyDmgOneSlash   = GetSkillCvar("sk_zombie_barney_dmg_one_slash");
	gSkillData.zombieBarneyDmgBothSlash  = GetSkillCvar("sk_zombie_barney_dmg_both_slash");
	gSkillData.zombieBarneyHead          = GetSkillCvar("sk_zombie_barney_head"); //Hitgroup 
	gSkillData.zombieBarneyChest         = GetSkillCvar("sk_zombie_barney_chest"); //Hitgroup
	gSkillData.zombieBarneyStomach       = GetSkillCvar("sk_zombie_barney_stomach"); //Hitgroup
	gSkillData.zombieBarneyLeg           = GetSkillCvar("sk_zombie_barney_leg"); //Hitgroup
	gSkillData.zombieBarneyArm           = GetSkillCvar("sk_zombie_barney_arm"); //Hitgroup

	//=========================================================
	// NPCs: Pit Drone
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.pitdroneHealth			= GetSkillCvar("sk_pitdrone_health");
	gSkillData.pitdroneDmgBite			= GetSkillCvar("sk_pitdrone_dmg_bite");
	gSkillData.pitdroneDmgWhip			= GetSkillCvar("sk_pitdrone_dmg_whip");
	gSkillData.pitdroneDmgSpit			= GetSkillCvar("sk_pitdrone_dmg_spit");
	gSkillData.pitdroneHead				= GetSkillCvar("sk_pitdrone_head"); //Hitgroup 
	gSkillData.pitdroneChest			= GetSkillCvar("sk_pitdrone_chest"); //Hitgroup
	gSkillData.pitdroneStomach			= GetSkillCvar("sk_pitdrone_stomach"); //Hitgroup
	gSkillData.pitdroneLeg				= GetSkillCvar("sk_pitdrone_leg"); //Hitgroup
	gSkillData.pitdroneArm				= GetSkillCvar("sk_pitdrone_arm"); //Hitgroup

	//=========================================================
	// NPCs: Voltigore
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.voltigoreHealth			= GetSkillCvar("sk_voltigore_health");
	gSkillData.voltigoreDmgPunch		= GetSkillCvar("sk_voltigore_dmg_punch");
	gSkillData.voltigoreDmgBeam			= GetSkillCvar("sk_voltigore_dmg_beam");
	gSkillData.voltigoreHead			= GetSkillCvar("sk_voltigore_head"); //Hitgroup 
	gSkillData.voltigoreChest			= GetSkillCvar("sk_voltigore_chest"); //Hitgroup
	gSkillData.voltigoreStomach			= GetSkillCvar("sk_voltigore_stomach"); //Hitgroup
	gSkillData.voltigoreLeg				= GetSkillCvar("sk_voltigore_leg"); //Hitgroup
	gSkillData.voltigoreArm			    = GetSkillCvar("sk_voltigore_arm"); //Hitgroup

	//=========================================================
	// NPCs: Ally Grunt
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.fgruntHealth				= GetSkillCvar("sk_hgrunt_ally_health");
	gSkillData.fgruntDmgKick			= GetSkillCvar("sk_hgrunt_ally_kick");
	gSkillData.fgruntShotgunPellets		= GetSkillCvar("sk_hgrunt_ally_pellets");
	gSkillData.fgruntGrenadeSpeed		= GetSkillCvar("sk_hgrunt_ally_gspeed");
	gSkillData.fgruntHead				= GetSkillCvar("sk_hgrunt_ally_head"); //Hitgroup 
	gSkillData.fgruntChest				= GetSkillCvar("sk_hgrunt_ally_chest"); //Hitgroup
	gSkillData.fgruntStomach			= GetSkillCvar("sk_hgrunt_ally_stomach"); //Hitgroup
	gSkillData.fgruntLeg				= GetSkillCvar("sk_hgrunt_ally_leg"); //Hitgroup
	gSkillData.fgruntArm				= GetSkillCvar("sk_hgrunt_ally_arm"); //Hitgroup

	//=========================================================
	// NPCs: Ally Grunt Medic
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.medicHealth = GetSkillCvar( "sk_medic_ally_health");
	gSkillData.medicDmgKick = GetSkillCvar( "sk_medic_ally_kick");
	gSkillData.medicHeal = GetSkillCvar( "sk_medic_ally_heal");

	//=========================================================
	// NPCs: Ally Grunt Torch
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.torchHealth = GetSkillCvar( "sk_torch_ally_health");
	gSkillData.torchDmgKick = GetSkillCvar( "sk_torch_ally_kick");

	//=========================================================
	// NPCs: Pitworm
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.pwormHealth = GetSkillCvar("sk_pitworm_health");
	gSkillData.pwormDmgSwipe = GetSkillCvar("sk_pitworm_dmg_swipe");
	gSkillData.pwormDmgBeam = GetSkillCvar("sk_pitworm_dmg_beam");

	//=========================================================
	// NPCs: Shock Roach
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.sroachHealth = GetSkillCvar("sk_shockroach_health");
	gSkillData.sroachDmgBite = GetSkillCvar("sk_shockroach_dmg_bite");
	gSkillData.sroachLifespan = GetSkillCvar("sk_shockroach_lifespan");

	//=========================================================
	// NPCs: Gonome
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.gonomeHealth = GetSkillCvar("sk_gonome_health");
	gSkillData.gonomeDmgOneSlash = GetSkillCvar("sk_gonome_dmg_one_slash");
	gSkillData.gonomeDmgGuts = GetSkillCvar("sk_gonome_dmg_guts");
	gSkillData.gonomeDmgOneBite = GetSkillCvar("sk_gonome_dmg_one_bite");

	//=========================================================
	// NPCs: ShockTrooper
	// For Spirit of Half-Life v1.9: Opposing-Force Edition
	//=========================================================
	gSkillData.strooperHealth = GetSkillCvar("sk_shocktrooper_health");
	gSkillData.strooperDmgKick = GetSkillCvar("sk_shocktrooper_kick");
	gSkillData.strooperGrenadeSpeed = GetSkillCvar("sk_shocktrooper_gspeed");
	gSkillData.strooperMaxCharge = GetSkillCvar("sk_shocktrooper_maxcharge");
	gSkillData.strooperRchgSpeed = GetSkillCvar("sk_shocktrooper_rchgspeed");

	//Turret
	gSkillData.turretHealth = GetSkillCvar( "sk_turret_health");

	// MiniTurret
	gSkillData.miniturretHealth = GetSkillCvar( "sk_miniturret_health");
	
	// Sentry Turret
	gSkillData.sentryHealth = GetSkillCvar( "sk_sentry_health");

	// Shockroach
	gSkillData.monDmgShockroach = GetSkillCvar("sk_plr_shockroachm");

// PLAYER WEAPONS

	// Crowbar whack
	gSkillData.plrDmgCrowbar = GetSkillCvar( "sk_plr_crowbar");

	// pipe wrench
	gSkillData.plrDmgWrench = GetSkillCvar("sk_plr_pipewrench");

	// Knife whack
	gSkillData.plrDmgKnife = GetSkillCvar("sk_plr_knife");
	gSkillData.plrDmgKnifeCharge = GetSkillCvar("sk_plr_knife_charge");

	// Glock Round
	gSkillData.plrDmg9MM = GetSkillCvar( "sk_plr_9mm_bullet");

	// 357 Round
	gSkillData.plrDmg357 = GetSkillCvar( "sk_plr_357_bullet");

	// 762 Round
	gSkillData.plrDmg762 = GetSkillCvar("sk_plr_762_bullet");

	// MP5 Round
	gSkillData.plrDmgMP5 = GetSkillCvar( "sk_plr_9mmAR_bullet");

	// M203 grenade
	gSkillData.plrDmgM203Grenade = GetSkillCvar( "sk_plr_9mmAR_grenade");

	// Shotgun buckshot
	gSkillData.plrDmgBuckshot = GetSkillCvar( "sk_plr_buckshot");

	// Crossbow
	gSkillData.plrDmgCrossbowClient = GetSkillCvar( "sk_plr_xbow_bolt_client");
	gSkillData.plrDmgCrossbowMonster = GetSkillCvar( "sk_plr_xbow_bolt_monster");

	// RPG
	gSkillData.plrDmgRPG = GetSkillCvar( "sk_plr_rpg");

	// Gauss gun
	gSkillData.plrDmgGauss = GetSkillCvar( "sk_plr_gauss");

	// Egon Gun
	gSkillData.plrDmgEgonNarrow = GetSkillCvar( "sk_plr_egon_narrow");
	gSkillData.plrDmgEgonWide = GetSkillCvar( "sk_plr_egon_wide");

	// Hand Grendade
	gSkillData.plrDmgHandGrenade = GetSkillCvar( "sk_plr_hand_grenade");

	// Satchel Charge
	gSkillData.plrDmgSatchel = GetSkillCvar( "sk_plr_satchel");

	// Tripmine
	gSkillData.plrDmgTripmine = GetSkillCvar( "sk_plr_tripmine");

	// MONSTER WEAPONS
	gSkillData.monDmg12MM = GetSkillCvar( "sk_12mm_bullet");
	gSkillData.monDmgMP5 = GetSkillCvar ("sk_9mmAR_bullet" );
	gSkillData.monDmg9MM = GetSkillCvar( "sk_9mm_bullet");
	gSkillData.monDmgM249 = GetSkillCvar("sk_556_bullet");

	// MONSTER HORNET
	gSkillData.monDmgHornet = GetSkillCvar( "sk_hornet_dmg");

	// M249
	gSkillData.plrDmgM249 = GetSkillCvar("sk_plr_556_bullet");

	// Shockroach
	gSkillData.plrDmgShock = GetSkillCvar("sk_plr_shockroachs");

	// Sporegrenade
	gSkillData.plrDmgSpore = GetSkillCvar("sk_plr_spore");

	// PLAYER HORNET
// Up to this point, player hornet damage and monster hornet damage were both using
// monDmgHornet to determine how much damage to do. In tuning the hivehand, we now need
// to separate player damage and monster hivehand damage. Since it's so late in the project, we've
// added plrDmgHornet to the SKILLDATA struct, but not to the engine CVar list, so it's inaccesible
// via SKILLS.CFG. Any player hivehand tuning must take place in the code. (sjb)
	gSkillData.plrDmgHornet = 7;

	// HEALTH/CHARGE
	gSkillData.suitchargerCapacity = GetSkillCvar( "sk_suitcharger" );
	gSkillData.batteryCapacity = GetSkillCvar( "sk_battery" );
	gSkillData.healthchargerCapacity = GetSkillCvar ( "sk_healthcharger" );
	gSkillData.healthkitCapacity = GetSkillCvar ( "sk_healthkit" );
	gSkillData.scientistHeal = GetSkillCvar ( "sk_scientist_heal" );
    gSkillData.flashlightCharge = GetSkillCvar ( "sk_flashcharge" );
	
	// monster damage adj
	gSkillData.monHead = GetSkillCvar( "sk_monster_head" );
	gSkillData.monChest = GetSkillCvar( "sk_monster_chest" );
	gSkillData.monStomach = GetSkillCvar( "sk_monster_stomach" );
	gSkillData.monLeg = GetSkillCvar( "sk_monster_leg" );
	gSkillData.monArm = GetSkillCvar( "sk_monster_arm" );

	// player damage adj
	gSkillData.plrHead = GetSkillCvar( "sk_player_head" );
	gSkillData.plrChest = GetSkillCvar( "sk_player_chest" );
	gSkillData.plrStomach = GetSkillCvar( "sk_player_stomach" );
	gSkillData.plrLeg = GetSkillCvar( "sk_player_leg" );
	gSkillData.plrArm = GetSkillCvar( "sk_player_arm" );
}

//=========================================================
// instantiate the proper game rules object
//=========================================================

CGameRules *InstallGameRules( void )
{
	SERVER_COMMAND( "exec game.cfg\n" );
	SERVER_EXECUTE( );

	if ( !gpGlobals->deathmatch )
	{
		// generic half-life
		g_teamplay = 0;
		return new CHalfLifeRules;
	}
	else
	{
		if ( teamplay.value > 0 )
		{
			// teamplay

			g_teamplay = 1;
			return new CHalfLifeTeamplay;
		}
		if ((int)gpGlobals->deathmatch == 1)
		{
			// vanilla deathmatch
			g_teamplay = 0;
			return new CHalfLifeMultiplay;
		}
		else
		{
			// vanilla deathmatch??
			g_teamplay = 0;
			return new CHalfLifeMultiplay;
		}
	}
}



