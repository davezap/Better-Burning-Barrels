

class BBB_Type<Class T>
{
    autoptr ref FireplaceBase base = null;

    void BBB_Type(T object)
    {
        base = object;
        BBB_Log.LogEx("New BBB_Type = " + T.ToString());

    }
}

class BBB_Types
{
    static FireplaceBase base = null;
    static BarrelHoles_ColorBase ourBarrelHoles = null;
    static Fireplace ourFireplace = null;
    static bool isSet = false;
    static string m_sType = "";

    static void Reset()
    {
        isSet = false;
        ourBarrelHoles = null;
        ourFireplace = null;
        base = null;
        m_sType = "";
    }

    static void InitCast(Object obj, string type) 
    {
        //BBB_Log.LogEx("InitCast:" + type + " obj=" + obj);
        isSet = false;
        ourBarrelHoles = null;
        ourFireplace = null;
        base = null;
        if(obj == null)
        {
            m_sType = "";
            return;
        }
        m_sType = type;
        if(type == "Barrel")
        {
            ourBarrelHoles = BarrelHoles_ColorBase.Cast( obj );    
        } else {
            ourFireplace = Fireplace.Cast( obj );
        }
        base = FireplaceBase.Cast(obj);
        isSet = true;
        //BBB_Log.LogEx("isSet:" + isSet + " base=" + base + " ourBarrelHoles=" + ourBarrelHoles + " ourFireplace=" + ourFireplace);
    }

    static bool GetBetterBurningBarrel()
    {
        if(ourBarrelHoles) return ourBarrelHoles.GetBetterBurningBarrel();
        if(ourFireplace) return ourFireplace.GetBetterBurningBarrel();
        return false;
    }
    static void SetBetterBurningBarrel() 
    {
        if(ourBarrelHoles) ourBarrelHoles.SetBetterBurningBarrel();
        if(ourFireplace) ourFireplace.SetBetterBurningBarrel();
    }


    /// Other stuff.

   static bool CanPutIntoHands( EntityAI parent )
	{
        if(ourBarrelHoles) return ourBarrelHoles.CanPutIntoHands( parent );
        return false;
	}

   static void Open() {
        if(ourBarrelHoles) ourBarrelHoles.Open();
    }

    static void Close() {
        if(ourBarrelHoles) ourBarrelHoles.Close();
    }

    static bool IsKindling(ItemBase item) {
        if(ourBarrelHoles) return ourBarrelHoles.IsKindling(item);
        if(ourFireplace) return ourFireplace.IsKindling(item);
        return false;
    }

    static bool IsFuel(ItemBase item) {
        if(ourBarrelHoles) return ourBarrelHoles.IsFuel(item);
        if(ourFireplace) return ourFireplace.IsFuel(item);
        return false;
    }

    static bool CanExtinguishFire()
	{
        if(ourBarrelHoles) return ourBarrelHoles.CanExtinguishFire();
        //if(ourFireplace) return ourFireplace.CanExtinguishFire();
        return false;
	}

    // ignores locked state.
    static void OverrideOpenState(int sOpen) 
    {
        if(ourBarrelHoles) ourBarrelHoles.OverrideOpenState(sOpen);
    }

	static bool IsLocked()
	{
		if(ourBarrelHoles) return ourBarrelHoles.IsLocked();
        return false;
	}

    static void Lock(float actiontime)
	{
        if(ourBarrelHoles) ourBarrelHoles.Lock(actiontime);
	}
	
	static void Unlock()
	{
		if(ourBarrelHoles) ourBarrelHoles.Unlock();
	}

    static bool GetTripod()
    {
        if(ourFireplace) return ourFireplace.GetTripod();
        return false;
    }

    static void SetTripod(bool state)
    {
        if(ourFireplace) ourFireplace.SetTripod(state);
    }

    static bool GetCircle()
    {
        if(ourFireplace) return ourFireplace.GetCircle();
        return false;
    }

    static void SetCircle(bool state)
    {
        if(ourFireplace) ourFireplace.SetCircle(state);
    }

    static void DisableDamage(bool state)
    {
        if(ourBarrelHoles) ourBarrelHoles.DisableDamage(state);
        if(ourFireplace) ourFireplace.DisableDamage(state);
    }
}