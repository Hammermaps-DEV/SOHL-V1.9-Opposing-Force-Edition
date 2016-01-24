$modelname "v_crossbow.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures


$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000


//reference mesh(es)
$body "body" "v_crossbow_reference"

$bodygroup "hands"
{
	//studio "v_barney_hands_reference"
	//studio "v_gordon_hands_reference"
	studio "v_soldier_hands_reference"
}

$bodygroup "arrows"
{
studio "v_arrow4"
studio "v_arrow3"
studio "v_arrow2"
studio "v_arrow1"
blank
}


// 9 hit box(es)
$hbox 0 "Bip01 R Hand" 0.000000 -0.990000 -1.710000 3.920000 1.946522 4.704309
$hbox 0 "crossbow parent" -3.940000 -20.639999 -5.310000 3.960000 6.860000 3.590000
$hbox 0 "grip" -4.000000 -10.388143 -2.752418 4.000000 5.101495 0.485730
$hbox 0 "Slide" -1.650000 -3.389288 -3.590000 4.050000 1.690000 0.450000
$hbox 0 "Bip01 L Hand" 0.000000 -0.960000 -1.820000 3.910000 0.900000 1.710000
$hbox 0 "Bip01 L Finger0" 0.000000 -0.950000 -0.570000 2.450000 0.380000 0.970000
$hbox 0 "L_Arm_bone" -1.920000 -2.640000 -2.300000 2.760000 2.490000 7.980000
$hbox 0 "Bip01 R Finger0" 0.000000 -0.940000 -1.000000 2.450000 0.390000 0.540000
$hbox 0 "R_Arm_bone" -2.620000 -2.640000 -2.290000 2.060000 2.490000 7.990000

// 10 animation sequence(s)
$sequence "idle1" "idle1" fps 30 
$sequence "idle2" "idle2" fps 30 
$sequence "fidget1" "fidget1" fps 30 
$sequence "fire" "fire" fps 60 { event 5004 25 "weapons/xbow_reload1.wav" } 
$sequence "fire_last" "fire_last" fps 30 
$sequence "reload" "reload" fps 30 { event 5004 12 "weapons/reload2.wav" } { event 5004 46 "weapons/reload3.wav" } { event 5004 69 "weapons/xbow_reload1.wav" } 
$sequence "draw1" "draw1" fps 30 
$sequence "draw2" "draw2" fps 30 
$sequence "holster1" "holster1" fps 25 
$sequence "holster2" "holster2" fps 25 

// End of QC script.
