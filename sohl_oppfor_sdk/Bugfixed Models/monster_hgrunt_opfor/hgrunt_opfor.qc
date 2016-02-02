$modelname "hgrunt_opfor.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures


$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 73.000000


//reference mesh(es)
$body "body" "grunt_fatigues_reference"

$bodygroup "heads"
{
studio "grunt_head_mask_reference"
studio "grunt_head_commander_reference"
studio "grunt_head_shotgun_reference"
studio "grunt_head_saw_wht_reference"
studio "grunt_head_saw_blk_reference"
studio "grunt_head_MP_reference"
studio "grunt_head_major_reference"
studio "grunt_head_commander_blk_reference"
}

$bodygroup "torso"
{
studio "grunt_reg_torso_reference"
studio "grunt_saw_gunner_torso_reference"
studio "grunt_noback_torso_reference"
studio "grunt_shotgun_torso_reference"
}

$bodygroup "weapons"
{
studio "MP5_reference"
studio "shotgun_reference"
studio "SAW_reference"
blank
}


// 3 attachment(s)
$attachment 0 "Bip01 R Hand" 25.000000 0.000000 5.000000
$attachment 1 "Bip01 R Hand" 0.000000 0.000000 0.000000
$attachment 2 "Bip01 Head" 0.000000 0.000000 0.000000

// 2 bone controller(s)
$controller 4 "Bone05" ZR 0.000000 45.000000
$controller 0 "Bip01 Head" XR -70.000000 70.000000

// 16 hit box(es)
$hbox 3 "Bip01 Pelvis" -3.640000 -6.050000 -8.070000 6.340000 8.410000 8.300000
$hbox 6 "Bip01 L Thigh" 0.000000 -6.350000 -6.050000 16.309999 5.570000 3.840000
$hbox 6 "Bip01 L Calf" 0.000000 -5.190000 -4.440000 22.150000 5.680000 4.780000
$hbox 6 "Bip01 L Foot" -1.080000 -1.900000 -3.240000 3.230000 11.210000 2.860000
$hbox 7 "Bip01 R Thigh" 0.000000 -6.350000 -4.060000 16.309999 5.570000 5.820000
$hbox 7 "Bip01 R Calf" 0.000000 -5.190000 -5.010000 22.150000 5.680000 4.220000
$hbox 7 "Bip01 R Foot" -1.080000 -1.900000 -3.720000 3.230000 11.210000 2.390000
$hbox 3 "Bip01 Spine1" -4.570000 -9.990000 -10.520000 11.490000 8.420000 10.750000
$hbox 2 "Bip01 Spine2" 0.000000 -10.510000 -11.440000 14.050000 9.260000 12.420000
$hbox 1 "Bip01 Head" 0.000000 -5.430000 -6.130000 14.660000 9.400000 6.110000
$hbox 4 "Bip01 L UpperArm" -2.810000 -1.990000 -4.740000 12.060000 4.780000 3.500000
$hbox 4 "Bip01 L Forearm" 0.000000 -1.430000 -2.150000 10.210000 4.010000 2.980000
$hbox 4 "Bip01 L Hand" -0.270000 -2.650000 -2.750000 7.580000 1.890000 1.800000
$hbox 5 "Bip01 R UpperArm" -2.830000 -1.990000 -3.460000 12.030000 4.780000 4.780000
$hbox 5 "Bip01 R Forearm" 0.000000 -1.430000 -2.940000 10.190000 4.000000 2.180000
$hbox 5 "Bip01 R Hand" -0.160000 -2.040000 -1.790000 7.500000 2.400000 2.760000

$sequencegroupsize 64

// 73 animation sequence(s)
$sequence "walk1" "walk1" LX fps 30 loop ACT_WALK 1 
$sequence "run" "run" LX fps 30 loop ACT_RUN 1 
$sequence "victorydance" "victorydance" fps 21 ACT_VICTORY_DANCE 1 
$sequence "cower" "cower" fps 28 ACT_COWER 1 
$sequence "smflinch" "smflinch" fps 20 ACT_SMALL_FLINCH 5 
$sequence "launchgrenade" "launchgrenade" fps 30 ACT_RANGE_ATTACK2 1 { event 8 16 } 
$sequence "throwgrenade" "throwgrenade" fps 30 ACT_RANGE_ATTACK2 1 { event 7 36 } 
$sequence "idle1" "idle1" fps 30 loop ACT_IDLE 1 
$sequence "idle2" "idle2" fps 10 loop ACT_IDLE 1 
$sequence "combatidle" "combatidle" fps 30 loop ACT_IDLE_ANGRY 1 
$sequence "frontkick" "frontkick" fps 35 ACT_MELEE_ATTACK1 1 { event 3 14 } { event 2010 14 } 
$sequence "crouching_idle" "crouching_idle" fps 30 loop 
$sequence "crouching_wait" "crouching_wait" fps 30 ACT_CROUCH 0 
$sequence "crouching_mp5" "crouching_mp5_blend1" "crouching_mp5_blend2" blend XR -45 45 fps 20 loop ACT_RANGE_ATTACK1 1 {
  { event 4 0 } 
  { event 5 2 } 
  { event 6 4 } 
  { event 5001 0 "50" } 
  { event 5001 2 "50" } 
  { event 5001 4 "50" } 
 }
$sequence "standing_mp5" "standing_mp5_blend1" "standing_mp5_blend2" blend XR -45 45 fps 20 loop {
  { event 4 0 } 
  { event 5 2 } 
  { event 6 4 } 
  { event 5001 0 "50" } 
  { event 5001 2 "50" } 
  { event 5001 4 "50" } 
 }
$sequence "reload_mp5" "reload_mp5" fps 40 ACT_RELOAD 1 { event 2 47 } 
$sequence "crouching_shotgun" "crouching_shotgun_blend1" "crouching_shotgun_blend2" blend XR -45 45 fps 20 ACT_RANGE_ATTACK1 1 { event 4 0 } { event 5001 0 "51" } 
$sequence "standing_shotgun" "standing_shotgun_blend1" "standing_shotgun_blend2" blend XR -45 45 fps 20 { event 4 0 } { event 5001 0 "51" } 
$sequence "reload_shotgun" "reload_shotgun" fps 40 { event 2 47 } 
$sequence "crouching_saw" "crouching_saw_blend1" "crouching_saw_blend2" blend XR -45 45 fps 20 loop ACT_RANGE_ATTACK1 1 {
  { event 4 3 } 
  { event 5001 3 "50" } 
  { event 5 6 } 
  { event 5001 6 "50" } 
  { event 5 9 } 
  { event 5001 9 "50" } 
  { event 5 12 } 
  { event 5001 12 "50" } 
  { event 5 13 } 
  { event 5001 13 "50" } 
  { event 5 16 } 
  { event 5001 16 "50" } 
  { event 6 19 } 
  { event 5001 19 "50" } 
 }
$sequence "standing_saw" "standing_saw_blend1" "standing_saw_blend2" blend XR -45 45 fps 20 loop {
  { event 4 3 } 
  { event 5001 3 "50" } 
  { event 5 6 } 
  { event 5001 6 "50" } 
  { event 5 9 } 
  { event 5001 9 "50" } 
  { event 5 12 } 
  { event 5001 12 "50" } 
  { event 5 13 } 
  { event 5001 13 "50" } 
  { event 5 16 } 
  { event 5001 16 "50" } 
  { event 6 19 } 
  { event 5001 19 "50" } 
 }
$sequence "reload_saw" "reload_saw" fps 40 ACT_RELOAD 1 { event 2 47 } 
$sequence "advance_signal" "advance_signal" fps 30 ACT_SIGNAL1 1 { event 10 1 } 
$sequence "flank_signal" "flank_signal" fps 40 ACT_SIGNAL2 1 
$sequence "retreat_signal" "retreat_signal" fps 30 ACT_SIGNAL3 1 
$sequence "drop_grenade" "drop_grenade" fps 40 ACT_SPECIAL_ATTACK1 1 { event 9 22 } 
$sequence "limpingwalk" "limpingwalk" LX fps 30 loop ACT_WALK_HURT 1 
$sequence "limpingrun" "limpingrun" LX fps 30 loop ACT_RUN_HURT 1 
$sequence "180L" "180L" fps 12 ACT_TURN_LEFT 1 
$sequence "180R" "180R" fps 12 ACT_TURN_RIGHT 1 
$sequence "dieback1" "dieback1" X fps 25 ACT_DIEBACKWARD 1 {
  { event 11 5 } 
  { event 2001 17 } 
  { event 1000 26 } 
  { event 1001 26 } 
 }
$sequence "dieforward" "dieforward" X fps 26 ACT_DIEFORWARD 1 {
  { event 11 3 } 
  { event 2001 10 } 
  { event 1000 40 } 
  { event 1001 40 } 
 }
$sequence "diesimple" "diesimple" X fps 15 ACT_DIESIMPLE 1 {
  { event 11 3 } 
  { event 2001 10 } 
  { event 1000 14 } 
  { event 1001 14 } 
 }
$sequence "diebackwards" "diebackwards" X fps 30 ACT_DIEBACKWARD 1 {
  { event 11 6 } 
  { event 2001 12 } 
  { event 1000 53 } 
  { event 1001 53 } 
 }
$sequence "dieheadshot" "dieheadshot" X fps 30 ACT_DIE_HEADSHOT 1 {
  { event 11 1 } 
  { event 2001 20 } 
  { event 1000 32 } 
  { event 1001 32 } 
 }
$sequence "diegutshot" "diegutshot" X fps 30 ACT_DIE_GUTSHOT 1 {
  { event 11 0 } 
  { event 2001 29 } 
  { event 1000 68 } 
  { event 1001 68 } 
 }
$sequence "barnacled1" "barnacled1" fps 18 ACT_BARNACLE_HIT 1 
$sequence "barnacled2" "barnacled2" fps 30 loop ACT_BARNACLE_PULL 1 
$sequence "barnacled3" "barnacled3" fps 18 ACT_BARNACLE_CHOMP 1 
$sequence "barnacled4" "barnacled4" fps 15 loop ACT_BARNACLE_CHEW 1 
$sequence "dead_on_stomach" "dead_on_stomach" fps 1 
$sequence "deadstomach" "deadstomach" fps 1 
$sequence "deadside" "deadside" fps 1 
$sequence "deadsitting" "deadsitting" fps 1 
$sequence "dead_on_back" "dead_on_back" fps 1 
$sequence "dead_headcrabed" "dead_headcrabed" fps 1 
$sequence "hgrunt_dead_stomach" "hgrunt_dead_stomach" fps 1 
$sequence "dead_canyon" "dead_canyon" fps 1 
$sequence "repel_jump" "repel_jump" fps 22 ACT_HOVER 1 
$sequence "repel_repel" "repel_repel" fps 18 loop ACT_GLIDE 1 
$sequence "repel_shoot" "repel_shoot_blend1" "repel_shoot_blend2" blend XR -90 45 fps 18 loop ACT_FLY 1 {
  { event 4 1 } 
  { event 5 3 } 
  { event 6 5 } 
  { event 5001 1 } 
  { event 5001 3 } 
  { event 5001 5 } 
 }
$sequence "repel_land" "repel_land" fps 20 ACT_LAND 1 
$sequence "repel_die" "repel_die" fps 12 
$sequence "sitting" "sitting" fps 10 loop 
$sequence "sitting2" "sitting2" fps 8 loop 
$sequence "sitting3" "sitting3" fps 6 loop 
$sequence "react" "react" fps 30 { event 1000 1 } { event 1001 1 } 
$sequence "holding_on" "holding_on" fps 10 loop 
$sequence "open_floor_grate" "open_floor_grate" fps 25 
$sequence "hgrunt_on_stretcher" "hgrunt_on_stretcher" fps 34 
$sequence "crawling" "crawling" fps 17 { event 1001 120 } 
$sequence "of2a2_speech_idle" "of2a2_speech_idle" fps 30 loop { event 1000 30 } { event 1001 30 } 
$sequence "of2a2_death_speech" "of2a2_death_speech" fps 17 { event 1000 120 } { event 1001 120 } 
$sequence "of1a6_get_on" "of1a6_get_on" fps 10 loop 
$sequence "cpr" "cpr" fps 30 { event 1000 120 } { event 1001 120 } 
$sequence "mic_idle1" "mic_idle1" fps 30 
$sequence "mic_idle2" "mic_idle2" fps 30 
$sequence "mic_idle3" "mic_idle3" fps 30 
$sequence "mic_idle4" "mic_idle4" fps 30 
$sequence "mic_stand" "mic_stand" fps 30 
$sequence "interogation" "interogation" fps 30 
$sequence "interogation_idle" "interogation_idle" fps 30 loop 
$sequence "shoot_shotgun" "shoot_shotgun" fps 25 {
  { event 4 12 } 
  { event 5001 12 "51" } 
  { event 2 28 } 
 }

// End of QC script.
