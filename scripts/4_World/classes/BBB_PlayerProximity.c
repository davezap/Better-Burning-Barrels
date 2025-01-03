//! DayZ Mod : DaveZ's Better Burning Barrels (AKA BBB)
//! 
//! Visit the discord server to report bugs, ask questions or just say hello.
//! 	https://discord.gg/AqDzhMJr
//! 
//!   ******** Repacking is not permitted without prior approval ********
//!
//! DO NOT call methods starting with _ from user code.
//!
//! This class is used to manage players who are proximate to barrels.
class BBB_PlayerProximity
{
    private string m_sPlayerID;
    private float m_fDistance;
    private int m_iTimeout;
    private int m_iList;
    private bool m_bHasPung; // did player ping any barrels in the last barrel update?
    private PlayerBase m_Player;
    private PlayerIdentity m_Identity;
    private int m_iState;

    //--------------------------------------------------------------------------
	//! NOTE: Probably best not to create instances of this class and let the 
    //! BBB manage the BBB_BarrelLocation::ProxPlayers
    void Init(PlayerBase p)
    {
        m_Player = p;
        
        m_iList = -1;   // Default to rando.
        m_iState = 2;  // Default to online.
        if(m_Player)
        {
            m_Identity = m_Player.GetIdentity();
            m_sPlayerID = m_Identity.GetPlainId();
        }

    }

    //--------------------------------------------------------------------------
	//! Return SteamID of this player.
	//! \return \p string
    string GetPlayerID() { return m_sPlayerID; }

    //--------------------------------------------------------------------------
	//! Gets last player distance to Barrel.
	//! \return \p float, metres.
    float GetDistance() { return m_fDistance; }
    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    void _SetDistance(float d) { m_fDistance = d; }
    
    //--------------------------------------------------------------------------
	//! What list the player is a member of. -1=no list, 0=off list, 1=on list.
    int GetList() { return m_iList; }
    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    void _SetList(int d) { m_iList = d; }

    //--------------------------------------------------------------------------
	//! If this player has pinged the barrel.
    //!
    //! Note from the BBB_BarrelLocation::ProxPlayers perspective this will
    //! always be true. It's mainly of use internally.
    bool GetHasPung() { return m_bHasPung; }
    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    void _SetHasPung(bool d) { m_bHasPung = d; }

    int GetState() { return m_iState; }
    void _SetActive() { 
        m_iState = 2; 
        BBB_Log.LogEx("Player State 2");
    }
    void _SetLoggingOut() { 
        m_iState = 1; 
        BBB_Log.LogEx("Player State 1");
    }
    void _SetDeleted() { 
        m_iState = 0; 
        BBB_Log.LogEx("Player State 0");
    }

    //--------------------------------------------------------------------------
	//! Returns DayZ PlayerBase for this player.
    PlayerBase GetPlayer() { return m_Player; }

    //--------------------------------------------------------------------------
	//! Helper function returns PlayerBase::GetPosition() for this player.
	//! \return \p vector, world position.
    vector GetPosition()
    {
        if(m_Player) return m_Player.GetPosition();
        return vector.Zero;
    }

    //--------------------------------------------------------------------------
	//! Helper function returns PlayerBase::GetIdentity().GetName() for this 
    //! player.
	//! \return \p string, player name.
    string GetName()
    {
        if(m_Identity) return m_Identity.GetName();
        return "";
    }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    void _ResetTimeout(int t)
    {
        m_iTimeout = t;
    }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    int _UpdateTimer(int seconds)
    {
        m_iTimeout -= seconds;
        if(m_iTimeout < 0) m_iTimeout = 0;
        return m_iTimeout;
    }

    //--------------------------------------------------------------------------
	//! Returns true if timeout has expired. Not relevent for user scripts.
    bool IsReady()
    {
        return m_iTimeout == 0;
    }
}
