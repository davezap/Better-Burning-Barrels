
# DayZ Mod : DaveZ's Better Burning Barrels (BBB)

Visit the discord server to report bugs, ask questions or just say hello.
	https://discord.gg/tvrFAMgUtt

Repacking of this mod is now permitted, please read terms on the discord.

The project is now open source and can be found here
	https://github.com/davezap/Better-Burning-Barrels


<!-- ----------------------------------------------------------------------- -->
# ABOUT

Better Burning Barrels is the most unnecessarily complicated barrel mod for DayZ. 
This mod lets you add burning barrels at specific locations that are on fire
and never go out or optionally turn on/off with a variety of configurable
options such as, the time of day, temperature, player proximity or even your own
scripted logic.

You can also whitelist player ID's to create barrels that only turn on for your
teammates to create interesting capture-the-flag scenarios.

It's probably easier to talk about the things Better Burning Barrels cannot do!


<!-- ----------------------------------------------------------------------- -->
# WHAT's NEW

It's been over a year and I'm back at it! Decided it was long overdue an update.

Version 0.50 - 4 Jan 2025
- You can now add Fireplace's (aka campfire) see options for barrel type. 
  Somewhat breaking from the theme, a rebranding might be in order.
- Fixed the VERY ANNOYING sound bug \o/ previous versions the barrel open or 
  close sound effect would play every few seconds (depending on the 
  open/closed state of the barrel) after lighting. These sounds will now 
  only play with player interaction and not on any other event.
- Fixed ListModeParamA and ListModeParamB not working if the value was 1.
- Fixed firewood not being replenished after fire goes out. This made fireplaces
  without stone circles effectively disappear
- Fixed bug where new barrels near lots of other objects would fail to spawn in
  thanks to Daxst3r for reporting this.
- JSON loader, I wrote a JSON parser to replace the DayZ one. You don't
  need to make any changes to your BBBConfig.json. The technical reasoning
  can be found at the top of BBB_Json.c for those who are interested.
- Removed extraneous debugging from JSON loader.
- Patched a 'NULL pointer to instance' where I was trying to get player info
  after logout. This happened when players skipped the countdown.
- Also just a bunch of testing to ensure everything still working with the
  latest DayZ release (1.26 as of now)

See end of this document for previous change history.


<!-- ----------------------------------------------------------------------- -->
# HOW TO INSTALL

You will need to have DayZ and DayZServer ready to go, I can't help with that.

Then install the mod to your server in the usual way :) ha, everyone says that.
No but really, for those that don't yet know.
1. Go to Steam / Library / DayZ / Workshop
2. Search for "Better Burning Barrels"
3. That's me wearing a hard-hat and high-viz
4. Click the + icon to subscribe to the mod.
5. Steam will update your DayZ install to include the mod, so close it down.
6. Explore your way here (being the default)
   ``` C:\Program Files (x86)\Steam\steamapps\common\DayZ\!Workshop ```
7. Copy the "@Better Burning Barrels" folder.
   Now explore to the server
   ``` C:\Program Files (x86)\Steam\steamapps\common\DayZServer ```
8. Paste the folder here (there is no !Workshop folder the mods just go in
   the server root.

Then...
1. Copy the file @Better Burning Barrels\keys\DaveZ.bikey
   To the DayZServer\keys folder.
2. Copy the @Better Burning ``` Barrels\ServerProfiles\DaveZ ``` folder 
   To the ``` DayZServer\ServerProfiles ``` folder. 
   This folder contains the BBBConfig.json and is where you will be adding
   all your BARRELS along with any scripts.
   The DaveZ/BBB folder contains documentation and scripting 
   examples (optional).
3. Configure your server startup script to load the mod. Most people have a 
   .bat file with something like this.
   ``` start "DayZ Server" /min "DayZServer_x64.exe" -config=%serverConfig% -port=%serverPort% "-mod=@CF;@VPPAdminTools;@BBB" ``` 
	  

Runtime debug information is available via the files with the prefix
``` DayZServer/ServerProfiles/script_***.log ```
The relevant lines are prefixed with "[BBB]"


<!-- ----------------------------------------------------------------------- -->
# BBBConfig.json configuration

Take care to not break the JSON file by forgetting quotes or commas. It's easy 
to do. Recommend you use a tool like https://jsonlint.com/ to validate your file
after you make changes to it.

The following is a description of all the options currently available. 

### Version:"0.50" 
**!!Do not change this!!**

### BarrelUpdateTime [whole seconds, defaults to 5]
Test player location to barrels every BarrelUpdateTime seconds.
The more frequently the faster barrels will react but this may increase load
on the server (not that it's really much of a load)

### DebugBarrels  [0 or 1, 0 by default]
Setting to 1 prints additional debug information during run time. This is 
very helpful for me to receive, it really helps diagnose problems. The
default debugging is minimal and more for your benefit.
	
### DebugWeather  [whole seconds, disabled by and defaults to 0]
Prints to the server log environmental information that is useful for 
configuring Barrels.
Log example ...
```[BBB] Sunrise 5:14, Sunset 18:46, Temp 11.6051, DateTime 2023 9 28 20:11 daytime=false```
	
### DebugYAY [whole seconds, defaults to 0]
Every DebugYAY prints a list of all players and locations on the server.
``` YAY 1 Players | DaveZ (76561198113121974) @ <3715.817871, 402.012451, 6000.250488> | ...```

### BarrelLocations [array of BarrelLocation]
The array of BarrelLocation objects, described next.


<!-- ----------------------------------------------------------------------- -->
## BarrelLocation

### Type 	[string "Fireplace" or "Barrel" default]

### Name [string]
This is an optional name for the barrel, this is only printed to the logs. If 
you don't give a name the logs will just show a list index number with the first
in the config being #0

### Position [vector decimal [x,y,z], The only required parameter]
x,y,z coordinates to place the barrel.

**HINT**: To help you determine barrel locations DebugYAY writes plater positions
to the server log.

If you use another means to get coordinates that do not give the Y (height)
coordinate,	such as iZurvive just set y to 0, see DontSnapToGround.

**NOTE** : Do not place your regular in game barrels or fireplaces within 
1 metre of one ours. The mod deletes all others within this range for 
housekeeping purposes.


### DontSnapToGround [0 or 1, 0 by default]
The default behavior is to snap the y (vertical) position of the barrel	to
whatever surface is directly below. If you leave DontSnapToGround=0 (default) 
and specify a Y coordinate BBB will cast a ray from +0.5m down to the next
surface and snap the barrel to that, if you set Y=0 BBB will instead cast a
ray from 1000 meters and snap to the first thing it hits (the ground or top of 
a structure what ever comes first). The reason for the +0.5 meters is that tools
like VPP that copy the position to clipboard with 'P' key tend to be exactly on
the floor inside buildings and barrels can end up snapping to the ground below
the floor.
	
If you want to force a specific Y coordinate you need to set DontSnapToGround=1. 
For instance, where you want floating Barrels or there is some problem with the
surface snapping.


### LogPlayerProximity [whole meters, 0 is default and disabled]
Additionally record information about players proximity to this specific 
barrel. This is independent from OnPlayerProximity


<!-- ----------------------------------------------------------------------- -->
## Barrel Specific Options

These options only relate to barrels and will be ignored by fireplaces.

### Open [0 or 1 (default)]
Sets the initial state of the barrel lid to be Open (1) or Closed (0)
	
### Locked [0 (default) or 1]
Locks (1) the barrel lid to its initial state, preventing the player from
changing it via the normal interaction.

### Color / Colour [string "red", "blue", "green", "yellow", default is "red"]
Pick from a variety of vibrant barrel colours to beautify your base.
Available options for your choosing are blue, green, red and yellow.

<!-- ----------------------------------------------------------------------- -->
## Fireplace Type Options

Conversely, these options only relate to fireplaces and will be ignored by 
barrels.

### Tripod [true or false (default)]
Deploys a cooking tripod over the fire.

### Circle [true or false (default)]
Constructs a stone circle around the fire.


<!-- ----------------------------------------------------------------------- -->
## Standard Conditional Options

The following parameters are logically OR'd together. For example, if you 
specify OnDuringNight=1 and OnPlayerProximity=5 the barrel will burn when either
condition is true.


### OnTimeFrom, OnTimeTo [decimal HourMins, 0 is default and disabled]
Ignite the barrel starting at OnTimeFrom and ending just prior to OnTimeTo. 
For example, if you have OnTimeFrom=17 and OnTimeTo=19 the barrel will burn 
starting at 5 pm and turn off at 7 pm. Times crossing midnight are 
handled for example OnTimeFrom=21 and OnTimeTo=3 will run for 6 hours
starting at 9 pm. Times are decimal calculated as Hour + (Mins / 60), 
so 6:30 am would be 6.5.

### OnTempBelow, OnTempAbove [decimal degrees Celsius, 0 is default and disabled]
OnTempBelow Turns the barrel on when temp drops below this value. Can be any 
positive or negative decimal number. For example 14.9. 
OnTempAbove likewise does the opposite and is provided if a fire is wanted
only on nice sunny days.

### OnDuringNight, OnDuringDay [0 or 1, 0 is default and disabled]
DayZ provides approximate sunrise and sunset times corrected for the time
of year, so if OnDuringNight is set to 1 the barrel will light up between
these times. OnDuringDay does the inverse.

### OnPlayerProximity [whole meters, 0 is default and disabled]
Barrel will fire up when players are within this many meters of a barrel and
automatically extinguish when they leave the radius.


<!-- ----------------------------------------------------------------------- -->
## Player List Conditional Options

Each Barrel has two lists ListPlayersA (the ON list) and ListPlayersB (the OFF
list) that you can populate with players Steam ID's. A variety of logical 
conditions can be then used to determine if the barrel should be burning or 
extinguished based on players from either team proximity to it.

### ListProximityA [whole meters, 0 is default and disabled]
### ListPlayersA   [Array of String of Player Steam ID's]
### ListModeParamA [whole number]
Barrel is ON if at least ListModeParamA members of ListPlayersA are within 
the radius ListProximityA.

### ListProximityB [whole meters, 0 is default and disabled]
### ListPlayersB [Array of String of Player Steam ID's]
### ListModeParamB [whole number]
Barrel is OFF if at least ListModeParamB members of ListPlayersB are within 
the radius ListProximityB.

### ListMode [whole number, 0 is default]
This option modifies the ordering and priority of Standard Conditionals and
Player List Conditionals, where  
0 = ListModeParamB Barrel OFF state overrides ListProximityA Barrel ON state.
Both override all other Barrel "On..." settings.  
1 = ListModeParamA Barrel ON state overrides ListProximityB Barrel OFF state.
Both override all other Barrel "On..." settings.  
2 = As 0 except Barrel "On..." settings override both if true.  
3 = As 1 except Barrel "On..." settings override both if true.  
4 = As 0 except Barrel "On..." settings override both if false.  
5 = As 1 except Barrel "On..." settings override both if false.  


<!-- ----------------------------------------------------------------------- -->
# BBB.c User Enforce Script

Since Version 0.3 you can now write server side Enforce script that is dynamically loaded by Better Burning Barrels and will allow server owners to write their own Barrel Logic. This was done after I came to the realisation that the ListMode stuff was too complex and didn't even cover all the possible combinations and server owners will likely have a never-ending list of options for me to add :)

To be loaded the script must be called BBB.c and be located in your 
``` DayZServer/ServerProfiles/DaveZ ``` folder alongside your BBBConfig.json file.
Some example scripts have been added to the mod folder.
``` @Better Burning Barrels/ServerProfiles/DaveZ/BBB/ ```

Comments in that script give the basics, additional details are on Discord.

The script is run server-side and has access to all DayZ global Classes and 
proto Functions. The parent module is GetGame().GetMission().MissionScript.

A basic primer to Enforce script is here. 
https://community.bistudio.com/wiki/DayZ:Enforce_Script_Syntax#Primitive_Types

The DayZ Code Explorer by Zeroy is the most amazing resource for DayZ scripters.
https://dayzexplorer.zeroy.com/


<!-- ----------------------------------------------------------------------- -->
# Config Examples

Here are some example barrel configurations for BBBConfig.json

## Heating but no cooking with Standard Conditionals

The following BarrelLocation will fire up when the temperature drops below
14 degrees or at night time. The barrel has its lid open and is locked so
players cannot close it. This means they are unable to cook on it. 
```
{
  "Version": "0.5",
  "BarrelUpdateTime": 10,
  "BarrelLocations": [
    {
      "OnTempBelow": 14.0,
      "OnDuringNight": 1,
      "Locked": 1,
      "Open": 1,
      "Position": [3710.82, 402.01, 5996.32]
  }
  ]
}
```

## Capture-the-flag with Player List Conditionals

Let's consider a capture-the-flag style event where the goal is for TeamA to 
keep their fire burning while TeamB's objective is to extinguish it.

The following BarrelLocation object says that as long as one member of TeamA 
remains within 100 meters of the barrel it will remain on, however, if 2 members 
if TeamB get within 10 meters of the barrel it will go out.

The Barrel is anyway always on at night and so TeamB cannot win by attacking at 
this time.

```
{
  "Version": "0.5",
  "BarrelUpdateTime": 1,
  "DebugBarrels": 1,
  "BarrelLocations": [
    {
      "Colour": "Red",
      "OnDuringNight": 1,
      "ListMode": 2,
      "ListProximityA": 100,
      "ListModeParamA": 1,
      "ListPlayersA": [
        "11111111111111111",
        "22222222222222222",
        "33333333333333333"
      ],
      "ListProximityB": 10,
      "ListModeParamB": 2,
      "ListPlayersB": [
        "44444444444444444",
        "55555555555555555",
        "66666666666666666"
      ],
      "Position": [3710.81,402.0,5993.59]
    }
  ]
}
```

Server admins can see the following entry in the logs indicating a win for 
TeamB.

```[BBB] ID:0 extinguish.```

NOTE: The example used ListMode=2. If ListMode=0 was used then the barrel would
stay lit at night even if all TeamA members left the area. There are many
different logical combinations to be had here. I'll put more examples on 
Discord.


<!-- ----------------------------------------------------------------------- -->
# Default BBBConfig.json

Below is the default json configuration that ships with the mod that places
four barrels at Green Mountain with various options.

Note: our new showroom is located in the big shed in Kamyshovo!

```
{
  "Version": "0.50",
  "BarrelUpdateTime": 1,
  "DebugBarrels": 1,
  "DebugWeather": 60,
  "DebugYAY": 60,
  "BarrelLocations": [
    {
      "Name": "B0_RED",
      "Colour":"Red",
      "OnPlayerProximity":3,
      "Open": 1,
      "Position": [12075.33, 3.089654, 3456.172119]
    },
    {
      "Name": "B1_YLW",
      "Colour":"Yellow",
      "OnPlayerProximity":3,
      "Open": 0,
      "Position": [12073.12, 3.12, 3455.68]
    },
    {
      "Name": "B2_GRN",
      "Colour":"Green",
      "OnPlayerProximity":3,
      "Open": 0,
      "Position": [12069.923828, 3.078028, 3454.740234]
    },
    {
      "Name": "B3_BLU",
      "Colour":"Blue",
      "OnPlayerProximity":3,
      "Open": 1,
      "Position": [12078.101563, 3.096107, 3457.109619]
    },
    {
      "Name":"F4",
      "Type":"Fireplace",
      "Tripod": true,
      "Circle": true,
      "OnPlayerProximity":10,
      "Position": [12070.697266, 3.116442, 3451.480469]
    },
    {
      "Name":"F5",
      "Type":"Fireplace",
      "OnPlayerProximity":3,
      "Position": [12071.165039, 3.063281, 3448.481934]
    },
    {
      "Name": "B6_RED",
      "Colour":"Red",
      "Open": 1,
      "Locked": 1,
      "OnDuringNight": 1,
      "Position": [12074.443359, 3.049184, 3441.508301]
    },
    {
      "Name": "B7_BLU",
      "Colour":"Blue",
      "Open": 1,
      "Locked": 1,
      "OnDuringDay": 1,
      "LogPlayerProximity": 10,
      "Position": [12081.784180, 3.053838, 3443.565918]
    },
    {
      "Name": "B8_GRN",
      "Colour":"Green",
      "Open": 1,
      "Locked": 1,
      "Position": [12079.044922, 3.034917, 3436.718750]
    }
  ]
}
```

<!-- ----------------------------------------------------------------------- -->
# FULL CHANGE LOG

This log notes changes to the first Github release for version 0.05.

## Version 0.45 - 29 July 2023
- Fixed bug where all items (including meat and pots) were be deleted from 
  Barrel inventory when it was extinguished. This wont happen now.
- Finally figured out how to get the intersecting surface below the barrel.
  so snapping now works inside buildings and on rooftops. 
  See DontSnapToGround for more details.

## Version 0.4 - 28 July 2023
- Major internal refactoring to improve script support added last version
  Writing documentation of BBB classes for user scripts, now available in...
  ```@BBB\ServerProfiles\DaveZ\BBB\documentation```
  Writing example user scripts, now available in...
  ``` @BBB\ServerProfiles\DaveZ\BBB ```
- BBB_Settings can now dynamically add/remove Barrels.
- BBB_BarrelLocation can now dynamically SetPosition() and SetColor().
- Players are immediately now removed from Barrel when they leave Proximity.
- Added support for the American spelling of Color, you're welcome.
- Fixed bug where Barrels with only position and no other settings would not 
  fire up.
	
## Version 0.3 - 22 July 2023
- Added user injectable Enforce script BBB.c for doing your own Barrel Logic.
- Barrel processing is now done on the server-side.
- Removed all client-side code.
- No longer dependent on Community Framework or any other mod.
- Improved server-side efficiency. Players are tested every 30 seconds unless
  near the barrel then they are tested every BarrelUpdateTime.
- PlayerReportTime has been deprecated
- ProximityTimeout has been deprecated
- DebugYAY has been modified for server-side operation.
- [BBB] log entries are now time-stamped using the server time zone.
- BarrelUpdateTime has been added.
- Forgot to actually implement ListModeParamA and ListModeParamB in 
  previous version :)

## Version 0.3 - 16 July 2023
- initial release to Steam.