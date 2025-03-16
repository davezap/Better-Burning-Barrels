////////////////////////////////////////////////////////////////////////////////
DayZ Mod : DaveZ's Better Burning Barrels (DaveZ-BBB)

This mod lets you spawn burning barrels at specific locations that are on fire
and never go out or optionally turn on and off with a variety of configurable
options, such as the time of day, temperature, player proximity.

-- HOW TO INSTALL --

Install the mod to your server in the usual way. Copy the key from the mod
key folder to your DayZServer key folder.

Copy the @BBB/ServerProfiles/DaveZ folder to your DayZServer/ServerProfiles 
folder. This folder contains the main configuration file DaveZConfig.json and 
is where you will be adding all your BARRELS!


-- DaveZConfig.json --

BarrelLocations:
	Is an array of BarrelLocation objects
	

Position [vetcor decimal [x,y,z], required]
	x,y,z coordinates to place the barrel.
	To help you out this mod writes your current position in to your player log 
	every time you press the X key. To find this log on Windows navigate to 
	C:\Users\[yourusername]\AppData\Local\DayZ and open the most recent 
	"script_*****.log" file. Look for the lines that look like this... 
	[BBB] YAY! POS=<3711, 402, 5993> SNAP=<3711, 404, 5993>
	The POS is at your feet, the SNAP is the ground level below you.
	
	If you use another means to get coordinates that does not give the Y coordinate,
	such as iZurvive with Ctrl+C just set y to 0 but read below.


The following optional parameters may are logically OR'd toghether. For example, 
if you specify OnDuringNight=1 and OnPlayerProximity=5 the barrel will burn both
when it is night time and during the day when a player is within 5 meters of it.

DontSnapToGround [0 or 1, defaults to 0]
	The default behaviour is to snap the y (vertical) position of the barrel
	to the ground surface so you can set the y part of Position=0.
	However, inside of buildings where the floor is raised or your on the second
	story the snapping will put the barrel in the foundations so you will want 
	to disable with this option and specify the y value.
	
	Currenly with my limited knowledge of the DayZ codebase I don't know a way to
	get the next y collision point bellow a given y. If any one knows how objects
	in general can be placed on surfaces let me know :)

OnTimeFrom, OnTimeTo [decimal HourMins, defaults to 0, disabled with 0]
	Ignite barrel between starting at OnTimeFrom and ending just prior to
	OnTimeTo. For example if you have OnTimeFrom=17 and OnTimeTo=19 the barrel
	will run starting at 5pm and turn off at 7pm. Times crossing midnight are 
	handled for example OnTimeFrom=21 and OnTimeTo=3 will run for 6 hours
	starting at 9pm.
	Times are decimal calculated as Hour + (Mins / 60), so 6:30am would be 6.5.

OnTempBelow, OnTempAbove [decimal degrees celsius, defaults to 0, disabled with 0]
	OnTempBelow Turns barrel on when temp drops below this value. Can be any 
	positive or negative decimal number. For example 14.9. 
	OnTempAbove likewise does the oposite and is provided even if a nice fire 
	on a hot day is probably not something you want.

OnDuringNight, OnDuringDay [0 or 1, defaults to 0, disabled with 0]
	DayZ provides approximate sunrise and sunset times corrected for the time
	of year, so if OnDuringNight is set to 1 the barrel will light up between
	these times. OnDuringDay does the inverse.

OnPlayerProximity [whole meters, defaults to 0, disabled with 0]
	Barrel will fire up when players are within this many meters of a barrel and
	automatically extinguish when they leave the radius.

Open [0 or 1, defaults to 1]
	Sets the inital state of the barrel lid to be Open (1) or Closed (0)
	
Locked [0 or 1, defaults to 0]
	Locks (1) the barrel lid at it's initial state and prevents the player from
	opening or closing it.



-- Default DaveZConfig.json --

Below is the default json configuration that ships with the mod that places
four barrels at Green Mountain with various options.

{
    "Version": "0.1",
    "BarrelLocations": [
        {
			"OnTimeFrom" : 1,
			"OnTimeTo" : 22,
			"OnDuringDay" : 1,
            "Position": [
                3710.8129,
                402.0,
                5993.5932
            ]
        },
        {
			"OnTempBelow": 14.0,
			"OnDuringNight" : 1,
			"Open": 1,
			"Locked": 1,
            "Position": [
                3709.6999,
                402.0,
                5996.7900
            ]
        },
        {
			"OnPlayerProximity": 3,
            "Position": [
                3708.0,
                402.0,
                5999.0
            ]
        },
		{
			"DontSnapToGround": 1,
			"OnPlayerProximity": 2,
            "Position": [
				3680.2299,
				406.7900,
				5995.12988
			]
		}
    ]
}
