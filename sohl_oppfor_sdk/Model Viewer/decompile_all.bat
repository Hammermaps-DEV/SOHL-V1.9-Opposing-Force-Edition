@echo off

echo Half-Life MDL Decompiler 1.2
echo 2003, Kratisto. Based on code from Valve's HL SDK.
echo --------------------------------------------------
echo Decompiling all HL *.mdl models in current folder.
echo Please wait...

rem for %%I in (*.mdl) do mdldec.exe  %%I .\%%~nI

if exist mdldec_log.txt del mdldec_log.txt
for %%I in (*.mdl) do mdldec.exe  %%I .\%%~nI >>mdldec_log.txt

echo Created output log, see mdldec_log.txt 
echo Done.
echo --------------------------------------------------
echo.

pause