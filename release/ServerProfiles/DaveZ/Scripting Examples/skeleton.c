void Init(DaveZSettings settings)
{
	// User our logging class.
	BBBLog.Log("[DZS] init()");
	
}

bool ShouldFire(BarrelLocation barrel, int interval)
{
	// This line is required. It's used by the script loader to check 
	// if you have implemented this handler.
	if(!barrel) return true;

	// This line runs the default BBB logic. We return True if we want the 
	// Barrel to fire up and False to extinguish it.
	return barrel.ShouldFire();
}
