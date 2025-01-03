//! DayZ Mod : DaveZ's Better Burning Barrels (AKA BBB)
//! 
//! Visit the discord server to report bugs, ask questions or just say hello.
//! 	https://discord.gg/AqDzhMJr
//! 
//!   ******** Repacking is not permitted without prior approval ********
//! 
//! DO NOT call methods starting with _ from user code.
//!
//! Dynamic loading of Enforce script and handling handlers.
class BBB_Script
{
    ScriptModule MyMod;
    bool m_bScriptGood;
    bool m_bImplementsShouldFire;
    
    void BBB_Script(string file, BBB_Settings settings) 
    {
        if (!FileExist(file)) return;
        BBB_Log.Log("Injecting script " + file);
        MyMod = ScriptModule.LoadScript(GetGame().GetMission().MissionScript, file, false);
        if(!MyMod) {
            BBB_Log.Log("Script Injection Error");
            return;
        }
        m_bScriptGood = true;

        int a;
        
        bool returnVal;

        if(MyMod.CallFunctionParams(null, "ShouldFire", returnVal, new Param2<BBB_BarrelLocation, int>( null, 0 )))
        {
            m_bImplementsShouldFire = true;
        }

        // Call Init function.
        a = MyMod.CallFunction(null, "Init", null, settings);        
    }

    static ref BBB_Script Load(string file, BBB_Settings settings)
    {
        if (!FileExist(file)) return null;

        return new BBB_Script(file, settings);
    }

    void ~BBB_Script() 
    {
        MyMod.Release();
    }

    bool ShouldFire(BBB_BarrelLocation barrel, int interval) 
    {
        if(!m_bScriptGood || !m_bImplementsShouldFire) return false;

        bool returnVal = false;

        //int a = MyMod.CallFunction(null, "runner", returnVal, barrel);
        int a = MyMod.CallFunctionParams(null, "ShouldFire", returnVal, new Param2<BBB_BarrelLocation, int>( barrel, interval ));
        if(a) 
        {
            return returnVal;
        } else {
            return false;
        }
    }
}