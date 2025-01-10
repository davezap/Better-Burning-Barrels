:: This is the script I use while developing.
:: It makes and signs the PBO from the source using pack.bat and deploys it to
:: the server.
:: Then it runs the server configured with the test PBO called BBB
::   it's called Better Burning Barrels in production.
:: Next we wait for the server to start and open the most recent log file
::
:: TIP: Set TimeLogin=0 in DayZServer\mpmissions\[your mission]\db\globals.xml
::      for faster load times.
::
:: FUNFACT: BBB has no client side scripts so we don't need to copy it there
::   during development. As a result, after packing, you'll only have a @BBB in
::   the server folder. In the production release it's really the launcher that
::   enforces subscribing and downloading the mod on the client side and at that
::   point would complain if they were mismatched.
::

@echo off
:start
::Server name
set serverName="DaveZ"
::Server files location
set serverPath=C:\Program Files (x86)\Steam\steamapps\common\DayZServer
set serverLocation="C:\Program Files (x86)\Steam\steamapps\common\DayZServer\dayZServer_x64"
set BECLocation="C:\Program Files (x86)\Steam\steamapps\common\DayZServer\battleye"
set textEditor=C:\Program Files\Notepad++\notepad++.exe

::Server Port
set serverPort=2302
::Server config
set serverConfig=serverDZ.cfg
::Logical CPU cores to use (Equal or less than available)
set serverCPU=4
::Sets title for terminal (DONT edit)
title %serverName% batch
::DayZServer location (DONT edit)
::cd "%serverPath%"
echo (%time%) %serverName% started.

call P:\BBB\pack.bat  || goto :error

echo Copy package to server
rd /S /Q "%serverPath%\@BBB\"
xcopy /S /C /Q /Y "P:\BBB\release\" "%serverPath%\@BBB\"


echo Launching server...
cd "%serverPath%"
:: Launch parameters (edit end: -config=|-port=|-profiles=|-doLogs|-adminLog|-netLog|-freezeCheck|-filePatching|-BEpath=|-cpuCount=)
:: Production name @Better Burning Barrels
start "DayZ Server" /min "DayZServer_x64.exe" -config=%serverConfig% -port=%serverPort% "-mod=@CF;@VPPAdminTools;@BBB" "-profiles=ServerProfiles" -cpuCount=%serverCPU% -dologs -adminlog -netlog -freezecheck
:: ALL mods Server diag mode.
::start "DayZ Server" /min "DayZDiag_x64.exe" -server -config=%serverConfig% -port=%serverPort% "-mod=@CF;@VPPAdminTools;@BBB" "-profiles=ServerProfiles" -cpuCount=%serverCPU% -dologs -adminlog -netlog -freezecheck

:: Wait for log file to be created. 
@timeout 5 /nobreak
FOR /F "eol=| delims=" %%I IN ('DIR "%serverPath%\ServerProfiles\*.log" /A-D /B /O-D /TW 2^>nul') DO (
    SET "NewestFile=%%I"
    GOTO FoundFile
)
ECHO No *.log file found!
GOTO :error

:FoundFile
ECHO Newest *.log file is: "%NewestFile%"
call "%textEditor%" "%serverPath%\ServerProfiles\%NewestFile%"


echo "Press enter to shutdown."
pause
echo "Shutting down."
taskkill /im DayZServer_x64.exe /F
::Time in seconds to wait before..
timeout 10

exit


:error
echo "Something happened! ¯\_(ツ)_/¯"
pause
