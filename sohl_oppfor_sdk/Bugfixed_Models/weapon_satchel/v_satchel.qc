$modelname "v_satchel.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures


$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000


//reference mesh(es)
$body "body" "satchel_charge"

$bodygroup "hands"
{
	//studio "v_barney_hands_reference"
	//studio "v_gordon_hands_reference"
	studio "v_soldier_hands_reference"
}


// 8 hit box(es)
$hbox 0 "Bip01 L Hand" 0.000000 -0.960000 -1.820000 3.910000 3.999003 1.710000
$hbox 0 "psx_v_satchel" -4.870000 -5.920000 -4.660000 5.050000 4.320000 4.890000
$hbox 0 "Bone01" -0.836786 -0.240000 -2.177715 0.450000 1.124343 0.040000
$hbox 0 "Bip01 L Finger0" 0.000000 -0.950000 -0.570000 2.450000 0.380000 0.970000
$hbox 0 "L_Arm_bone" -1.920000 -2.640000 -2.300000 2.760000 2.490000 7.980000
$hbox 0 "Bip01 R Hand" 0.000000 -0.990000 -1.710000 3.920000 0.870000 1.820000
$hbox 0 "Bip01 R Finger0" 0.000000 -0.940000 -1.000000 2.450000 0.390000 0.540000
$hbox 0 "R_Arm_bone" -2.620000 -2.640000 -2.290000 2.060000 2.490000 7.990000

// 4 animation sequence(s)
$sequence "idle1" "idle1" fps 20 loop 
$sequence "fidget1" "fidget1" fps 30 
$sequence "draw" "draw" fps 30 
$sequence "drop" "drop" fps 30 

// End of QC script.
