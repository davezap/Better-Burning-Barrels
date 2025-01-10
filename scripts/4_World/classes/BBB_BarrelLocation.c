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
//! BBB_BarrelLocation provides Barrels, Barrel options and player proximity.
/**
    @code
        // Add a Barrel.
        BBB_BarrelLocation newBarrel = new BBB_BarrelLocation("3706.2619 402.0124 5987.6875");
        newBarrel.SetOnDuringDay(true);
        newBarrel.SetOpen(false);
        newBarrel.SetColor("Green");
        settings.AddBarrel(newBarrel);

        // Delete a Barrel
        BBB_BarrelLocation newBarrel = settings.GetBarrel(3)
        if(newBarrel) newBarrel.Remove();

        // Move a Barrel
        barrel.SetPosition("3706.2619 402.0124 5987.6875");

        // Get count of players within OnPlayerProximity meters, and are not on
        // ListPlayers lists.
        barrel.PlayerProximityCount();

    @endcode
*/
class BBB_BarrelLocation: BBB_JsonMap
{
    // Start of Config members // 
    private string Type = "";
    private string Name = "";
    private vector Position;
    private float OnTimeFrom = 0.0;
	private float OnTimeTo = 0.0;
    private float OnTempAbove = 0.0;
    private float OnTempBelow = 0.0;
    private int OnDuringNight = 0;
    private int OnDuringDay = 0;
    private int OnPlayerProximity = 0;
    private int ListProximityA = 0;
    private int ListModeParamA = 0;
	private autoptr ref array<ref string> ListPlayersA;
    private int ListProximityB = 0;
    private int ListModeParamB = 0;
	private autoptr ref array<ref string> ListPlayersB;
    private int ListMode = 0;
    private int LogPlayerProximity = 0;
    private int Open = 1;
    private int Locked = 0;
    private int DontSnapToGround = 0;
    private string Colour = "";
    private string Color = "";
    private bool ATripod = false;
    private bool AStoneCircle = false;
    private int ExtinguishMethod = 0;
    private bool NoPain = false;
    // End of Config members // 

    private int m_iID = -1;
    private vector m_vHasMovedFrom;
    private bool m_vHasChangedColour;
    private bool m_vHasLockedOrOpened;
    private int m_iLastList0 = 0;
    private int m_iLastList1 = 0;
    private int m_iLastRandos = 0;
    private int m_iLastLoggers = 0;
    private autoptr ref map<string, ref BBB_PlayerProximity> ProxPlayers;
    private int m_iMaxProximity = 0;  // the greatest radius to check if players are in.
    private bool m_bIgnite;
    private bool m_bOut;
    private int m_extinguish_timer = 0;
    //private Object m_OBarrel = null;

    //--------------------------------------------------------------------------
	//! Constructor
    //! \param newLocation \p vector new location of barrel
    void BBB_BarrelLocation(vector objectPos)
    {
        ProxPlayers = new map<string, ref BBB_PlayerProximity>();
        ListPlayersA = new array<ref string>;
        ListPlayersB = new array<ref string>;

        JSON_MapAdd(this, "Type", "Type", string, void);
        JSON_MapAdd(this, "Name", "Name", string, void);
        JSON_MapAdd(this, "Position", "Position", vector, void);
        JSON_MapAdd(this, "OnTimeFrom", "OnTimeFrom", float, void);
        JSON_MapAdd(this, "OnTimeTo", "OnTimeTo", float, void);
        JSON_MapAdd(this, "OnTempAbove", "OnTempAbove", float, void);
        JSON_MapAdd(this, "OnTempBelow", "OnTempBelow", float, void);
        JSON_MapAdd(this, "OnDuringNight", "OnDuringNight", int, void);
        JSON_MapAdd(this, "OnDuringDay", "OnDuringDay", int, void);
        JSON_MapAdd(this, "OnPlayerProximity", "OnPlayerProximity", int, void);
        JSON_MapAdd(this, "ListProximityA", "ListProximityA", int, void);
        JSON_MapAdd(this, "ListModeParamA", "ListModeParamA", int, void);

        JSON_MapAdd(this, "ListPlayersA", "ListPlayersA", array, string);
        JSON_MapAdd(this, "ListProximityB", "ListProximityB", int, void);
        JSON_MapAdd(this, "ListModeParamB", "ListModeParamB", int, void);
        JSON_MapAdd(this, "ListPlayersB", "ListPlayersB", array, string);

        JSON_MapAdd(this, "ListMode", "ListMode", int, void);
        JSON_MapAdd(this, "LogPlayerProximity", "LogPlayerProximity", int, void);
        JSON_MapAdd(this, "Open", "Open", int, void);
        JSON_MapAdd(this, "Locked", "Locked", int, void);
        JSON_MapAdd(this, "DontSnapToGround", "DontSnapToGround", int, void);

        JSON_MapAdd(this, "Colour", "Colour", string, void);
        
        JSON_MapAdd(this, "ATripod", "Tripod", bool, void);
        JSON_MapAdd(this, "AStoneCircle", "Circle", bool, void);
        JSON_MapAdd(this, "ExtinguishMethod", "ExtinguishMethod", int, void);

        JSON_MapAdd(this, "NoPain", "NoPain", bool, void);



        m_iID = -1;
        Position = objectPos;
        if(Type!="Barrel" && Type != "Fireplace")
        {
            Type = "Barrel";
        }
    }

    string GetType()
    {
        return Type;
    }

    string GetClassName()
    {
        if(Type=="Barrel")
        {
            return "BarrelHoles_" + GetColor();
        }

        return Type;
    }

    string GetName()
    {
        return Name;
    }

    //--------------------------------------------------------------------------
	//! Flag barrel for destruction.
    //!
    //! This will cause barrel to be removed from world and 
    //! BBB_Settings::BarrelLocations array. The remaining barrels in the array
    //! will be reindexed.
    void Remove()
    {
        int oldID = m_iID;
        m_iID = -1;
        BBB_Log.LogEx("Marking for removal " + oldID + " to " + m_iID);
    }

    
    // Class constructors are not called it the object is restored via JsonLoadFile
    void _InitPostConfigLoad(int barrelIDX)
    {
        //BBB_Log.Log("Barrel:" + barrelIDX + " _InitPostConfigLoad()");
        // Config loaded Barrels set this to 0
        _SetID(-1);

        if(Type=="") Type = "Barrel";

        if(OnPlayerProximity || LogPlayerProximity || ListProximityA || ListProximityB)
        {
            _UpdateMaxProximity();
        }
    
        // validates colour
    	SetColour(GetColour());

        // Barrel is -1 till we set it.
        _SetID(barrelIDX);
    }
	
    private void _UpdateMaxProximity() {
        m_iMaxProximity = OnPlayerProximity;
        if(m_iMaxProximity < LogPlayerProximity) m_iMaxProximity = LogPlayerProximity;
        if(m_iMaxProximity < ListProximityA) m_iMaxProximity = ListProximityA;
        if(m_iMaxProximity < ListProximityB) m_iMaxProximity = ListProximityB;
        //BBB_Log.Log("_UpdateMaxProximity " + m_iMaxProximity + " " + ListProximityA);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Start of Config Setters and Getters.

    //--------------------------------------------------------------------------
	//! Get the Barrel On start time.
	//! \return \p float, DayZ float time.
    float GetOnTimeFrom() { return OnTimeFrom; }
	//--------------------------------------------------------------------------
	//! Set the Barrel On start time.
	//! \param time \p float, DayZ float time.
    void SetOnTimeFrom(float time) { OnTimeFrom = time; }

	//--------------------------------------------------------------------------
	//! Get the Barrel On end time.
	//! \return \p float, DayZ float time.
    float GetOnTimeTo() { return OnTimeTo; }
	//--------------------------------------------------------------------------
	//! Set the Barrel On end time.
	//! \param time \p float, DayZ float time.
    void SetOnTimeTo(float time) { OnTimeTo = time; }

	//--------------------------------------------------------------------------
	//! Get temperature above, Barrel will turn on if above this.
	//! \return \p float, DayZ temperature in degrees celsius .
    float GetOnTempAbove() { return OnTempAbove; }
    //--------------------------------------------------------------------------
	//! Set temperature above, Barrel will turn on if above this.
	//! \param temp \p float, DayZ temperature in degrees celsius .
    void SetOnTempAbove(float temp) { OnTempAbove = temp; }

	//--------------------------------------------------------------------------
	//! Get temperature below, Barrel will turn on if below this.
	//! \return \p float, DayZ temperature in degrees celsius .
    float GetOnTempBelow() { return OnTempBelow; }
    //--------------------------------------------------------------------------
	//! Set temperature below, Barrel will turn on if below this.
	//! \param temp \p float, DayZ temperature in degrees celsius .
    void SetOnTempBelow(float d) { OnTempBelow = d; }

    //--------------------------------------------------------------------------
	//! Gets On During Night setting.
	//! \return \p int, 1 (true) if enabled.
    int GetOnDuringNight() { return OnDuringNight; }
    //--------------------------------------------------------------------------
	//! Sets On During Night setting. 1==true
	//! \param int \p float, DayZ temperature in degrees celsius .
    void SetOnDuringNight(int d) { OnDuringNight = d; }

    //--------------------------------------------------------------------------
	//! Gets On During Day setting. 1==true
	//! \return \p int, 1 (true) if enabled.
    int GetOnDuringDay() { return OnDuringDay; }
    //--------------------------------------------------------------------------
	//! Sets On During Day setting. 1==true
	//! \param d \p int, 1 (true) equals enabled.
    void SetOnDuringDay(int d) { OnDuringDay = d; }

	//--------------------------------------------------------------------------
	//! Get OnPlayerProximity setting.
	//! \return \p int, distance in metres.
    int GetOnPlayerProximity() { return OnPlayerProximity; }
     //--------------------------------------------------------------------------
	//! Set OnPlayerProximity setting.
	//! \param d \p int, distance in metres.
    void SetOnPlayerProximity(int d)
    {
        OnPlayerProximity = d;
        _UpdateMaxProximity();
    }


	//--------------------------------------------------------------------------
	//! Get ListProximityA setting.
	//! \return \p int, distance in metres.
    int GetListProximityA() { return ListProximityA; }
    //--------------------------------------------------------------------------
	//! Set ListProximityA setting.
	//! \param d \p int, distance in metres.
    void SetListProximityA(int d)
    {
        ListProximityA = d;
        _UpdateMaxProximity();
    }
    //--------------------------------------------------------------------------
	//! Gets ListModeParamA setting.
	//! \return \p int, number of players required within ListProximityA.
    int GetListModeParamA() { return ListModeParamA; }
    //--------------------------------------------------------------------------
	//! Sets ListModeParamA setting.
	//! \param d \p int, number of players required within ListProximityA.
    void SetListModeParamA(int d) { ListModeParamA = d; }
    //--------------------------------------------------------------------------
	//! Gets ListPlayersA list.
	//! \return \p ref array<ref string>, reference to array of PlayerIDs.
    ref array<ref string> GetListPlayersA() { return ListPlayersA; }

	//--------------------------------------------------------------------------
	//! Get ListProximityB setting.
	//! \return \p int, distance in metres.
    int GetListProximityB() { return ListProximityB; }
     //--------------------------------------------------------------------------
	//! Set ListProximityB setting.
	//! \param d \p int, distance in metres.   
    void SetListProximityB(int d)
    {
        ListProximityB = d;
        _UpdateMaxProximity();
    }
    //--------------------------------------------------------------------------
	//! Gets ListModeParamB setting.
	//! \return \p int, number of players required within ListModeParamB.
    int GetListModeParamB() { return ListModeParamB; }
    //--------------------------------------------------------------------------
	//! Sets ListModeParamB setting.
	//! \param d \p int, number of players required within ListModeParamB.
    void setListModeParamB(int d) { ListModeParamB = d; }
    //--------------------------------------------------------------------------
	//! Gets ListPlayersB list.
	//! \return \p ref array<ref string>, reference to array of PlayerIDs.
    ref array<ref string> GetListPlayersB() { return ListPlayersB; }

	//--------------------------------------------------------------------------
	//! Get ListMode setting.
	//! \return \p int, mode.
    int GetListMode() { return ListMode; }
     //--------------------------------------------------------------------------
	//! Set ListMode setting.
	//! \param d \p int, mode.
    void SetListMode(int d) { ListMode = d; }


	//--------------------------------------------------------------------------
	//! Get LogPlayerProximity setting.
	//! \return \p int, distance in metres.  
    int GetLogPlayerProximity() { return LogPlayerProximity; }
    //--------------------------------------------------------------------------
	//! Set LogPlayerProximity setting.
	//! \param d \p int, distance in metres.  
    void setLogPlayerProximity(int d)
    {
        LogPlayerProximity = d;
        _UpdateMaxProximity();
    }

    //--------------------------------------------------------------------------
	//! Gets Open setting. 1==true
	//! \return \p int, 1 (true) if barrel lid is to be open by default.
    int GetOpen() { return Open; }
    //--------------------------------------------------------------------------
	//! Sets Open setting. 1==true
	//! \param d \p int, 1 (true) if barrel lid is to be open by default.
    void SetOpen(int d) { 
        if(Open != d) SetHasLockedOrOpened();
        Open = d;
    }

    //--------------------------------------------------------------------------
	//! Gets Locked setting. 1==true
	//! \return \p int, 1 (true) if barrel lid is to be locked by default.
    int GetLocked() { return Locked; }
    //--------------------------------------------------------------------------
	//! Sets Locked setting. 1==true
	//! \param d \p int, 1 (true) if barrel lid is to be locked by default.
    void SetLocked(int d) { 
        if(Open != d) SetHasLockedOrOpened();
        Locked = d; 
    }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    bool _GetHasLockedOrOpened() { 
        if(m_vHasLockedOrOpened)
        {
            m_vHasLockedOrOpened = false;
            return true; 
        }
        return false;
    }
	//--------------------------------------------------------------------------
	//! Call to update world barrel state.
    //!
    //! User scripts would not normally need to call this as SetLocked() and
    //! SetOpen() do it for you already.
    void SetHasLockedOrOpened() { m_vHasLockedOrOpened = true; }

    //--------------------------------------------------------------------------
	//! Gets DontSnapToGround setting. 1==true
	//! \return \p int, 1 (true) to use your barrel Position Y coordinate.
    int GetDontSnapToGround() { return DontSnapToGround; }
    //--------------------------------------------------------------------------
	//! Sets DontSnapToGround setting. 1==true
	//! \param d \p int, int, 1 (true) to use your barrel Position Y coordinate.
    void SetDontSnapToGround(int d) { DontSnapToGround = d; }

    //--------------------------------------------------------------------------
	//! Gets Colour setting.
	//! \return \p string, Barrel colour.
    string GetColour() { 
        if(Colour) return Colour;
        return Color;
    }
    //--------------------------------------------------------------------------
	//! Sets Colour setting.
	//! \param d \p string, Barrel colour.
    void SetColour(string colour)
    {
        // Check the only these colours have been given because this realtes
        // to class names.
        if(colour == "") colour = "none";
        colour = colour.Trim();
        colour.ToLower();
        switch(colour.Get(0))
        {
            case "b":
                colour = "Blue";
                break;
            case "g":
                colour = "Green";
                break;
            case "r":
                colour = "Red";
                break;
            case "y":
                colour = "Yellow";
                break;
            default:
                if(Colour != "")
                {
                    BBB_Log.Log("'" + Colour + "' is an invalid Barrel Colour. Please use Blue, Green, Red or Yellow. I choose Red.");
                } else if (Color != "") {
                    BBB_Log.Log("Howdy partner, '" + Color + "' is an invalid Barrel Color. Please use Blue, Green, Red or Yellow. I choose Red.");
                }
                colour = "Red";
                break;
        }
        Colour = colour;

        // This just forces BarrelTask() to recreate the barrel with new colour
        // Just don't do it to barrels that are not yet in the game world.
        if(m_iID!=-1) m_vHasChangedColour = true;
    }

    //--------------------------------------------------------------------------
	//! Gets Color setting.
	//! \return \p string, Barrel color.
    string GetColor() { 
        if(Colour) return Colour;
        return Color; 
    }
    //--------------------------------------------------------------------------
	//! Sets Color setting.
	//! \param d \p string, Barrel color.
    // Just like feet and pounds, we're really just faking it ;)
    void SetColor(string color) { SetColour(color); }

    void SetTripod(bool state) {ATripod = state;}
    bool GetTripod() { return ATripod;} 

    void SetCircle(bool state) {AStoneCircle = state;}
    bool GetCircle() { return AStoneCircle;} 

    void SetExtinguishMethod(bool state) {ExtinguishMethod = state;}
    bool GetExtinguishMethod() { return ExtinguishMethod;} 
    bool ExtinguishCheckStartTimer() {
        if (m_extinguish_timer == 0 && ExtinguishMethod > 0) {
            m_extinguish_timer = BBB_EH.GetServerTime() + ExtinguishMethod;
            return true;
        }
        return false;
    }
    void ExtinguishStopTimer() {
         m_extinguish_timer = 0;
    }
    bool ExtinguishTimeout () {
        if (m_extinguish_timer==0) return false;
        bool timeout = BBB_EH.GetServerTime() >= m_extinguish_timer;
        if (timeout) m_extinguish_timer = 0;
        return timeout;
    }

    bool GetNoPain() {return NoPain;}
    void SetNoPain(bool pain) {NoPain = pain;}
    // End of Config Setters and Getters.
    ////////////////////////////////////////////////////////////////////////////


	//--------------------------------------------------------------------------
	//! Get Barrel index/id 
	//! \return \p int, Index of barrel as it appeared in BarrelLocations
    int GetID() { return m_iID; }
    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    void _SetID(int new_id)
    {
        m_iID = new_id;
    }
    
 	//--------------------------------------------------------------------------
	//! Get Max Proximity 
	//! \return \p int, the largest of the proximity distance settings in metres.
    int GetMaxProximity() { return m_iMaxProximity; }

   

  	//--------------------------------------------------------------------------
	//! Returns true if the Barrel is currently burning. 
	//! \return \p bool, is ignited.
    bool GetIgnite() { return m_bIgnite; }
    //--------------------------------------------------------------------------
	//! Sets Set the Barrel burning state
	//! \param d \p bool, false=out, true= should be burning.
    void SetIgnite(bool d) { 
        m_bIgnite = d;
    }

    bool isOut(bool d) { 
        bool state_change = false;
        if(m_bOut != d) {
            state_change = true;
        }
        m_bOut = d;
        return state_change;
    }

    //--------------------------------------------------------------------------
	//! Get the game world barrel.
	//! \return \p BarrelHoles_ColorBase, DayZ itembase.
    //Object GetBarrelObject() { return m_OBarrel; }
	//--------------------------------------------------------------------------
	//! Set the game world barrel.
	//! \param barrel \p float, BarrelHoles_ColorBase, DayZ itembase.
    //void _SetBarrelObject(Object barrel) { m_OBarrel = barrel; }


  	//--------------------------------------------------------------------------
	//! Returns players who have recently pinged this Barrel, and they will only
    //! do so if within GetMaxProximity() distance. Barrels without proximity 
    //! settings will not receive pings. Players are removed from the list if 
    //! they do not ping for 
	//! \return \p ref map<string, ref BBB_PlayerProximity>, is ignited.
    ref map<string, ref BBB_PlayerProximity> GetProxPlayers()
    {
        return ProxPlayers;
    }

    //--------------------------------------------------------------------------
	//! Returns current world position of Barrel.
    //! \return \p vector
    bool fixed_pos = false;
    vector GetPosition()
    {
        if(!fixed_pos && DontSnapToGround!=1) 
        {
            fixed_pos = true;
            Position[1] = BBB_EH.GetGroundBelow(Position);
        }
        return Position;
    }

    //--------------------------------------------------------------------------
	//! Sets current world position of Barrel, observes DontSnapToGround option.
    //! \param newLocation \p vector new location of barrel
    void SetPosition(vector newLocation)
    {
        vector oldLocation = Position;
        Position = newLocation;
        fixed_pos = false;

        if(oldLocation != newLocation)
        {
            // Instruct MissionServer::BarrelTask to delete old one. 
            m_vHasMovedFrom = oldLocation;
        }

        // Update Proxyimity Player distances and list counts.
        _PlayersUpdate(0);
    }

    //--------------------------------------------------------------------------
	//! Returns old position of Barrel if SetPosition has been called.
    //! Automatically set to vector.Zero by BBB once move is complete.
    //! \return \p vector
    vector GetIsMoving()
    {
        return m_vHasMovedFrom;
    }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    void _Moved() 
    {
        m_vHasMovedFrom = vector.Zero;
    }

    //--------------------------------------------------------------------------
	//! Retruns true if Barrel is changing colour.
    //! \return \p bool
    bool GetIsChangingColour()
    {
        return m_vHasChangedColour;
    }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    void _ChangedColour() 
    {
        m_vHasChangedColour = false;
    }    

    //--------------------------------------------------------------------------
	//! Searches for PlayerID (Steam ID) and returns what list they are assigned
    //! to. Will be 0 = ListPlayersB, 1 = ListPlayersA, -1 = Player not found.
    //! \param playerID \p string
    int WhatList(string playerID)
    {
        foreach(string PlayerID1: ListPlayersA)
		{
            if(PlayerID1 == playerID) return 1;
        }
        foreach(string PlayerID0: ListPlayersB)
		{
            if(PlayerID0 == playerID) return 0;
        }  
        return -1;
    }

    //--------------------------------------------------------------------------
	//! Searches for PlayerID in ProxPlayers and returns the ProxPlayers key
    //! if found. 
    //! \param playerID \p string
    //!
    //! Note : For ProxPlayers key we use the DayZ UniqueID (hashed steamID, 
    //! database Xbox id...) This is not the same as the steamID that specify in
    //! ListPlayersA and ListPlayersB. Given DayZ console is not moddable this 
    //! is probably not an issue.
    string ProxPlayerByPlainID(string playerID)
    {
        foreach(string k, BBB_PlayerProximity p : ProxPlayers)
		{
            if(p.GetPlayerID() == playerID) return k;
        }
        return "";
    }

    private void UpdateLastListCounts(BBB_PlayerProximity player)
    {
        float distance = player.GetDistance();

        if(player.GetList() == 1) {
            if(distance < ListProximityA) m_iLastList1++;
        } else if(player.GetList() ==0) {
            if(distance < ListProximityB) m_iLastList0++;
        } else {
            if(distance < OnPlayerProximity) m_iLastRandos++;
        }

        if(distance <  LogPlayerProximity)
        {
            m_iLastLoggers++;
        }
    }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    bool _PlayerPing(PlayerBase playerSource, int playerTimeout)
    {
        float distance = DistanceTo(playerSource.GetPosition());

        // Don't check player so frequently if they are 100 meters away from
        // the edge of the largest detection zone. This should improve server
        // performance.
        if(distance - GetMaxProximity() > 100)
        {
            //BBB_Log.Log("Distance " + distance + " prox " + GetMaxProximity());
            return false;
        }

        string safeID = playerSource.GetIdentity().GetId();
        BBB_PlayerProximity player = ProxPlayers[safeID];
        int list;

        if(!player)
        {
            if(distance >= m_iMaxProximity)
            {
                return true;
            }
            
            ProxPlayers[safeID] = new BBB_PlayerProximity();
            player = ProxPlayers[safeID];
            player.Init(playerSource);
            list = WhatList(player.GetPlayerID());
            BBB_Log.LogEx("Barrel:" + m_iID + " - Added player " + player.GetName() + " (" + player.GetPlayerID() + ")");
            UpdateLastListCounts(player);   // immediately count new player.
        } else {
            if(distance >= m_iMaxProximity)
            {
                BBB_Log.LogEx("Barrel:" + m_iID + " - Removed player " + player.GetName() + " (" + player.GetPlayerID() + ")");
                ProxPlayers.Remove(safeID);
                return true;
            }
            list = WhatList(player.GetPlayerID());
        }

        player._SetHasPung(true);
        player._SetDistance(distance);
        player._SetList(list);
        player._ResetTimeout(playerTimeout);

        if(distance < LogPlayerProximity)
        {
            BBB_Log.Log(string.Format("%1 (%2) is %4 meters from barrel %3", player.GetName(), player.GetPlayerID(), GetID(), distance));
        }

        return true;
    }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    void _PlayersUpdate(int interval)  // Don't call x_ methods from user code.
    {
        // It's simpler and more reliable to update the Last counts each pass.
        m_iLastList0 = 0;
        m_iLastList1 = 0;
        m_iLastRandos = 0;
        m_iLastLoggers = 0;

        foreach(string k, BBB_PlayerProximity player : ProxPlayers)
		{
            if(player.IsReady())
            {
                if(player.GetState()==0)
                    BBB_Log.LogEx("Barrel:" + m_iID + " - Removed deleted player");
                else
                    BBB_Log.LogEx("Barrel:" + m_iID + " - Removed player " + player.GetName() + " (" + player.GetPlayerID() + ")");
                ProxPlayers.Remove(k);
            } else {
                //BBB_Log.Log("Barrel:" + m_iID + " Update player " + player.GetPlayerID() + "=" + player._UpdateTimer(interval));
                player._UpdateTimer(interval);
                if(player.GetDistance() == 0 || interval == 0)
                {
                     player._SetDistance(DistanceTo(player.GetPosition()));
                }

                UpdateLastListCounts(player);
            }
        }
        return;
    }

	//--------------------------------------------------------------------------
	//! Returns count of all players currently within LogPlayerProximity radius.
	//! \return \p int, count of players.
    int LogPlayerProximityCount()
    {
        return m_iLastLoggers;
    }

	//--------------------------------------------------------------------------
	//! Returns count of all players currently within OnPlayerProximity radius.
	//! \return \p int, count of players.
    int PlayerProximityCount()
    {
        return m_iLastRandos;
    }

	//--------------------------------------------------------------------------
	//! Returns count of ListPlayersA players currently within ListProximityA radius.
	//! \return \p int, count of players.
    int OnPlayerListCount()
    {
        return m_iLastList1;
    }

	//--------------------------------------------------------------------------
	//! Returns count of ListPlayersB players currently within ListProximityB radius.
	//! \return \p int, count of players.
    int OffPlayerListCount()
    {
        return m_iLastList0;
    }

	//--------------------------------------------------------------------------
	//! Returns Total players within List Proximities. 
    //! Bascially OnPlayerListCount() + OffPlayerListCount()
	//! \return \p int, count of players.
    int AllPlayerListCount()
    {
        return m_iLastList1 + m_iLastList0;
    }

	//--------------------------------------------------------------------------
	//! Returns the ratio OnPlayerListA to Total Players
    //! Basically OnPlayerListCount() / AllPlayerListCount()
	//! \return \p float, distance in metres.
    float GetListPlayerRatio()
    {
        if(m_iLastList1 + m_iLastList0 == 0) return -1;
        return m_iLastList1 / (m_iLastList1 + m_iLastList0);
    }

	//--------------------------------------------------------------------------
	//! Get distance from somePlace to this Barrel.
	//! \param somePlace \p vector, location of interest.
	//! \return \p float, distance in metres.
    float DistanceTo(vector somePlace)
    {
        return vector.Distance(Position, somePlace);
    }

  	//--------------------------------------------------------------------------
	//! As ShouldFire() but takes enum BBB_EIgnore options to suppress testing
    //! of particular items.
    //! \param ignore \p BBB_EIgnore, these can be OR'd togethered.
    //!
    //! For example, if in the ShouldFire() handler you use Player Proximity as
    //! an even for some of your own logic but still want the barrel to light
    //! with default logic.
    /**
        @code
            if(barrel.TestOnPlayerProximity())
		    {
                // Do our stuff based on Proximity.

                // But don't also turn the Barrel On because of it.
                return barrel.ShouldFireIgnore(BBB_EIgnore.PLAYERPROXIMITY);
            }
        @endcode
    */
    bool ShouldFireIgnore(BBB_EIgnore ignore)
    {
        
        bool shouldFire = false;
        bool onIgnite = false;
        if(TestUnconfigured())
        {
            //BBB_Log.LogEx(GetName() + " TestUnconfigured=True");
            if(ignore & BBB_EIgnore.UNCONFIGURED)
            {
                return false;
            } else {
                return true;
            }
        }
        if(TestOnDay() && !(ignore & BBB_EIgnore.DAY)) {
            onIgnite = true;
        }
        if(TestOnNight() && !(ignore & BBB_EIgnore.NIGHT)) {    
            onIgnite = true;
        }
        if(TestOnTemp() && !(ignore & BBB_EIgnore.TEMP)) onIgnite = true;
        if(TestOnTime() && !(ignore & BBB_EIgnore.TIME)) onIgnite = true;
        if(TestOnPlayerProximity() && !(ignore & BBB_EIgnore.PLAYERPROXIMITY)) onIgnite = true;
        
        if( (ListProximityA && !(ignore & BBB_EIgnore.PROXIMITYA)) || (ListProximityB && !(ignore & BBB_EIgnore.PROXIMITYB)) )
        {
            /// ListModes
            switch(ListMode)
            {
                case 0: 
                    ///0: ListModeParamB Barrel OFF state overrides ListProximityA Barrel ON state.
                    ///Both override all other Barrel "On..." settings
                    shouldFire = onIgnite;
                    if(TestOnPlayerList()) shouldFire = true;
                    if(TestOffPlayerList()) shouldFire = false;
                    break;
                case 1:
                    /// 1: ListModeParamA Barrel ON state overrides ListProximityB Barrel OFF state.
                    /// Both override all other Barrel "On..." settings
                    shouldFire = onIgnite;
                    if(TestOffPlayerList()) shouldFire = false;
                    if(TestOnPlayerList()) shouldFire = true;
                    break;
                case 2:
                    /// 2: As 0 except Barrel "On..." settings override both if true.
                    if(TestOnPlayerList()) shouldFire = true;
                    if(TestOffPlayerList()) shouldFire = false;
                    if(onIgnite == true) shouldFire = true;
                    break;
                case 3:
                    /// 3: As 1 except Barrel "On..." settings override both if true.
                    if(TestOffPlayerList()) shouldFire = false;
                    if(TestOnPlayerList()) shouldFire = true;
                    if(onIgnite == true) shouldFire = true;
                    break;
                case 4:
                    /// 4: As 0 except Barrel "On..." settings override both if false.
                    if(TestOnPlayerList()) shouldFire = true;
                    if(TestOffPlayerList()) shouldFire = false;
                    if(onIgnite == false) shouldFire = false;
                    break;
                case 5:
                    /// As 1 except Barrel "On..." settings override both if false.
                    if(TestOffPlayerList()) shouldFire = false;
                    if(TestOnPlayerList()) shouldFire = true;
                    if(onIgnite == false) shouldFire = false;
                    break;
                default:
                    shouldFire = false;
                    break;
            }

        } else {
            shouldFire = onIgnite;
        }

        return shouldFire;
    }

  	//--------------------------------------------------------------------------
	//! Performs all Tests and uses default logic to figure if the Barrel
    //! should be on or off.
    bool ShouldFire()
    {
        return ShouldFireIgnore(0);
    }

  	//--------------------------------------------------------------------------
	//! Returns true if no configuration settings are enabled, the default 
    //! behaviour for barrels with only a Position is to be on.
    bool TestUnconfigured()
    {
        int checkAllInts = OnDuringNight + OnDuringDay + OnPlayerProximity + ListProximityA + ListProximityB;
        float checkAllFloats = OnTimeFrom + OnTimeTo + OnTempAbove + OnTempBelow;

        if( checkAllInts==0 && checkAllFloats==0 ) return true;
        return false;
    }

  	//--------------------------------------------------------------------------
	//! Returns true if OnDuringDay is set and IsDay() is true
    bool TestOnDay()
    {
        if(OnDuringDay && BBB_EnvironmentHelper.IsDay()) return true;
        return false;
    }

  	//--------------------------------------------------------------------------
	//! Returns true if OnDuringNight is set and IsDay() is false
    bool TestOnNight()
    {
		if(OnDuringNight && BBB_EnvironmentHelper.IsDay()==false) return true;
        return false;
    }

    //--------------------------------------------------------------------------
    //! Returns true if OnTempAbove set and GetTemp() is higher or OnTempBelow
    //! is set and GetTemp() is lower.
    bool TestOnTemp()
    {
        if(OnTempAbove || OnTempBelow) {
            float temp = BBB_EnvironmentHelper.GetTemp();
            if(OnTempAbove != 0 && temp > OnTempAbove) return true;
            if(OnTempBelow != 0 && temp < OnTempBelow) return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------
    //! Returns true if OnTimeFrom and OnTimeTo are and GetGameTimeF() is
    //! within that range.
    //!
    //! NOTE: If OnTimeTo <= OnTimeFrom  it is assumed we are crossing midnight,
    //! For example OnTimeFrom=21 OnTimeTo=3. Otherwise it's two times on
    //! the same day.
    bool TestOnTime()
    {
			if(OnTimeFrom || OnTimeTo)
			{
				float time = BBB_EnvironmentHelper.GetGameTimeF();
				if(OnTimeTo <= OnTimeFrom) 
				{
					if(OnTimeFrom <= time || time < OnTimeTo) return true;
				} else {
					if(OnTimeFrom <= time && time < OnTimeTo) return true;
				}
			}
            return false;
    }

    //--------------------------------------------------------------------------
    //! Returns true if any player is within OnPlayerProximity meters.
    bool TestOnPlayerProximity()
    {
        return (m_iLastRandos > 0);
    }

    //--------------------------------------------------------------------------
    //! Returns true if any player is within LogPlayerProximity meters.
    bool TestLogPlayerProximity()
    {
        return (m_iLastLoggers > 0);
    }

    //--------------------------------------------------------------------------
    //! Returns true if any player is within ListProximityA meters.
    //! AKA On Players, because they turn the barrel on.
    bool TestOnPlayerList()
    {
        return (m_iLastList1 >= ListModeParamA);
    }

    //--------------------------------------------------------------------------
    //! Returns true if any player is within ListProximityB meters.
    //! AKA Off Players, because they turn the barrel off.
    bool TestOffPlayerList()
    {
        return (m_iLastList0 >= ListModeParamB);
    }
}


enum BBB_EIgnore
{
    //! Do not check TestOnTime()
    TIME = 1,
    //! Do not check TestOnTemp()
    TEMP = 2,
    //! Do not check TestOnDay()
    DAY = 4,
    //! Do not check TestOnNight()
    NIGHT = 8,
    //! Do not check TestOnPlayerProximity()
    PLAYERPROXIMITY = 16,
    //! Do not check ListProximityA and ListMode
    PROXIMITYA = 32,
    //! Do not check ListProximityB and ListMode
    PROXIMITYB = 64,
    //! Do not check TestUnconfigured(). In this casae ShouldFireIgnore() will always return
    //! false if this is used.
    UNCONFIGURED = 128
}