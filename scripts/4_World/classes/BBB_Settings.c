//! DayZ Mod : DaveZ's Better Burning Barrels (AKA BBB)
//! 
//! Visit the discord server to report bugs, ask questions or just say hello.
//! 	https://discord.gg/AqDzhMJr
//! 
//!   ******** Repacking is not permitted without prior approval ********
//! 
//! Note this class breaks from Enforce scripting conventions in that we use
//! PascalCase with no prefix for member names restored from the JSON config.
//!
//! DO NOT call methods starting with _ from user code.
//!
//! Loads config and enables adding of the Barrels.

const string BBB_CURRENT_VERSION = "0.50";
const string DAVEZ_CONFIG_ROOT_SERVER = "$profile:DaveZ\\";
const string BBB_CONFIG = DAVEZ_CONFIG_ROOT_SERVER + "BBBConfig.json";

// Note this class breaks from Enforce scripting conventions in that we use
// PascalCase with no prefix for member names restored from the JSON config.

class BBB_Settings: BBB_JsonMap
{
	// Start of Config members // 
    private string Version = "";
    private ref array<ref BBB_BarrelLocation> BarrelLocations;
	ref BBB_BarrelLocation BarrelLocation;
	private int PlayerReportTime;	// Deprecated - Keep to preserve backward compatability, removed V0.3
	private int BarrelUpdateTime;	// How frequently to test barrels.
	private int ProximityTimeout;	// Deprecated - Keep to preserve backward compatability, removed V0.3
	private int DebugBarrels;
	private int DebugWeather;
	private int DebugYAY;
	// End of Config members // 


    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    void BBB_Settings() {
		BBB_Log.Log("[JSON] BBB_Settings()");


        BarrelLocations = new array<ref BBB_BarrelLocation>;

		JSON_MapAdd(this, "Version", "Version", string, void);
		JSON_MapAdd(this, "BarrelLocation", "BarrelLocation", BBB_BarrelLocation, void);
    	JSON_MapAdd(this, "BarrelLocations", "BarrelLocations", array, BBB_BarrelLocation );
		JSON_MapAdd(this, "PlayerReportTime", "PlayerReportTime", int, void);
		JSON_MapAdd(this, "BarrelUpdateTime", "BarrelUpdateTime", int, void);
		JSON_MapAdd(this, "ProximityTimeout", "ProximityTimeout", int, void);
		JSON_MapAdd(this, "DebugBarrels", "DebugBarrels", int, void);
		JSON_MapAdd(this, "DebugWeather", "DebugWeather", int, void);
		JSON_MapAdd(this, "DebugYAY", "DebugYAY", int, void);
/*
		private string Version = "";
		private ref array<ref BBB_BarrelLocation> BarrelLocations;
		private int PlayerReportTime;	// Deprecated - Keep to preserve backward compatability, removed V0.3
		private int BarrelUpdateTime;	// How frequently to test barrels.
		private int ProximityTimeout;  // Deprecated - Keep to preserve backward compatability, removed V0.3
		private int DebugBarrels;
		private int DebugWeather;
		private int DebugYAY;
		*/
    }

    //--------------------------------------------------------------------------
	//! Add Barrel to the list.
	//! \param barrel \p BBB_BarrelLocation 
	void AddBarrel(ref BBB_BarrelLocation barrel)
	{
		if(barrel) BarrelLocations.Insert(barrel);
		//barrel._InitPostConfigLoad(BarrelLocations.Count()-1); <<< Broken expression (missing ';'?)
		barrel._InitPostConfigLoad(BarrelLocations.Count() - 1);
	}

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally. use 
	//! Use BBB_BarrelLocation.Remove() instead otherwise barrel will persist
	//! in world.
	void _RemoveBarrel(int barrelIDX)
	{
		if(barrelIDX < BarrelLocations.Count()) {

			//BBB_Log.Log("Removed Barrel=" + barrelIDX); 
			BarrelLocations.Remove(barrelIDX);
			//BBB_Log.Log("Barrel Count=" + BarrelLocations.Count());
			if(barrelIDX < BarrelLocations.Count()) {
				for(int a = barrelIDX; a < BarrelLocations.Count(); a++)
				{
					BarrelLocations[a]._SetID(a);
				}
			}
		}
	}

    //--------------------------------------------------------------------------
	//! Gets Barrel by index.
	//! \param barrelIDX \p int 
	//! \return \p BBB_BarrelLocation.
	ref BBB_BarrelLocation GetBarrel(int barrelIDX) {
		if(barrelIDX >= 0 && barrelIDX < BarrelLocations.Count())
		{
			return BarrelLocations[barrelIDX];
		}
		return null;
	}

    //--------------------------------------------------------------------------
	//! Count of Barrels.
	int GetBarrelCount() { 
		return BarrelLocations.Count(); 
	}

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    private void _Default()
    {   
        // Green mountain next to radio tower door.
        AddBarrel(new BBB_BarrelLocation("3710.81 0 5993.59"));
    }

    //--------------------------------------------------------------------------
	//! Gets config version.
	//! \return \p string.
	string GetVersion() { return Version; }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
	ref array<ref BBB_BarrelLocation> _GetBarrelLocations() { return BarrelLocations; }

    //--------------------------------------------------------------------------
	//! Gets BarrelUpdateTime.
	int GetBarrelUpdateTime() { return BarrelUpdateTime; }
    //--------------------------------------------------------------------------
	//! Gets DebugBarrels.
	int GetDebugBarrels() { return DebugBarrels; }
    //--------------------------------------------------------------------------
	//! Gets DebugWeather.
	int GetDebugWeather() { return DebugWeather; }
    //--------------------------------------------------------------------------
	//! Gets DebugYAY.
	int GetDebugYAY() { return DebugYAY; }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    static ref BBB_Settings _Load()	{
		BBB_Settings settings = new BBB_Settings();

		if ( !FileExist( DAVEZ_CONFIG_ROOT_SERVER ) )
		{
			MakeDirectory( DAVEZ_CONFIG_ROOT_SERVER );
		}

		if (FileExist(BBB_CONFIG))
		{
			BBB_Json json = new BBB_Json();
			json.load(BBB_CONFIG);
			BBB_Log.Log("Post json.load dump...");
			json.dump(null, 0);

    		settings.JSON_Import(json.GetDocRoot());
			//json_save.save(BBB_CONFIG + ".json");
			//BBB_Json json = new BBB_Json();
			//JsonFileLoader<BBB_Settings>.JsonLoadFile(BBB_CONFIG, settings);
		}
		else 
		{
			BBB_Log.Log("Could not read settings : " + BBB_CONFIG + ". Placing default Barrels.");
			settings._Default();
		}

		// Validation...
		if(settings.GetBarrelUpdateTime() <=0)
		{
			BBB_Log.Log("BarrelUpdateTime of " + settings.BarrelUpdateTime + " is an invalid. Setting default value (5)");
			settings.BarrelUpdateTime = 5;
		}

		if(settings.DebugWeather <0)
		{
			BBB_Log.Log("Setting DebugWeather needs to be positive. Setting default (0)");
			settings.DebugWeather = 0;
		}
		if(settings.DebugYAY < 0) 
		{
			BBB_Log.Log("Setting DebugYAY needs to be positive. Setting default (0)");
			settings.DebugWeather = 0;
		}

		if(settings.DebugBarrels !=0 && settings.DebugBarrels != 1)
		{
			BBB_Log.Log("Setting DebugBarrels needs to be 1 or 0. Setting default (0)");
			settings.DebugBarrels = 0;
		}

		// Init each Barrel.
		BBB_Log.Log("Loaded " + settings.BarrelLocations.Count() + " Barrels.");
		for(int barrelID = 0; barrelID < settings.BarrelLocations.Count(); barrelID++)
		{
			BBB_BarrelLocation b = settings.BarrelLocations.Get(barrelID);
			if(b) 
			{
				b._InitPostConfigLoad(barrelID);
			} else {
				BBB_Log.Log("Failed to fetch Barrel for _init(). This should not happen?");
			}
		}

		BBB_Log.DebugMode(settings.GetDebugBarrels());

		return settings;
	}
}