:: Packing script peepers our mod for publishing to the steam store or for
:: local testing.
::
:: NOTE: This does not binarize and only handles .c files currently.
::

@echo off
setlocal enableDelayedExpansion

:start
set modname="Better Burning Barrels"

:: This would be bad.
IF "%TMP%"=="" GOTO :error
IF "%TMP%"=="""" GOTO :error
set tmpfolder=%TMP%\BBB
set DayZtools=C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\bin
::set app_binarizer="%DayZtools%\Binarize\binarize.exe"

set ProjectDir=P:\BBB
set ExcludeLst=P:\BBB\pack_exclude.lst
set PrivateKey=P:\Mods\@DaveZ\keys\DaveZ.biprivatekey
set PublicKey=P:\Mods\@DaveZ\keys\DaveZ.bikey
:: !! Warning: Everything in here will be nuked.
set ReleaseDir=P:\BBB\release


echo Clearing temp folder : %tmpfolder%

rd /S /Q %tmpfolder%
mkdir %tmpfolder%
echo Copying "%ProjectDir%\*.c" "%tmpfolder%\*.c"
:: My project only has .c files.
xcopy /S /C /Q /Y /exclude:%ExcludeLst% "%ProjectDir%\*.c" "%tmpfolder%\*.c"
::Other types might include *.emat;*.edds;*.ptc;*.c;*.imageset;*.layout;*.ogg;*.png;*.paa;*.rvmat;*.wrp

echo CfgConvert
"%DayZtools%\CfgConvert\CfgConvert.exe" -bin -dst "%tmpfolder%\scripts\config.bin" "%ProjectDir%\scripts\config.cpp"

:: there is nothing to binarize

echo FileBank
"%DayZtools%\PboUtils\FileBank.exe" -exclude "%ExcludeLst%" -property "product=dayz ugc" -property "prefix=BBB" -dst "%TMP%" "%tmpfolder%" || goto :error

echo Building release directory.
rd /S /Q %ReleaseDir%
mkdir %ReleaseDir%
mkdir %ReleaseDir%\keys
mkdir %ReleaseDir%\addons
copy "%ProjectDir%\README.md" "%ReleaseDir%\" || goto :error
copy "%PublicKey%" "%ReleaseDir%\keys\" || goto :error
xcopy /S /C /Q /Y "%ProjectDir%\ServerProfiles\" "%ReleaseDir%\ServerProfiles\"
copy "%TMP%\BBB.pbo" "%ReleaseDir%\addons\BBB.pbo" || goto :error

:: Sign file.
echo Signing PBO
"%DayZtools%\DsUtils\DSSignFile.exe" "%PrivateKey%" "%ReleaseDir%\addons\BBB.pbo" || goto :error

echo Packing Complete.
goto fin

:error
echo "Something happened while packing! ¯\_(ツ)_/¯"
pause
exit -1073741510
:fin