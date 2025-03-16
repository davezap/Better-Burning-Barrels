@echo on
:start
::Server name Arab zoon
set serverName="DaveZ"
::Server files location
set serverPath="C:\Program Files (x86)\Steam\steamapps\common\DayZServer\"
set serverLocation="C:\Program Files (x86)\Steam\steamapps\common\DayZServer\dayZServer_x64"
set BECLocation="C:\Program Files (x86)\Steam\steamapps\common\DayZServer\battleye"
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
::Launch parameters (edit end: -config=|-port=|-profiles=|-doLogs|-adminLog|-netLog|-freezeCheck|-filePatching|-BEpath=|-cpuCount=)
:: @Code Lock;@BaseBuildingPlus
:: Minimal mods.
::start "DayZ Server" /min "DayZServer_x64.exe" -config=%serverConfig% -port=%serverPort% "-mod=@CF;@ZomBerry Admin Tools;@VPPAdminTools;@BBB" "-profiles=ServerProfiles" -cpuCount=%serverCPU% -dologs -adminlog -netlog -freezecheck
:: All mods.

::copy "P:\BBB\release\addons\*" "C:\Program Files (x86)\Steam\steamapps\common\DayZServer\@BBB\addons\*" || goto :error

echo Binarizing Done

cd "%serverPath%"
:: @BuildEverywhere;@SkyZ - Skybox Overhaul;@Dabs Framework;@DayZ Editor Loader
:: @Heatmap;@BuilderItems;@TraderPlus; @ZomBerry Admin Tools
:: @Better Burning Barrels
:: start "DayZ Server" /min "DayZServer_x64.exe" -config=%serverConfig% -port=%serverPort% "-mod=@CF;@VPPAdminTools;@BBB" "-profiles=ServerProfiles" -cpuCount=%serverCPU% -dologs -adminlog -netlog -freezecheck
start "DayZ Server" /min "DayZServer_x64.exe" -config=%serverConfig% -port=%serverPort% "-mod=@CF;@VPPAdminTools;@Better Burning Barrels" "-profiles=ServerProfiles" -cpuCount=%serverCPU% -dologs -adminlog -netlog -freezecheck
:: ALL mods Server diag mode.
::start "DayZ Server" /min "../DayZ/DayZDiag_x64.exe" -server -filePatching -config=%serverConfig% -port=%serverPort% "-profiles=ServerProfiles" -cpuCount=%serverCPU% -dologs -adminlog -netlog -freezecheck

:: We are using messages.xml to shutdown server at the 4 hour mark.
:: If that does not happen then we will force it to shutdown 2 minutes later.
echo "Running timeout. Press Ctrl+C to cleanly shutdown early."
timeout 14520
taskkill /im DayZServer_x64.exe /F
::Time in seconds to wait before..
timeout 10
::Go back to the top and repeat the whole cycle again
goto start


:error
echo "Something happened! ¯\_(ツ)_/¯"