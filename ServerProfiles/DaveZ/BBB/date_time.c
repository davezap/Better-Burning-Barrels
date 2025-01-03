void SetTheTime()
{
	BBB_EH.SetGameTime("22/07/23 15:33");
}

void Init(BBB_Settings settings)
{
	// User our logging class.
	BBB_Log.Log("[DZS] init()");
	
	// your init.c mission file sets the time after the economy loads.. so 
	// wait 60 seconds to override.
	GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SetTheTime, 60000, false);
	
	// Do it now so we can work with the date/time we want while in this function.
	SetTheTime();
	
	if(BBB_EH.GameTimeBetween("20/07/23 12:50", "25/07/23 12:50"))
	{
		BBB_Log.LogEx("Is Between");
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

	
	return barrel.ShouldFire();
}