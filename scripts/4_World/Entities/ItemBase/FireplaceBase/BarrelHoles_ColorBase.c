//! DayZ Mod : DaveZ's Better Burning Barrels (AKA BBB)
//! 
//! Visit the discord server to report bugs, ask questions or just say hello.
//! 	https://discord.gg/AqDzhMJr
//! 
//!   ******** Repacking is not permitted without prior approval ********
//!
//! DO NOT call methods starting with _ from user code.
//!
//! modded class for BarrelHoles_ColorBase to prevent picking, open/close/lock
modded class BarrelHoles_ColorBase
{
	private bool m_bIsLocked = false;
	private autoptr ref Timer BarrelOpener;
    private bool m_bBetterBurningBarrel = false;
    private bool m_bPleaseNoHurty = false;

    //// Constructor ///////////////////////////////////////////////////////////

    void BarrelHoles_ColorBase()
    {
        BarrelOpener = new Timer();
    }

    //// Overrides /////////////////////////////////////////////////////////////

    override bool CanPutIntoHands( EntityAI parent )
	{
        //BBB_Log.LogEx("!CanPutIntoHands()")
        if (m_bBetterBurningBarrel) return false;
		return super.CanPutIntoHands( parent );
	}

    override void Open() {
        if(m_bIsLocked) return;
        super.Open();
        //BBB_Log.LogEx("!Open()")
    }

    override void Close() {
        if(m_bIsLocked) return;
        super.Close();
        //BBB_Log.LogEx("!Close()")
    }

    override bool IsKindling(ItemBase item) {
        //BBB_Log.LogEx("!IsKindling()")
        return super.IsKindling(item);
    }

    override bool IsFuel(ItemBase item) {
        //BBB_Log.LogEx("!IsFuel()")
        return super.IsFuel(item);
    }

    override bool CanExtinguishFire()
	{
        //BBB_Log.LogEx("!CanExtinguishFire()")
        if (m_bBetterBurningBarrel) return false;
		return super.CanExtinguishFire();
	}

    override void CreateAreaDamage()
    {
        if(m_bPleaseNoHurty) {
            DestroyAreaDamage();
        } else {
            super.CreateAreaDamage();
        }
    }

    //// Our Stuff /////////////////////////////////////////////////////////////

    bool GetBetterBurningBarrel() { return m_bBetterBurningBarrel; }
    void SetBetterBurningBarrel() { m_bBetterBurningBarrel = true; }

    // Sets open/closed ignores locked state.
    // NOTE: The patch for 0.5 to sound sound effect problems. When an action is
    //       performed on the server values are synced back to the client who
    //       then plays the sounds. This is the source of lots of strange sounds
    //       most notably when logging in to the game and hearing padlocks lock
    //       doors open and so on. These are not player-initiated events it is
    //       the server setting the initial state of objects. In the case of 
    //       BarrelHoles_ColorBase the client side OnVariablesSynchronized()
    //       does not / is not able? to distinguish between server or player
    //       raised events. Opening a barrel is just opening a barrel.
    //       BI must have patched this because they now have OpenLoad() and
    //       CloseLoad() methods in BarrelHoles_ColorBase that just set 
    //       SetSynchDirty() instead of forcing the SoundSynchRemote();
    //       OR it was always there and I did not notice it ;)
    //  https://dayzexplorer.zeroy.com/barrelholes__colorbase_8c_source.html
    void OverrideOpenState(int sOpen) 
    {
        if(sOpen) {
            OpenLoad();
        } else {
            CloseLoad();
        }
    }

	bool IsLocked()
	{
		return m_bIsLocked;
	}

    void Lock(float actiontime)
	{
		m_bIsLocked = true;
        if(actiontime) BarrelOpener.Run(actiontime, this, "Unlock", NULL,false);
	}
	
	void Unlock()
	{
		m_bIsLocked = false;
	}

    void DisableDamage(bool damage)
    {
        m_bPleaseNoHurty = damage;
    }

}