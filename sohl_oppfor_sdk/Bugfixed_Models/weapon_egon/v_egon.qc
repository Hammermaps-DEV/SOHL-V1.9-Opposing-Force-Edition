$modelname "v_egon.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.0
$cliptotextures


$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000


//reference mesh(es)
$bodygroup "body"
{
	//studio "v_barney_hands_reference"
	//studio "v_gordon_hands_reference"
	studio "v_soldier_hands_reference"
}

$bodygroup "firemode"
{
studio "v_egon"
studio "v_egon_alt"
}


// 14 skin families
$texturegroup skinfamilies 
{
{ "controls_work.bmp" }
{ "controls_work01.bmp" }
{ "controls_work02.bmp" }
{ "controls_work03.bmp" }
{ "controls_work04.bmp" }
{ "controls_work05.bmp" }
{ "controls_work06.BMP" }
{ "controls_work07.bmp" }
{ "controls_work08.bmp" }
{ "controls_work09.bmp" }
{ "controls_work10.bmp" }
{ "controls_work11.bmp" }
{ "controls_work12.BMP" }
{ "controls_work13.bmp" }
}

// 3 attachment(s)
$attachment 0 "Gauss" -10.000000 2.500000 0.000000
$attachment 1 "Gauss" -10.000000 2.500000 0.000000
$attachment 2 "Gauss" -10.000000 2.500000 0.000000

// 3 hit box(es)
$hbox 10 "Gauss" -13.730000 -1.270000 -6.190000 2.140000 3.620000 9.780000
$hbox 0 "Bip01 L Arm2" 0.000000 -2.800000 -1.730000 11.680000 3.020000 4.000000
$hbox 0 "Bip01 L Hand" 0.000000 -0.870000 -1.940000 3.890000 1.030000 1.790000

// 8 animation sequence(s)
$sequence "idle" "idle" fps 30 
$sequence "fidget" "fidget" fps 30 { event 5004 18 "weapons/egon_idle_switch.wav" } 
$sequence "altfireon" "altfireon" fps 30 { event 5004 1 "weapons/egon_switch1.wav" } 
$sequence "altfireoff" "altfireoff" fps 30 { event 5004 1 "weapons/egon_switch2.wav" } 
$sequence "stop" "stop" fps 10 
$sequence "fire1" "fire1" fps 20 loop 
$sequence "draw" "draw" fps 30 
$sequence "holster" "holster" fps 30 

// End of QC script.
