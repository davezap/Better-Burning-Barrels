//! DayZ Mod : DaveZ's Better Burning Barrels (AKA BBB)
//! 
//! Visit the discord server to report bugs, ask questions or just say hello.
//! 	https://discord.gg/AqDzhMJr
//! 
//!   ******** Repacking is not permitted without prior approval ********
//!
//! DO NOT call methods starting with _ from user code.
//!
//! CORE Barrels loop and logic.

modded class MissionServer
{	
	private autoptr ref BBB_Settings m_Settings;
	private autoptr ref BBB_Script DZS;
	private autoptr ref map<string, ref BBB_PlayerProximity> Players = new map<string, ref BBB_PlayerProximity>();

	//// Constructor ///////////////////////////////////////////////////////////

	void MissionServer()
	{
		BBB_Log.Log("DaveZ's Better Burning Barrels - Version 0.50");

		LoadSettings();

		BBB_Log.LogEx("-- Debuging Barrels --");

		BBB_EH.IsWorldLoaded();
	}

	//// Overrides /////////////////////////////////////////////////////////////
	
	override void OnMissionStart()
	{
		super.OnMissionStart();

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(BarrelTask, m_Settings.GetBarrelUpdateTime() * 1000, true);

		if(m_Settings.GetDebugWeather()) {
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(WeatherTask, 1000 * m_Settings.GetDebugWeather(), true);
		}

		if(m_Settings.GetDebugYAY()) {
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(YAYTask, 1000 * m_Settings.GetDebugYAY(), true);
		}

		DZS = BBB_Script.Load("$profile:DaveZ\\BBB.c", m_Settings);
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player,identity);

		if(identity)
		{
			string safeID = identity.GetId();

			BBB_Log.LogEx(string.Format("InvokeOnConnect %1 (%2) %3", identity.GetName(), identity.GetPlainId(), safeID));

			//if(!Players.Contains(safeID))
			//{
				//BBB_Log.Log("Added " + identity.GetPlainId() + " Players " + safeID);
				Players[safeID] = new BBB_PlayerProximity();
				Players[safeID].Init(player);
			//}
		}
	}

	override void OnEvent(EventType eventTypeId, Param params)
	{
		
		PlayerIdentity identity;
        PlayerBase player;
        int counter = 0;
         
		switch (eventTypeId)
		{	
		case ClientReconnectEventTypeID:
			ClientReconnectEventParams reconnectParams;
			Class.CastTo(reconnectParams, params);
			
			identity = reconnectParams.param1;
			Class.CastTo(player, reconnectParams.param2);
			if (!player)
			{
				Debug.Log("ClientReconnectEvent: Player is empty");
				return;
			}
			
			if(Players.Contains(identity.GetId())) {
				Players[identity.GetId()]._SetActive();
			} else {
				BBB_Log.LogEx("Already deleted?");
			}

			//OnClientReconnectEvent(identity, player);
			BBB_Log.LogEx(string.Format("Event ClientReconnectEventTypeID"));
			break;
		
		case ClientDisconnectedEventTypeID:
			ClientDisconnectedEventParams discoParams;
			Class.CastTo(discoParams, params);      
			
			identity = discoParams.param1;
			Class.CastTo(player, discoParams.param2);       
			int logoutTime = discoParams.param3;
			bool authFailed = discoParams.param4;

			if (!player)
			{
				BBB_Log.LogEx("ClientDisconnectenEvent: Player is empty");
				return;
			}
			if (!identity)
			{
				BBB_Log.LogEx("ClientDisconnectenEvent: identity is empty");
				return;
			}

			if(Players.Contains(identity.GetId())) {
				if(Players[identity.GetId()].GetState()==1)
					Players[identity.GetId()]._SetDeleted();
				else
					Players[identity.GetId()]._SetLoggingOut();
			} else {
				BBB_Log.LogEx("Already deleted?");
			}

			//OnClientDisconnectedEvent(identity, player, logoutTime, authFailed);
			BBB_Log.LogEx(string.Format("Event ClientDisconnectedEventTypeID %1 %2 %3", logoutTime, authFailed, identity.GetPlainId()));
			break;
			
		case LogoutCancelEventTypeID:
			LogoutCancelEventParams logoutCancelParams;
			
			Class.CastTo(logoutCancelParams, params);               
			Class.CastTo(player, logoutCancelParams.param1);
			identity = player.GetIdentity();
			if (identity)
			{
				// disable reconnecting to old char
				// GetGame().RemoveFromReconnectCache(identity.GetId());
				BBB_Log.LogEx("[Logout]: Player " + identity.GetId() + " cancelled");
				if(Players.Contains(identity.GetId())) {
					Players[identity.GetId()]._SetActive();
				} else {
					BBB_Log.LogEx("Already deleted?");
				}
				
			}
			else
			{
				BBB_Log.LogEx("[Logout]: Player cancelled"); 
			}
			//m_LogoutPlayers.Remove(player);
			//m_NewLogoutPlayers.Remove(player);
			BBB_Log.LogEx(string.Format("Event LogoutCancelEventTypeID"));
			break;
		}
		
		
		super.OnEvent(eventTypeId, params);
	}

	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		BBB_Log.LogEx(string.Format("PlayerDisconnected %1", uid));

		Players[uid]._SetDeleted();
		Players.Remove(uid);

		// Note: At this point, identity can be already deleted
		if (!player)
		{
			BBB_Log.LogEx("Skipping player " + uid + ", already removed");
		}
		
		super.PlayerDisconnected(player,identity,uid);

	}


	//// Our Stuff /////////////////////////////////////////////////////////////

	private void LoadSettings()
	{
		m_Settings = BBB_Settings._Load();
	}

	private Object SpawnBBBObjects(int barrelID, vector objectPos, string objectType)
	{
		// Orientation test
		// Object obj = GetGame().CreateObjectEx( objectType, objectPos, 0, RF_FRONT );
		Object obj = GetGame().CreateObject( objectType, objectPos, false, false, true );
		if(obj==null) return null;

		//obj.SetPosition(objectPos); // prevent automatic on ground placing
		BBB_Log.LogEx(objectType + ":"  + barrelID + " - Spawned '" + objectType + "' AT <" + objectPos + ">");

		return obj;
	}

	private void WeatherTask()
	{
		if(BBreak) return;
		BBB_EnvironmentHelper.Debug();
	}

	private void YAYTask()
	{
		if(BBreak) return;
		string playerList;
		int cnt = Players.Count();
		foreach(string k, BBB_PlayerProximity p : Players)
		{
			playerList += p.GetName() + " (" + p.GetPlayerID() + ") @ " + p.GetPosition() + " | ";
			
			
		}
		BBB_Log.Log("YAY " + cnt + " Players | " + playerList);
	}

	private void PlayerTask(BBB_BarrelLocation barrelConfig, bool firstBarrel, bool lastBarrel)
	{
		if(BBreak) return;
		foreach(string k, BBB_PlayerProximity p : Players)
		{
			PlayerBase player = p.GetPlayer();
			if(player)
			{
				if(firstBarrel) 
				{	
					//BBB_Log.Log("(" + p.GetPlayerID() + ") _UpdateTimer " + p._UpdateTimer(m_Settings.GetBarrelUpdateTime()));
					//BBB_Log.Log("There are " + Players.Count() + " players");
					p._UpdateTimer(m_Settings.GetBarrelUpdateTime());
					p._SetHasPung(false);
				} 

				if (p.IsReady())
				{
					//if(firstBarrel) BBB_Log.Log("(" + p.GetPlayerID() + ") Pl ");
					PlayerIdentity identity;
					identity = player.GetIdentity();

					if(identity)
					{
						//Print("[BBB] Barrel " + barrelConfig.GetID() + " Ping");
						if(barrelConfig._PlayerPing(player, m_Settings.GetBarrelUpdateTime() + 2))
						{
							p._SetHasPung(true);
						}						
					}

					if(lastBarrel && !p.GetHasPung())
					{
						//BBB_Log.Log("(" + p.GetPlayerID() + ") LastBarrel");
						p._ResetTimeout(30);
					}
				} 
			}			
		}
		UpdateCorpseStatesServer();
	}

	bool insertItems(array<string> itemTypes)
	{
		foreach(string itemType: itemTypes)
		{
			ItemBase obj = ItemBase.Cast(BBB_Types.base.GetInventory().CreateAttachment(itemType));
			if (obj) {
				
				
				if (itemType=="Tripod") {
					BBB_Log.LogEx("Adding Pot to Tripod");
					obj.GetInventory().CreateAttachment("Pot");
				} else {
					obj.SetQuantityMax();
				}
				
			}
		}
		return true;
	}
	

	bool ItemMaintenance(BBB_BarrelLocation barrelConfig)
	{
		array<string> baseItemTypes = {"Paper", "WoodenStick", "BandageDressing", "Rag", "Bark_Oak", "Bark_Birch", "Firewood"};

		if(BBB_Types.m_sType=="Fireplace")
		{
			if(barrelConfig.GetCircle()) {
				baseItemTypes.Insert("Stone");
			}
			if (barrelConfig.GetTripod()) {
				BBB_Log.LogEx("Barrel:" + bname + " - Adding tripod to fireplace");
				baseItemTypes.Insert("Tripod");
			}
		}
		
		insertItems(baseItemTypes);

		if(BBB_Types.m_sType=="Fireplace")
		{
			// TODO: I was hoping this would make just a ground fire.
			// but it just fails to spawn the fireplace at all?
			if(barrelConfig.GetCircle()) {
				BBB_Types.base.SetStoneCircleState(true);
				BBB_Log.LogEx("Barrel:" + bname + " Adding stone circle to fireplace");
			}
		}

		return true;
	}

	string bname = "";
	bool WorldLoaded = false;
	bool BBreak = false;
    //--------------------------------------------------------------------------
	//! Handles spawning, maintanence and state changes of barrels.
	private void BarrelTask()
	{
		if(BBreak) return;
		if(!WorldLoaded) {
			if(BBB_EH.IsWorldLoaded()) {
				WorldLoaded = true;
			} else {
				return;
				
			}
		}

		int barrelID;
		int barrelCount = m_Settings.GetBarrelCount();
		int firstProxBarrel = -1;
		int lastProxBarrel = 0;

		for(barrelID = barrelCount - 1; barrelID > 0 ; barrelID--)
		{
			if(m_Settings.GetBarrel(barrelID).GetMaxProximity())
			{
				lastProxBarrel = barrelID;
				barrelID = 1; // break;
			}
			//BBB_Log.Log("lastProxBarrel=" + lastProxBarrel);
		}
		
		for(barrelID = 0; barrelID < barrelCount; barrelID++)
		{
			BBB_Types.Reset();
			BBB_BarrelLocation barrelConfig = m_Settings.GetBarrel(barrelID);

			bname = barrelConfig.GetName();
			if (bname=="") {bname = "#" + barrelID.ToString()}

			////////////////////////////////////////////////////////////////////
			// Update players proximity to barrel/barrels if needed.
			//
			//Print("[BBB] Barrel:"  + barrelID + " MaxProximity=" + barrelConfig.GetMaxProximity())
			if(barrelConfig.GetID()!=-1)
				barrelConfig._PlayersUpdate(m_Settings.GetBarrelUpdateTime());
			
			if(barrelConfig.GetMaxProximity())
			{
				//bool firstb = barrelID==0;
				//bool lastb = barrelID==(barrelCount-1); // Enforce cannot do this. Broken expression (missing ';'?)
				//bool lastb = barrelID==last_barel_idx;
				if(firstProxBarrel==-1) 
				{
					firstProxBarrel = barrelID;
				}
				PlayerTask(barrelConfig, barrelID==firstProxBarrel, barrelID==lastProxBarrel);
			}

			int pc = 0;
			vector boundingBox = "1 1 1";
			array<Object> objectsExclude = new array<Object>;
			array<Object> objectsNearby = new array<Object>;
			vector objectPos = barrelConfig.GetPosition();
			//BBB_Log.LogEx("Barrel:" + bname + " Pos=" + objectPos);
			// Delete any barrels in the vacinity.
			GetGame().IsBoxColliding( objectPos, vector.Zero, boundingBox, objectsExclude, objectsNearby);
			
			int m = objectsNearby.Count();

			if(m > 1) {
				int deleted = 0;

				// find the barrel by GetBetterBurningBarrel, note this is lost after
				// a server restart so we also need the proximity locator after.
				// this.
				for (pc = 0; pc < objectsNearby.Count(); pc++)
				{
					if (objectsNearby.Get(pc).GetType().IndexOf( barrelConfig.GetType() )==0)
					//if (ourBarrelHoles==null && objectsNearby.Get(pc).GetType() == barrelConfig.GetClassName())
					{
						//Print(objectsNearby.Get(pc).GetType())
						BBB_Types.InitCast(objectsNearby.Get(pc), barrelConfig.GetType());
						if(BBB_Types.GetBetterBurningBarrel()) {
							//BBB_Log.LogEx("Barrel:" + barrelID + " - We found our marked barrel.");
							break;
						}
					}
				}

				// If that did not work then try find the barrel closest to our objectPos.
				// it 'should be' impossible for a user to place a barrel at d=0.
				if(! BBB_Types.isSet ) 
				{
					// The reason we get here is the server restart the modded members 
					// are not stored? So the barrel that was previously marked as 
					// IsInSafezon is false.

					float distance = 1000;
					for (pc = 0; pc < objectsNearby.Count(); pc++)
					{
						if (objectsNearby.Get(pc).GetType().IndexOf( barrelConfig.GetType() )==0)
						//if (objectsNearby.Get(pc).GetType() == barrelConfig.GetClassName())
						{
							float d = vector.Distance(objectPos, objectsNearby.Get(pc).GetPosition());
							BBB_Log.LogEx("[BBB] Barrel:" + bname + " - Distance " + d);
							if(d < distance) {
								BBB_Types.InitCast(objectsNearby.Get(pc), barrelConfig.GetType());
								distance = d;
							}
						}
					}
					if( BBB_Types.isSet ) BBB_Log.LogEx("Barrel:" + bname + " - We found our barrel by distance");
				}

				// Remove the extra ones now. If we failed to find our barrel then this will
				// also delete all barrels and one will be created in the next OnUpdate.
				for (pc = 0; pc < objectsNearby.Count(); pc++)
				{
					if(objectsNearby.Get(pc) != BBB_Types.base) {
						if (objectsNearby.Get(pc).GetType().IndexOf( barrelConfig.GetType() )==0)
						//if (objectsNearby.Get(pc).GetType() != barrelConfig.GetClassName())
						{
							GetGame().ObjectDelete(objectsNearby.Get(pc));
							deleted++;
						}
					}
				}
				m -= deleted;
				
				if(deleted) {
					BBB_Log.LogEx("Barrel:" + bname + " - Extra Barrels Deleted " + deleted);
				}
			} 
			else if (m == 1)	// One item in the location.
			{
				if (objectsNearby.Get(0).GetType().IndexOf( barrelConfig.GetType() )==0) {
				//if (objectsNearby.Get(0).GetType() == barrelConfig.GetClassName()) {
					//BBB_Log.LogEx("Barrel:" + bname + " - Found 1 Barrel");
					BBB_Types.InitCast(objectsNearby.Get(0), barrelConfig.GetType());
				} else {
					// ok so this can't be our barrel, probably some other 
					// object so force spawn one.
					m = 0;
				}
			}

			bool isChangingColour = barrelConfig.GetIsChangingColour();
			vector isMoving = barrelConfig.GetIsMoving();
			int internalID = barrelConfig.GetID();
			if(m && (isMoving || internalID == -1 || isChangingColour))
			{
				if(isChangingColour)
				{
					BBB_Log.LogEx("Barrel:" + bname + " changing colour to " + barrelConfig.GetColour() );
				}
				if(isMoving)
				{
					BBB_Log.LogEx("Barrel:" + bname + " Moved from " + isMoving + " to " + objectPos );
				}

				if(internalID == -1 || (isChangingColour && isMoving == vector.Zero))
				{
					isMoving = barrelConfig.GetPosition();
				}
				GetGame().IsBoxColliding( isMoving, vector.Zero, boundingBox, objectsExclude, objectsNearby);
				for (pc = 0; pc < objectsNearby.Count(); pc++)
				{
					if (objectsNearby.Get(pc).GetType().IndexOf( barrelConfig.GetType() )==0)
					//if (objectsNearby.Get(pc).GetType() != barrelConfig.GetClassName())
					{
						BBB_Log.LogEx("Barrel:" + bname + " Deleted old Barrel " + pc + " at " + isMoving);
						GetGame().ObjectDelete(objectsNearby.Get(pc));
					}
				}
				barrelConfig._Moved();
				if(isChangingColour) {
					m = 0;
					barrelConfig._ChangedColour();
				}
				// Check if Barrel has been flagged for removal with Remove().
				//BBB_Log.Log("Barrel ID=" + barrelConfig.GetID());
				if(internalID == -1)
				{
					BBB_Log.Log("Barrel: internalID==-1 Removing Barrel " + barrelID);
					/*
					if(barrelConfig.GetBarrelObject())
					{
						GetGame().ObjectDelete(barrelConfig.GetBarrelObject());
						barrelConfig._SetBarrelObject(null);
					}
					*/
					m_Settings._RemoveBarrel(barrelID);
					barrelCount--;
					if(barrelID <= lastProxBarrel) lastProxBarrel--;
					continue;
				}
				
			}

			if (m == 0 || !BBB_Types.isSet) // Whoops!, we misplaced our barrel. Respawn it.
			{
				BBB_Log.LogEx("Barrel:" + bname + " - Found barrels:0");
				Object obj = SpawnBBBObjects(barrelID, objectPos, barrelConfig.GetClassName());
				
				if(obj==null)
				{
					BBB_Log.LogEx("Barrel:" + bname + " - Failed to spawn in barrel at " + objectPos);
					return;
				}

				BBB_Types.InitCast(obj, barrelConfig.GetType());
				
				if(!BBB_Types.isSet) // fail-safe...
				{
					BBB_Log.LogEx("Barrel:" + bname + " - Failed to get our barrel at " + objectPos + " m=" + m);
					continue;
				}

				ItemMaintenance(barrelConfig);
			}



			////////////////////////////////////////////////////////////////////
			// if for any reason our barrel is not marked, do it now.
			// This will be true of any newly spawned barrels.
			if(BBB_Types.GetBetterBurningBarrel() == false)
			{
				BBB_Log.LogEx("Barrel:" + bname + " - Marking the barrel as ours.");
				// to prevent people picking them up and putting out.
				BBB_Types.SetBetterBurningBarrel();
				//barrelConfig._SetBarrelObject(ourBarrelHoles);
				// force to update this state.
				barrelConfig.SetHasLockedOrOpened();
				//
				ItemMaintenance(barrelConfig);
			} else {
				// Update our barrel config open state with the actual open state
				// if it has changed only.
				//if(barrelID == 0) BBB_Log.LogEx("1 B " + BBB_Types.base.IsOpen() + " O "+ barrelConfig.GetOpen());
				barrelConfig.SetOpen(BBB_Types.base.IsOpen());
			}


			////////////////////////////////////////////////////////////////////
			// Determin if the barrel should be on fire or not based on config settings.
			//
			if(DZS)
			{
				// User call backs if provided.
				if(DZS.m_bImplementsShouldFire)
				{	
					barrelConfig.SetIgnite(DZS.ShouldFire(barrelConfig, m_Settings.GetBarrelUpdateTime()));
				} else {
					// default logic.
					barrelConfig.SetIgnite(barrelConfig.ShouldFire());					
				}
			} else {
				// default logic.
				barrelConfig.SetIgnite(barrelConfig.ShouldFire());
			}


			////////////////////////////////////////////////////////////////////
			// Ignite or Extinguish the fire.
			//
			//Print("[BBB] Barrel:"  + barrelID + " ignite " + ignite); 
			if (BBB_Types.base.IsWet()) {
				// dry it out.
				BBB_Types.base.SetWet(BBB_Types.base.GetWetMin());
			}
			

			if(BBB_Types.base.IsBurning() && barrelConfig.GetIgnite() == false){

				BBB_Log.Log("Barrel:"  + bname + " - Extinguish.");
				array<EntityAI> subItemsExit = new array<EntityAI>;
				BBB_Types.base.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, subItemsExit);
				for (int i = 0; i < subItemsExit.Count(); i++)
				{
					ItemBase item = ItemBase.Cast( subItemsExit.Get(i) );
					if(BBB_Types.IsKindling( item ) || BBB_Types.IsFuel( item )) //subItemsExit.Get(i) != ourBarrelHoles
					{
						BBB_Types.base.GetInventory().LocalDestroyEntity(subItemsExit.Get(i));
					}
				}

			}
			else if (!BBB_Types.base.IsBurning() && !BBB_Types.base.IsWet() && barrelConfig.GetIgnite() == false)
			{
				if(barrelConfig.isOut(true))
				{
					ItemMaintenance(barrelConfig);
					BBB_Log.LogEx("Barrel:" + bname + " - Did Extinguish.");
				}

			}
			else if (BBB_Types.base.IsBurning() && !BBB_Types.base.IsWet() && barrelConfig.GetIgnite() == true)
			{
				if(barrelConfig.isOut(false)) {
					BBB_Log.LogEx("Barrel:" + bname + " - Did Relight ");
				}
			}
			else if (!BBB_Types.base.IsBurning() && !BBB_Types.base.IsWet() && barrelConfig.GetIgnite() == true)
			{
				// Closed barrels cannot be filled with fule so open it while we do this.
				bool rememberBarrelWasOpen = BBB_Types.base.IsOpen();
				if(!rememberBarrelWasOpen) BBB_Types.OverrideOpenState(1);

				//Print("[BBB] Barrel:"  + barrelID + " Fill" + " IsBurning " + ourBarrelHoles.IsBurning() + " IsWet " + ourBarrelHoles.IsWet() );
				BBB_Log.Log("Barrel:"  + bname + " - Relight.");
				//BBreak = true;
				
				ItemMaintenance(barrelConfig);

				EntityAI source;
				BBB_Types.base.OnIgnitedThis(source);

				// return the barrel to it's previous state. 
				if(!rememberBarrelWasOpen) BBB_Types.OverrideOpenState(0);
			}


			// BarrelHoles_ColorBase.c modded to add the lock/unlock functionality.
			
			if(barrelConfig._GetHasLockedOrOpened())
			{
				BBB_Types.OverrideOpenState(barrelConfig.GetOpen());
				//if(barrelID == 0) BBB_Log.LogEx("2 B " + BBB_Types.base.IsOpen() + " O "+ barrelConfig.GetOpen());
				if(barrelConfig.GetLocked()) {
					BBB_Types.Lock(0);
				} else {
					BBB_Types.Unlock();
				}
			}
		}
	}

}