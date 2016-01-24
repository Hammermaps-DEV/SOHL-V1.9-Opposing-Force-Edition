/*
//=========================================================
// NPC: Pit Drone * http://half-life.wikia.com/wiki/Pit_Drone
// For Spirit of Half-Life v1.9: Opposing-Force Edition
// Version: 1.0 / Date: 18.10.2015
//=========================================================
*/

$modelname "pit_drone.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures

$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 36.000000

//reference mesh(es)
$body "studio" "pit_drone_reference"

$bodygroup "gun"
{
	blank
	studio "pit_drone_horns01"
	studio "pit_drone_horns02"
	studio "pit_drone_horns03"
	studio "pit_drone_horns04"
	studio "pit_drone_horns05"
	studio "pit_drone_horns06"
}

// 17 hit box(es)
$hbox 2 "Bip01 Spine1" -12.000000 -9.000000 -10.000000 12.000000 17.000000 10.000000
$hbox 2 "Bip01 Neck" 0.000000 -1.000000 -4.650000 2.000000 0.000000 4.500000
$hbox 1 "Bip01 Head" 0.000000 -7.000000 -6.000000 12.000000 15.000000 6.000000
$hbox 4 "Bip01 L UpperArm" -2.260000 -5.800000 -4.750000 17.309999 4.480000 3.750000
$hbox 4 "Bip01 L Forearm" 0.000000 -1.920000 -1.680000 21.110001 2.670000 2.750000
$hbox 4 "Bip01 L Finger0" -1.780000 -1.660000 -2.710000 26.290001 0.830000 3.580000
$hbox 5 "Bip01 R UpperArm" -2.650000 -5.500000 -3.990000 16.889999 4.780000 4.460000
$hbox 5 "Bip01 R Forearm" 0.000000 -2.220000 -2.990000 21.110001 2.370000 1.440000
$hbox 5 "Bip01 R Finger0" -1.860000 -1.730000 -4.140000 26.209999 0.750000 2.150000
$hbox 6 "Bip01 L Thigh" 0.000000 -7.180000 -1.670000 15.760000 6.040000 4.230000
$hbox 6 "Bip01 L Calf" 0.000000 -2.770000 -2.690000 22.180000 4.430000 2.900000
$hbox 6 "Bip01 L Foot" 0.000000 -3.000000 -3.000000 5.300000 14.000000 3.000000
$hbox 7 "Bip01 R Thigh" 0.000000 -5.950000 -4.410000 15.760000 6.040000 2.600000
$hbox 7 "Bip01 R Calf" 0.000000 -2.770000 -2.900000 22.180000 4.430000 2.330000
$hbox 7 "Bip01 R Foot" 0.000000 -3.000000 -3.000000 5.300000 14.000000 3.000000
$hbox 3 "Bip01 Tail" 0.000000 -3.170000 -4.290000 8.540000 5.370000 4.110000
$hbox 3 "Bip01 Tail1" 0.000000 -3.160000 -2.850000 13.000000 2.680000 2.670000

// 19 animation sequence(s)
$sequence "idle_1" "idle_1" fps 30 loop ACT_IDLE 1 
$sequence "idle_2" "idle_2" fps 30 loop ACT_IDLE 2 
$sequence "walk" "walk" LX fps 30 loop ACT_WALK 1 
$sequence "gallop" "gallop" LX fps 30 loop ACT_WALK 2 
$sequence "run" "run" LX fps 50 loop ACT_RUN 1 
$sequence "jump" "jump" fps 30 ACT_HOP 1
$sequence "flinchs" "flinchs" fps 30 ACT_SMALL_FLINCH 1 
$sequence "flinchb" "flinchb" fps 30 ACT_BIG_FLINCH 1 
$sequence "turnleft" "turnleft" fps 30 ACT_TURN_LEFT 1 
$sequence "turnright" "turnright" fps 30 ACT_TURN_RIGHT 1 
$sequence "bite" "bite" fps 25 ACT_MELEE_ATTACK1 1 { event 4 12 } 
$sequence "whip" "whip" fps 30 ACT_MELEE_ATTACK1 1 {
	{ event 5 13 } 
	{ event 6 25 } 
} 
$sequence "range" "range" fps 30 ACT_RANGE_ATTACK1 1 {
	{ event 1 10 } 
	{ event 2 14 } 
}
$sequence "eat" "eat" fps 30 loop ACT_EAT 1 { event 1008 1 "pitdrone/pit_drone_eat.wav" } 
$sequence "reload" "reload" fps 13 ACT_RELOAD 1 { event 3 14 } 
$sequence "die" "die" fps 30 ACT_DIEFORWARD 1 { event 1008 1 "pitdrone/pit_drone_die1.wav" } 
$sequence "die1" "die1" fps 30 ACT_DIEBACKWARD 1 { event 1008 1 "pitdrone/pit_drone_die2.wav" } 
$sequence "die2" "die2" fps 30 ACT_DIESIMPLE 1 { event 1008 1 "pitdrone/pit_drone_die3.wav" } 
$sequence "jump_over_counter" "jump_over_counter" fps 17 { event 1008 1 "pitdrone/pit_drone_hunt3.wav" } 