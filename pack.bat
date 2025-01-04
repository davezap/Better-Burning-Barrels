@echo on
setlocal enableDelayedExpansion

:start
set modname="Better Burning Barrels"

set tmpfolder=%TMP%
:: This would be bad.
IF "%tmpfolder%"=="" GOTO :error
IF "%tmpfolder%"=="""" GOTO :error


set app_binarizer="C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\bin\Binarize\binarize.exe"

echo Clearing temp folder.

rd /S /Q %tmpfolder%\bbb
mkdir %tmpfolder%\bbb
echo Syncing folders... [source]=P:\BBB, [sync]=%tmpfolder%\bbb
:: My project only has .c files.
xcopy /S /C /Q /Y "P:\BBB\*.c" "C:\Users\David\AppData\Local\Temp\bbb\*.c"
::Other types might include *.emat;*.edds;*.ptc;*.c;*.imageset;*.layout;*.ogg;*.png;*.paa;*.rvmat;*.wrp

"C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\bin\CfgConvert\CfgConvert.exe" -bin -dst "%tmpfolder%\bbb\scripts\config.bin" "P:\BBB\scripts\config.cpp"

:: there is nothing to binarize

echo FileBank

"C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\bin\PboUtils\FileBank.exe" -exclude "P:\BBB\pack_exclude.lst" -property "product=dayz ugc" -property prefix=BBB -dst "%tmpfolder%" "%tmpfolder%\bbb" || goto :error

echo build release directory.

rd /S /Q P:\BBB\release
mkdir P:\BBB\release
mkdir P:\BBB\release\keys
mkdir P:\BBB\release\addons
copy "P:\BBB\README.md" "P:\BBB\release\" || goto :error
copy "P:\Mods\@DaveZ\keys\DaveZ.bikey" "P:\BBB\release\keys\" || goto :error
xcopy /S /C /Q /Y "P:\BBB\ServerProfiles\" "P:\BBB\release\ServerProfiles\"

copy "%tmpfolder%\BBB.pbo" "P:\BBB\release\addons\BBB.pbo" || goto :error

:: Sign file.
"C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\Bin\DsUtils\DSSignFile.exe" "P:\Mods\@DaveZ\keys\DaveZ.biprivatekey" "P:\BBB\release\addons\BBB.pbo" || goto :error

goto fin

:error
echo "Something happened! ¯\_(ツ)_/¯"

:fin