$modelname "v_gauss.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures


$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000


//reference mesh(es)
$body "body" "v_gauss"

$bodygroup "hands"
{
	//studio "v_barney_hands_reference"
	//studio "v_gordon_hands_reference"
	studio "v_soldier_hands_reference"
}


// 2 attachment(s)
$attachment 0 "Gauss Master" 17.000000 -1.000000 0.000000
$attachment 1 "Spinner" -4.000000 0.000000 0.000000

// 8 hit box(es)
$hbox 0 "Bip01 R Hand" 0.000000 -0.980000 -1.700000 5.073513 0.870000 5.123067
$hbox 0 "Gauss Master" -11.720000 -4.220000 -5.180000 11.330000 3.800000 2.670000
$hbox 0 "Spinner" -3.390000 -1.740000 -2.470000 0.000000 2.560000 2.480000
$hbox 0 "Bip01 L Hand" 0.000000 -0.960000 -1.820000 3.910000 0.890000 1.710000
$hbox 0 "Bip01 L Finger0" 0.000000 -0.950000 -0.560000 2.440000 0.380000 0.960000
$hbox 0 "L_Arm_bone" -1.910000 -2.640000 -2.290000 2.760000 2.490000 7.980000
$hbox 0 "Bip01 R Finger0" 0.000000 -0.930000 -1.000000 2.440000 0.390000 0.540000
$hbox 0 "R_Arm_bone" -2.610000 -2.630000 -2.290000 2.050000 2.490000 7.980000

// 9 animation sequence(s)
$sequence "idle" "idle" fps 15 loop 
$sequence "idle2" "idle2" fps 15 loop 
$sequence "fidget" "fidget" fps 30 { event 5004 21 "weapons/gauss_fidget.wav" } 
$sequence "spinup" "spinup" fps 30 loop 
$sequence "spin" "spin" fps 35 loop 
$sequence "fire" "fire" fps 30 
$sequence "fire2" "fire2" fps 30 
$sequence "holster" "holster" fps 45 
$sequence "draw" "draw" fps 60 

// End of QC script.
