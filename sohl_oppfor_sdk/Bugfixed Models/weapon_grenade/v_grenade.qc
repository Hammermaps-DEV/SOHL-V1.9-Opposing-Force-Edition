$modelname "v_grenade.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures

$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000

//reference mesh(es)
$body "body" "grenade"

$bodygroup "hands"
{
	//studio "v_barney_hands_Reference"
	//studio "v_gordon_hands_Reference"
	studio "v_soldier_hands_reference"
}


// 7 hit box(es)
$hbox 0 "pine" -1.300000 -1.310000 -2.370000 1.300000 1.210000 2.350000
$hbox 0 "Bip01 L Hand" 0.000000 -0.960000 -1.820000 3.910000 0.900000 1.710000
$hbox 0 "Bip01 L Finger0" 0.000000 -0.950000 -0.570000 2.450000 0.380000 0.970000
$hbox 0 "L_Arm_bone" -1.920000 -2.640000 -2.300000 2.760000 2.490000 7.980000
$hbox 0 "Bip01 R Hand" 0.000000 -0.990000 -1.710000 3.920000 0.870000 1.820000
$hbox 0 "Bip01 R Finger0" 0.000000 -0.940000 -1.000000 2.450000 0.390000 0.540000
$hbox 0 "R_Arm_bone" -2.620000 -2.640000 -2.290000 2.060000 2.490000 7.990000

// 8 animation sequence(s)
$sequence "idle" "idle" fps 30 loop 
$sequence "fidget" "fidget" fps 30 loop 
$sequence "pinpull" "pinpull" fps 45 
$sequence "throw1" "throw1" fps 13 
$sequence "throw2" "throw2" fps 20 
$sequence "throw3" "throw3" fps 30 
$sequence "holster" "holster" fps 30 
$sequence "draw" "draw" fps 30 

// End of QC script.
