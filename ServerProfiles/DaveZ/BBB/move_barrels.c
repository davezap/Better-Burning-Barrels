/* 
Move Barrels 
DemonstratesShows how 
	- to relocate barrel 0, loaded from JSON settings when ever 
	  OnPlayerProximity is triggered.
	- override settings in default logic
	- alternate colour of a barrel every 10 seconds.

Given this setup we will move the barrel between two locations when the player
is within OnPlayerProximity meters of the barrel.
{
    "Version": "0.5",
    "BarrelLocations": [
        {
			"OnPlayerProximity":2,
            "Open": 0,
            "Position": [
                3710.81,
                402.0,
                5993.59
            ]
        }
    ]
}
*/

vector orgionalPlace;
bool atAtNewPlace;
ref BBB_Settings bbbSettings;

void ChangeColour()
{
	BBB_BarrelLocation barrel = bbbSettings.GetBarrel(0);
	if(barrel) {
		if(barrel.GetColour()=="Yellow")
		{
			BBB_Log.LogEx("[DZS] Set Colour Red");
			barrel.SetColor("Red");
		} else {
			BBB_Log.LogEx("[DZS] Set Colour Yellow");
			barrel.SetColor("Yellow");
		}
	}
}

void Init(BBB_Settings settings)
{
	// User our logging class.
	BBB_Log.Log("[DZS] init()");
	
	// Keep a refference to the settings as we will not be given it again.
	bbbSettings = settings;
	
	// Change colour every 10 seconds.
	GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ChangeColour, 10000, true);
	
	// At start-up get the origional position of Barrel[0]
	BBB_BarrelLocation barrel = bbbSettings.GetBarrel(0);
	if(barrel)
	{
		orgionalPlace = barrel.GetPosition();
		BBB_Log.Log("[DZS] Got Barrel 0 position " + orgionalPlace);
	}
	
}

bool ShouldFire(BBB_BarrelLocation barrel, int interval)
{
	// This line is required. It's used by the script loader to check 
	// if you have implemented this handler.
	if(!barrel) 
	{
		BBB_Log.Log("[DZS] I implement ShouldFire");
		return true;
	}
		
	if(barrel.GetID() == 0)
	{
		if(barrel.TestOnPlayerProximity())
		{
			vector position;
			if (atAtNewPlace) {
				position = orgionalPlace;
				atAtNewPlace = false;

			} else {
				position[0] = 3700.71;
				position[1] = 402.01;
				position[2] = 5993.48;
				atAtNewPlace = true;
			}

			BBB_Log.LogEx("[DZS] Moving Barrel to " + position);
			barrel.SetPosition(position);
			
			return barrel.ShouldFireIgnore(BBB_EIgnore.PLAYERPROXIMITY);
		} else {
			return barrel.ShouldFire();
		}
	} else {
		return barrel.ShouldFire();
	}
}