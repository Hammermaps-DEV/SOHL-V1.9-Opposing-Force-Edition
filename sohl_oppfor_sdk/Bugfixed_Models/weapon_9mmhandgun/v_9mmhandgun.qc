$modelname "v_9mmhandgun.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures

$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000

//reference mesh(es)
$body "body" "v_beretta_reference"

$bodygroup "hands"
{
	//studio "v_barney_hands_reference"
	//studio "v_gordon_hands_reference"
	studio "v_soldier_hands_reference"
}

$body "clip" "v_clip"


// 2 attachment(s)
$attachment 0 "Bip01 R Hand" 12.000000 1.400000 4.300000
$attachment 1 "Bip01 R Hand" 20.000000 1.900000 5.000000

// 5 hit box(es)
$hbox 0 "Bip01 L Arm2" 0.000000 -1.820000 -1.460000 11.370000 2.590000 2.950000
$hbox 0 "Bip01 L Hand" 0.000000 -0.870000 -1.730000 4.170000 1.030000 1.860000
$hbox 0 "Bip01 R Arm2" 0.000000 -1.960000 -3.090000 11.370000 2.730000 1.550000
$hbox 0 "Bip01 R Hand" 0.000000 -0.870000 -2.600000 4.150000 3.330000 5.070000
$hbox 10 "Bone02" -0.020000 -8.550000 -0.810000 4.310000 0.660000 0.680000

// 12 animation sequence(s)
$sequence "idle1" "idle1" fps 16 
$sequence "idle2" "idle2" fps 16 
$sequence "idle3" "idle3" fps 14 
$sequence "draw" "draw" fps 30 
$sequence "holster" "holster" fps 30 
$sequence "shoot" "shoot" fps 25 
$sequence "shoot_empty" "shoot_empty" fps 25 
$sequence "reload" "reload" fps 18 { event 5004 4 "items/9mmclip2.wav" } { event 5004 23 "items/9mmclip1.wav" } 
$sequence "reload_noshot" "reload_noshot" fps 18 { event 5004 4 "items/9mmclip2.wav" } { event 5004 23 "items/9mmclip1.wav" } 

// End of QC script.
