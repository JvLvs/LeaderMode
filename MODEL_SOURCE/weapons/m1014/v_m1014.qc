/*
==============================================================================

QC script generated by Half-Life MDL Decompiler 1.2 
2003, Kratisto. Based on code from Valve's HL SDK.

v_m1014.mdl

Original internal name:
"v_m1014.mdl"

==============================================================================
*/

$modelname "v_m1014.mdl"
$cd ".\"
$cdtexture ".\"
$scale 2.0
$cliptotextures

$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000


$origin -0.5 -1 0


//reference mesh(es)

$body "hands" "rhand"

$bodygroup "leftHand"
{
studio "lhand"
blank
}

$body "hands" "rsleeve"

$bodygroup "lsleeve"
{
studio "lsleeve"
blank
}

$body "weapon" "m1014"

$bodygroup "bolt"
{
studio "bolt"
studio "bolt_back"
}



$bodygroup "sight"
{
blank
studio "reflexSight"
}

$bodygroup "laser"
{
blank
studio "laser"
}

$bodygroup "shell"
{
studio "shell"
blank
}


// x attachment(s)
$attachment 0 "bone_weapon" -44.600000 5.000000 0.000000
$attachment 1 "bone_weapon" -12.600000 5.000000 -4.000000

// x animation sequence(s)
$sequence "idle" "idle" fps 30 
$sequence "fire" "fire" fps 30 { event 5001 0 "26" } 
$sequence "fire_last" "fire_last" fps 30 { event 5001 0 "26" } 
$sequence "aim_fire" "aim_fire" fps 30 { event 5001 0 "26" } 
$sequence "aim_fire_last" "aim_fire_last" fps 30 { event 5001 0 "26" } 
$sequence "reload_start2" "reload_start2" fps 30 { event 5004 1 "weapons/m1014/m1014_reload_start.wav" } 
$sequence "reload_start_first2" "reload_start_first2" fps 30 {
  { event 5004 1 "weapons/m1014/m1014_reload_start.wav" } 
  { event 5004 19 "weapons/m1014/m1014_shell_takeout.wav" } 
  { event 5004 30 "weapons/m1014/m1014_shell_sideload.wav" } 
  { event 5004 41 "weapons/m1014/m1014_boltrelease.wav" } 
  { event 5004 51 "weapons/m1014/m1014_ready.wav" } 
 }
$sequence "reload_insert2" "reload_insert2" fps 35 { event 5004 1 "weapons/m1014/m1014_shell_takeout.wav" } { event 5004 9 "weapons/m1014/m1014_shell_insert.wav" } 
$sequence "reload_exit2" "reload_exit2" fps 30 { event 5004 1 "weapons/m1014/m1014_reload_end.wav" } 
$sequence "select_first" "select_first" fps 30 {
  { event 5004 1 "weapons/m1014/m1014_deploy.wav" } 
  { event 5004 21 "weapons/m1014/m1014_boltback.wav" } 
  { event 5004 30 "weapons/m1014/m1014_boltrelease.wav" } 
  { event 5004 40 "weapons/m1014/m1014_ready.wav" } 
 }
$sequence "select" "select" fps 30 { event 5004 1 "weapons/m1014/m1014_deploy.wav" } 
$sequence "holster" "holster" fps 30 { event 5004 1 "weapons/m1014/m1014_holster.wav" } 
$sequence "checkMag" "checkMag" fps 30 {
  { event 5004 1 "weapons/m1014/m1014_reload_start.wav" } 
  { event 5004 23 "weapons/m1014/m1014_boltback.wav" } 
  { event 5004 43 "weapons/m1014/m1014_boltrelease.wav" } 
  { event 5004 51 "weapons/m1014/m1014_ready.wav" } 
 }
$sequence "checkMag_empty" "checkMag_empty" fps 30 { event 5004 1 "weapons/m1014/m1014_reload_start.wav" } { event 5004 39 "weapons/m1014/m1014_ready.wav" } 
$sequence "block_up" "block_up" fps 30 
$sequence "block_down" "block_down" fps 30 
$sequence "leftHand_up" "leftHand_up" fps 30 
$sequence "leftHand_down" "leftHand_down" fps 30 
$sequence "dash_enter2" "dash_enter2" fps 30 
$sequence "dash_idle" "dash_idle" fps 30 
$sequence "dash_exit" "dash_exit" fps 30 

// End of QC script.