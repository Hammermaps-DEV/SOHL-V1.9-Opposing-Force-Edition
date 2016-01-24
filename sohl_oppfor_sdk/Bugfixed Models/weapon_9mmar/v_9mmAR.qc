$modelname "v_9mmAR.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures

$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000

//reference mesh(es)
$body "studio" "m4_view_reference"

$bodygroup "hands"
{
	//studio "v_barney_hands_reference"
	//studio "v_gordon_hands_reference"
	studio "v_soldier_hands_reference"
}

// 1 attachment(s)
$attachment 0 "carbine" 0.500000 -27.000000 2.000000

// 8 hit box(es)
$hbox 0 "carbine" -0.390000 -26.900000 -4.740000 1.880000 6.960000 6.030000
$hbox 0 "tube" -1.340000 -7.290000 -3.780000 1.340000 7.190000 0.120000
$hbox 0 "Bip01 L Hand" 0.000000 -1.040000 -1.850000 3.900000 0.820000 1.710000
$hbox 0 "Bip01 L Finger0" 0.000000 -0.870000 -0.470000 2.373625 0.360000 0.900000
$hbox 0 "L_Arm_bone" -1.850000 -2.510000 -3.310000 2.570000 2.480000 9.180000
$hbox 0 "Bip01 R Hand" 0.000000 -1.060000 -1.720000 3.900000 0.800000 1.830000
$hbox 0 "Bip01 R Finger0" 0.000000 -0.850000 -0.920000 2.373626 0.370000 0.450000
$hbox 0 "R_Arm_bone" -2.430000 -2.520000 -3.290000 1.980000 2.530000 9.200000

// 8 animation sequence(s)
$sequence "idle" "idle" fps 8 
$sequence "draw" "draw" fps 30 
$sequence "holster" "holster" fps 25 
$sequence "shoot_1" "shoot1" fps 10 { event 5001 0 "40" } 
$sequence "shoot_2" "shoot2" fps 10 { event 5001 0 "40" } 
$sequence "grenade" "grenade" fps 30 { event 5004 1 "weapons/glauncher.wav" } 
$sequence "reload" "reload" fps 30 { event 5004 5 "items/cliprelease1.wav" } { event 5004 24 "items/clipinsert1.wav" } 

// End of QC script.
