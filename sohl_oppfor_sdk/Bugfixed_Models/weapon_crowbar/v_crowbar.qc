$modelname "v_crowbar.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures

$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000

//reference mesh(es)
$body "studio" "v_crowbar_reference"

$bodygroup "hands"
{
	//studio "v_barney_hands_reference"
	//studio "v_gordon_hands_reference"
	studio "v_soldier_hands_reference"
}

// 6 hit box(es)
$hbox 0 "Bip01 R Hand" 0.000000 -0.990000 -6.110000 8.290000 2.880000 20.160000
$hbox 0 "Bip01 L Hand" 0.000000 -0.960000 -1.820000 3.910000 0.900000 1.710000
$hbox 0 "Bip01 L Finger0" 0.000000 -0.950000 -0.570000 2.450000 0.380000 0.970000
$hbox 0 "L_Arm_bone" -1.920000 -2.640000 -2.300000 2.760000 2.490000 7.980000
$hbox 0 "Bip01 R Finger0" 0.000000 -0.940000 -1.000000 2.450000 0.390000 0.540000
$hbox 0 "R_Arm_bone" -2.620000 -2.640000 -2.290000 2.060000 2.490000 7.990000

// 11 animation sequence(s)
$sequence "idle1" "idle1" fps 13 
$sequence "idle2" "idle2" fps 15 
$sequence "idle3" "idle3" fps 15
$sequence "draw" "draw" fps 24 
$sequence "holster" "holster" fps 24 
$sequence "attack1" "attack1" fps 22 
$sequence "attack1miss" "attack1miss" fps 22 
$sequence "attack2" "attack2" fps 22 
$sequence "attack2miss" "attack2miss" fps 22 
$sequence "attack3" "attack3" fps 24 
$sequence "attack3miss" "attack3miss" fps 24 

// End of QC script.
