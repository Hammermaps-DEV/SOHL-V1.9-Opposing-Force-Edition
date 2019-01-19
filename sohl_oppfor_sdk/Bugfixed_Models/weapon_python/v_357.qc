$modelname "v_357.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures

$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000

//reference mesh(es)
$body "gun" "v_python_reference_main"

$bodygroup "hands"
{
	//studio "v_barney_hands_reference"
	//studio "v_gordon_hands_reference"
	studio "v_soldier_hands_ref"
}

$bodygroup "scope"
{
blank
studio "v_python_laser_reference"
}

// 1 attachment(s)
$attachment 0 "python" 0.000000 -14.000000 6.000000

// 10 hit box(es)
$hbox 0 "Bip01 R Hand" 0.000000 -0.990000 -1.710000 5.941133 1.084807 3.855314
$hbox 0 "python" -0.730000 -10.380000 -5.620000 1.070000 5.160000 3.010000
$hbox 0 "revolver" -1.920000 -1.990000 -57.742607 1.080000 22.403749 3.270000
$hbox 0 "speed_loader" -1.300000 -1.300000 0.000000 1.300000 1.300000 3.150000
$hbox 0 "Bip01 L Hand" 0.000000 -0.960000 -1.820000 3.910000 0.900000 1.710000
$hbox 0 "Bip01 L Finger0" 0.000000 -0.950000 -0.570000 2.450000 0.380000 0.970000
$hbox 0 "L_Arm_bone" -1.920000 -2.640000 -2.300000 2.760000 2.490000 7.980000
$hbox 0 "Bip01 R Finger0" 0.000000 -0.940000 -1.000000 2.450000 0.390000 0.540000
$hbox 0 "R_Arm_bone" -2.620000 -2.640000 -2.290000 2.060000 2.490000 7.990000
$hbox 0 "lasersight_mesh" -1.360000 -11.650000 -2.660000 0.790000 0.000000 1.840000

// 8 animation sequence(s)
$sequence "idle1" "idle1" fps 30
$sequence "idle2" "idle2" fps 30
$sequence "idle3" "idle3" fps 30
$sequence "draw" "draw" fps 30 
$sequence "holster" "holster" fps 30 
$sequence "fire" "fire" fps 30 { event 5001 0 "31" } 
$sequence "fidget" "fidget" fps 30 
$sequence "reload" "reload" fps 37 { event 5004 22 "weapons/357_reload1.wav" } { event 5004 73 "weapons/reload2.wav" } { event 5004 81 "weapons/reload2.wav" } { event 5004 101 "weapons/reload1.wav" }

// End of QC script.
