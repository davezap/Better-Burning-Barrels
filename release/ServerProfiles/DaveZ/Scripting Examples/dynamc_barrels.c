void Init(DaveZSettings settings)
{
	// User our logging class.
	BBBLog.Log("[DZS] init()");
	
	// Create a new Green Barrel at Green Mountain.
	BBB_BarrelLocation newBarrel = new BBB_BarrelLocation("3706.2619 0 5987.6875");
	newBarrel.SetOnDuringDay(true);
	newBarrel.SetOpen(false);
	newBarrel.SetColor("Green");
	settings.AddBarrel(newBarrel);
	
}

// All handlers are optional. If you exclude ShouldFire BBB will use default 
// logic.
/*
bool ShouldFire(BarrelLocation barrel, int interval)
{
	// This line is required. It's used by the script loader to check 
	// if you have implemented this handler.
	if(!barrel) return true;

	// This line runs the default BBB logic. We return True if we want the 
	// Barrel to fire up and False to extinguish it.
	return barrel.ShouldFire();
}
*/