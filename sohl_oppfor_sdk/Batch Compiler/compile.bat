@echo off
set WADROOT=D:\Program Files (x86)\Steam\SteamApps\common\Half-Life\spirit\devkit\wads
echo ###################################################
echo #                 Batch  Compiler                 #
echo ###################################################
echo ###################################################
echo #  Please report bugs to: ryansgregg@hotmail.com  #
echo ###################################################
echo.
echo Written At: 10/13/2013 12:26 AM
echo BC Version: 3.1.2.0
echo.
if ERRORLEVEL 1 goto failed
if ERRORLEVEL 1 goto failed
"D:\Program Files (x86)\Steam\SteamApps\common\Half-Life\spirit\devkit\tools\ZHLT\x64(64bit)\hlcsg.exe" -wadautodetect -cliptype Precise -chart -estimate -texdata 8192 -threads 4 "D:\Program Files (x86)\Steam\SteamApps\common\Half-Life\spirit\devkit\maps\opposing_force\of1a5_new"
if ERRORLEVEL 1 goto failed
"D:\Program Files (x86)\Steam\SteamApps\common\Half-Life\spirit\devkit\tools\ZHLT\x64(64bit)\hlbsp.exe" -chart -estimate -texdata 8192 -threads 4 "D:\Program Files (x86)\Steam\SteamApps\common\Half-Life\spirit\devkit\maps\opposing_force\of1a5_new"
if ERRORLEVEL 1 goto failed
"D:\Program Files (x86)\Steam\SteamApps\common\Half-Life\spirit\devkit\tools\ZHLT\x64(64bit)\hlvis.exe" -fast -chart -estimate -texdata 8192 -threads 4 "D:\Program Files (x86)\Steam\SteamApps\common\Half-Life\spirit\devkit\maps\opposing_force\of1a5_new"
if ERRORLEVEL 1 goto failed
if ERRORLEVEL 1 goto failed
if ERRORLEVEL 1 goto failed
if ERRORLEVEL 1 goto failed
if ERRORLEVEL 1 goto failed
goto succeeded
:failed
echo.
echo There was a problem compiling your map, check your of1a5_new.log file for errors.
:succeeded
echo.
if exist "D:\Program Files (x86)\Steam\SteamApps\common\Half-Life\spirit\devkit\maps\opposing_force\of1a5_new.log" "C:\Windows\system32\notepad.exe" "D:\Program Files (x86)\Steam\SteamApps\common\Half-Life\spirit\devkit\maps\opposing_force\of1a5_new.log"
